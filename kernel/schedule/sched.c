

/**
 * 进程调度
 */
void schedule(void) {
	struct proc_struct *next = NULL;

	list_entry_t *le, *last;
	last = ( current == idleproc ? : &proc_list, &current->list_link );
	le = last;

	/**
	 * 对于双向循环链表的遍历, 遍历过程中要跳过链表头 proc_list
	 * 最后跳到原位时结束
	 */
	do {
		if ( (le = list_next(le)) != &proc_list) {
			next = le2proc(le, list_link);
			if (next->state == PROC_RUNNABLE) {
				break;
			}
		}
	} while (le != last);

	if (next == NULL || next->state != PROC_RUNNABLE) {
		// not found
		next = idleproc;
	}
	if (next != current) {
		proc_run(next);
	}
}


