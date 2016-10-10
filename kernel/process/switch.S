.text
.globl switch_to
switch_to:
	
	# 存储 prev 现场
	movl 4(%esp), %eax

	movl %ebx, 0(%eax)
	movl %ecx, 4(%eax)
	movl %edx, 8(%eax)
	movl %esi, 12(%eax)
	movl %edi, 16(%eax)
	movl %esp, 20(%eax)
	movl %ebp, 24(%eax)
	popl 28(%eax)

	# 恢复 next 现场
	movl 4(%esp), %eax

	pushl 28(%eax)
	movl 24(%eax), %ebp
	movl 20(%eax), %esp
	movl 16(%eax), %edi
	movl 12(%eax), %esi
	movl 8(%eax),  %edx
	movl 4(%eax),  %ecx
	movl 0(%eax),  %ebx

	ret # 根据 cs + eip  跳转 forkret