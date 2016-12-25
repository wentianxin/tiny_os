#ifndef __KERN_PROCESS_PROC_H__
#define __KERN_PROCESS_PROC_H__

#include <type.h>

enum proc_state {
	PROC_UNINIT = 0,
	PROC_SLEEPING,
	PROC_RUNNABLE,
	PROC_ZOMBIE,
};

/**
 * 对于 x86而言，所拥有的寄存器
 * 4个数据寄存器(ax, bx, cx, dx)
 * 2个指针寄存器(esp栈指针, ebp帧指针)
 * 6个段寄存器  (es, cs, ds, ss, fs, gs)
 * 1个指令指针寄存器 (EIP)
 * 1个标志寄存器 (EFlags)
 */
struct context {
    uint32_t eip;
    uint32_t esp;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
};

struct proc_struct {
	int pid;
	char name[PROC_NAME_LEN + 1];
	uintptr_t kstack;
	uintptr_t cr3;
	uint32_t flags;  // flags 暂时不清楚作用
	int runs;        // 运行时间
	volatile bool need_resched;
	struct proc_struct *parent;
	enum proc_state state;
	struct context context;
	struct mm_strcut *mm;
	struct trapframe *tf;
	list_entry_t list_link;
	list_entry_t hash_link;
};

extern list_entry_t proc_list;

extern struct proc_struct *idle_proc, *init_proc, *current;


#endif
