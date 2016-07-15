
.global start

start:
.code16
	
	cli # 禁止中断
	cld # 设置

	# 设置数据段的相关寄存器（ds 数据段， ss 堆栈段， es 附加段）
	xorw %ax, %ax
	movw %ax, %ds
	movw %ax, %ss
	movw %ax, %es

	# 第二步：开启A20 地址线
	# 8042芯片有两个I/O端口 0x60、0x64。
	# 开启A20地址线过程 ： 发送 0xdf 到 0x60，发送 0xd1 到 0x64
	# 然而再发送之前需要等待键盘输入的缓冲区为空。
.seta20.1:

	# 通过判断8042的状态寄存器的第2bit，0-缓冲区为空；1-缓冲区不为空
	# 状态寄存器的值可以通过读取8042芯片的0x64端口得到
	inb $0x64, %al
	testb $0x2, %al
	jnz seta20.1

	movb $0xd1, %al
	outb %a1, $0x64


.seta20.2
	
	inb   $0x64, %al
	testb $0x2,  %al
	jnz seta20.1

	movb $0xdf, %al # al寄存器是一个8bit的寄存器 （低8位），与 ah寄存器（高8位）组成 ax寄存器 （16位）
	outb %al, $0x64

	# 第三步：从实模式转入到保护模式
	
	# 载入临时全局描述符表 GDT
	lgdt gdetsec

	# 开启保护模式
	movl %cr0, %eax
	orl $0x1, %eax
	movl %eax, %cr0

	# 
	ljmp $PROT_MODE_CSEG, $protcseg

.code32
.protcseg

	# 重新设定数据段寄存器的值
	movw $PROT_MODE_DSEG, %ax
	movw %ax, %ds
	movw %ax, %ss
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

	movl $0x0, %ebp
	movl $start, %esp
	call bootmain

.data

gdt:
	SEG_NULLASM
	# type, base, limit(4G)
	SEG_ASM(STA_X, 0x0, 0xffffffff) # 代码段；起始地址: 0x8 ；类型：可读 可执行；限制大小：4G
	SEG_ASM(, 0x0, 0xffffffff) # 数据段；起始地址：0x10；类型：可写；限制大小：4G

gdtsec:
	.word 0x17 # gdt内存大小0x18 - 1
	.long gdt  # gdt起始位置


	
