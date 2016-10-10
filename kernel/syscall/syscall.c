/**
 * 系统函数
 */

void syscall(void) {
	struct trapframe *tf = current->tf;

	uint_32 num = tf->tf_regs.reg_eax;
}