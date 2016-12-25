.text
.globl switch_to
switch_to:
	
	# 存储 prev 现场

	#  获取函数参数 prev
	movl 4(%esp), %eax

    popl 0(%eax)                # save eip !popl
    movl %esp, 4(%eax)
    movl %ebx, 8(%eax)
    movl %ecx, 12(%eax)
    movl %edx, 16(%eax)
    movl %esi, 20(%eax)
    movl %edi, 24(%eax)
    movl %ebp, 28(%eax)

	# 恢复 next 现场
	#  获取函数参数 next
	movl 4(%esp), %eax

    movl 28(%eax), %ebp
    movl 24(%eax), %edi
    movl 20(%eax), %esi
    movl 16(%eax), %edx
    movl 12(%eax), %ecx
    movl 8(%eax), %ebx
    movl 4(%eax), %esp

	pushl 0(%eax)               # push eip
	ret
	# 根据 cs + eip  跳转 forkret
