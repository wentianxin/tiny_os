#ifndef __KERN_MM_MEMLAYOUT_H__
#define __KERN_MM_MEMLAYOUT_H__

/* 全局段寄存器 */

/* 全局描述符 */


/* 虚拟内存布局  */
#define KERNBASE 0xC0000000    // 虚拟内存起始地址
#define KMEMSIZE 0x38000000    // 最大内存 896M
#define KERNTOP  (KERNBASE + KMEMSIZE) // 虚拟内存的边界

#define VPT      0xFAC0000    // 二级页表的虚拟起始地址， 通过映射关系, 使得 VPT ~ VPT + 4M(4M中1M个二级页表项)完全映射4G内存

#define KSTACKPAGE  2         // 栈
#define KSTACKSIZE  (KSTACKPAGE * PGSIZE)

#include <types.h>
#include <atomic.h>
#include <list.h>

/**
 * 再次体现了指针的神奇之处
 * 		pde_t  pgdir 是页目录项的具体内容
 * 		pde_t *pgdir 是页目录项的地址; 页目录的起始地址 pgdir[n]为具体页目录项
 */
typedef uintptr_t pte_t;   // 页表项
typedef uintptr_t pde_t;   // 页目录项

/* 物理内存探测相关结构 */
#define E820MAX  20    // e820map map最大长度
#define E820_ARM 1     // 内存类型
#define E820_ARR 2
struct e820map {
	int nr_map;        // map数量
	struct {
		uint64_t addr; // 起始地址
		uint64_t size; // 长度
		uint32_t type; // 类型
	} __attribute__((packed) map[E820MAX]);
};

/* 页帧 */
struct Page {
	atmoic_t ref;   // 原子变量，引用的数量
	uint32_t flags; // 标志位
	list_entry_t page_link; /// 链表连接点
};

/* 存放空闲页帧的链表 */
typedef struct {
	list_entry_t free_list;
	unsigned int nr_free; // 空闲量
} free_area_t;


/* Page(页帧) 成员flags(32bit) 位的含义 */
#define PG_RESERVED  0   // flags 0位 : 标志该页帧是否保留 1:保留(即该页帧不是空闲页,无法分配与释放)； 0:未保留
#define PG_PROPERTY  1   // flags 1位 : 标志该页帧是否分配 1:分配； 0:未分配

/* 对PG_RESERVED位的操作 */
#define SetPageReserved(page)   set_bit(PG_RESERVED, &(page->flags))
#define ClearPageREserved(page) clear_bit(PG_RESERVED, &(page->flags))
#define PageReserved(page)      test_bit(PG_RESERVED, &(page->flags))

/* 页帧链表项转化为对应的页帧结构体(根据结构体成员的偏移量计算结构体首地址) */
#define le2page (le)						 \
	to_struct((le), struct Page, page_link)


#endif   /* !__KERN_MM_MEMLAYOUT_H__ */
