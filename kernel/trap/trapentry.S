
.text
.globl __alltraps
__alltraps:
	
	pushl %ds
	pushl %es

	#Push EAX, ECX, EDX, EBX, original ESP, EBP, ESI, and EDI
	pushal


	movl $GD_KDATA, %eax
	movw %ax, %ds
	movw %ax, %es

	# 存放 trapframe 的地址, 相当于一个指针的作用
	pushl %esp

	# trap.c trap function
	call trap

	pop %esp

.globl __trapret
__trapret:
	

	iret

.globl forkrets
forkrets:
	
	jmp __trapret