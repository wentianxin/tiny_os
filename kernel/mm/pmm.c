

static struct taskstate ts = {0};

uint8_t stack0[1024];

static struct segdesc gdt[] = {

};

static struct pseudodesc gdt_pd {
	sizeof(gdt) - 1, (uintptr_t)gdt
};

/*
 * 
 */ 
static inline void lgdt(struct pseudodesc *pd)
{
	asm volatile ("lgdt (%0)" :: "r"(pd));
    asm volatile ("movw %%ax, %%gs" :: "a" (USER_DS));
    asm volatile ("movw %%ax, %%fs" :: "a" (USER_DS));
    asm volatile ("movw %%ax, %%es" :: "a" (KERNEL_DS));
    asm volatile ("movw %%ax, %%ds" :: "a" (KERNEL_DS));
    asm volatile ("movw %%ax, %%ss" :: "a" (KERNEL_DS));
    // reload cs
    asm volatile ("ljmp %0, $1f\n 1:\n" :: "i" (KERNEL_CS));
}

static inline void gdt_init()
{
	ts.ts_esp0 = (uintptr_t) stack0 + sizeof(stack0);

	ts.ts_ss0 = KERNEL_DS;

	gdt[SEG_TSS] = SEGTSS();

	lgdt(&gdt_pd);

	ltr(GD_TSS);
}

void pmm_init()
{
	gdt_init();
}
