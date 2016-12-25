#include <proc.h>

list_entry_t proc_list;

struct proc_struct *current  = NULL;
/* 一个特殊的线程 : 对ucore的管理工作，自己初始化自己 */
struct proc_struct *idleproc = NULL;
struct proc_struct *initproc = NULL;


static proc_struct *alloc_proc(void) {
	struct proc_struct *proc = (proc_struct *) kmalloc(sizeof(struct proc_struct));
	if (proc != NULL) {
		proc->state = PROC_UNINT;
		proc->pid = -1;
		proc->runs = 0;
		proc->kstack = 0;
		proc->need_resched = 0;
		proc->parent = NULL;
		proc->mm = NULL;
		proc->tf = NULL;

		memset(&proc->context, 0, sizeof(struct context));
		memset(proc->name, 0, sizeof(proc->name));

		proc->flags = 0;

		proc->cr3 = boot_cr3;
	}
}

static int setup_kstack(struct proc_struct *proc) {
	struct Page *page = alloc_pages(KSTACKPAGE);
	if (page == NULL) {
		return -E_NO_MEM;
	}

	proc->kstack = (uintptr_t)page2kva(page); //从page指针的虚拟地址转到对应页的物理地址，再到页的虚拟地址
	return 0;
}

static int copy_mm(uint32_t clone_flags, struct proc_struct *proc) {
	/* do nothing */
	return 0;
}

/**
 * 设置进程在内核（将来也包括用户态）正常运行和调度所需的中断帧和执行上下文
 */
static void copy_thread(struct proc_struct *proc, uintptr_t esp,
		struct trapframe *tf) {
	proc->tf = (struct trapframe *)(proc->kstack + KSTACKSIZE) - 1;
	*(proc->tf) = *tf;

	proc->context.eip = (uintptr_t)forkret;
	proc->context.esp = (uintptr_t)proc->tf;

}

int do_fork(uint32_t clone_flags, uintptr_t stack, struct trapframe *tf) {
	int ret = -E_NO_FREE_PROC;
	struct proc_struct *proc = NULL;
	if (nr_process >= MAX_PROCESS) {

	}
	if ((proc = alloc_proc()) == NULL) {

	}

	proc->parent = current;


	// 内存栈分配
	if (setup_kstack(proc) != 0) {

	}

	// 虚拟内存
	if (copy_mm(clone_flags, proc) != 0) {

	}

	copy_thread(proc, stack, tf);

	bool intr_flag;
	local_intr_save(intr_flag);
	{
        proc->pid = get_pid();
        hash_proc(proc);
        list_add(&proc_list, &(proc->list_link));
        nr_process ++;
	}
	local_intr_restore(intr_flag);

	wakeup_proc(proc);

	ret - proc->pid;

	return ret;
}


/**
 * create kernel thread
 * 创建临时 trapframe: 设置该线程的入口(即设置trapframe的 cs, eip)
 * 存放函数指针 和 相关参数
 */
int kernel_thread(int (*fn)(void *), void *arg, uint32_t clone_flags) {
	struct trapframe tf;
	memset(&tf, 0, sizeof(struct trapframe));
	tf.cs = KERNEL_CS;
	tf.ds = tf.es = tf.ss = KERNAL_DS;

	tf.eip = kernel_thread_entry;
	tf.tf_regs.reg_ebx = fn;
	tf.tf_regs.reg_edx = arg;

	return do_fork(clone_flags | CLONE_VM, 0, &tf);
}

void forkret(void) {
	forkrets();
}

void proc_run(struct proc_struct *proc) {
	if (proc != current) {
		bool intr_flag;
		struct proc_struct *prev = current, *next = proc;
		local_intr_save(intr_flag);
		{
			current = proc;
			load_esp0(proc->kstack + KSTACKSIZE);
			lcr3(proc->cr3);
			switch_to(&prev->context, &next->context);
		}
		local_intr_restore(intr_flag);
	}
}

void proc_init(void) {
	
	if ( (idleproc = alloc_proc()) == NULL ) {

	}


	/* 为什么对于该线程不用分配页表，tf, mm */
	idleproc->pid = 0;
	idleproc->state = PROC_RUNNABLE;
	idleproc->kstack = (uintptr_t)bootstack;
	idleproc->need_resched = 1;

	set_proc_name(idleproc, "idle");

	nr_process++;

	current = idleproc;
	/* create kernel thread - init_main */
	int pid = kernel_thread(init_main, NULL, 0);
	if (pid <= 0) {

	}

	initproc = find_proc(pid);
	set_proc_name(initproc, "init");
}

void cpu_idle(void) {
	while (1) {
		if (current->need_resched) {
			schedule();
		}
	}
}
