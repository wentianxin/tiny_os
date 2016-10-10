#include <trap.h>

static void trap_dispatch(struct trapframe *tf) {

	switch (tf->tf_trapno) {
		case T_DEBUG:
		case T_BRKPT:
			debug_monitor(tf);
			break;
		case T_PGFLT:
			break;
		case T_SYSCALL:
			syscall();
			break;
		default:

	}
}

void trap(struct trapframe *tf) {

	if (current == NULL) {
		trap_dispatch(tf);
	} else {
		struct trapframe *otf = current->tf;

		current->tf = tf;

		trap_dispatch(tf);

		current->tf = otf;


	}
}