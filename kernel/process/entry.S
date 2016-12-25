.text
.global kernel_thread_entry
kernel_thread_entry

	pushl %edx
	call *%ebx

	pushl %eax
	call do_exit
