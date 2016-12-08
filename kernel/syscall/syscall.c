/**
 * 系统函数
 */

void syscall(void) {
	struct trapframe *tf = current->tf;

	int num = tf->tf_regs.reg_eax;

	uint32_t arg[5];

	if (num >= 0 && num < NUM_SYSCALLS) {
		
	}
}