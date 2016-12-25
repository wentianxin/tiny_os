int kern_init()
{
	pmm_init(); // 物理内存初始化

	pic_init(); // 中断控制初始化
	idt_init(); // 中断描述符表初始化

	vmm_init();
	proc_init(); //

	ide_init();
	swap_init();

	clock_init(); // 时钟中断初始化
	intr_enable();//

	cpu_idle();
}
