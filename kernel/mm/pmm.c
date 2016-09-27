#include <default_pmm.h>

static struct taskstate ts = {0};

uint8_t stack0[1024];


const struct pmm_manager *pmm_manager;

struct Page *pages;
int npage;


pde_t *boot_pgdir = NULL;


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

static void init_pmm_manager(void) {
	pmm_manager = &default_pmm_manager;
	pmm_manager->init();
}

static void page_init(void) {

	struct e820map *e820map = (struct e820map *)(0x8000 + KERNBASE);

	uint64_t maxpa = 0;
	for (int i = 0; i < e820map->nr_map; i++) {
        if (memmap->map[i].type == E820_ARM) {
            if (maxpa < end && begin < KMEMSIZE) {
                maxpa = end;
            }
        }
	}

	npage = maxpa / PGSIZE; // 一共的需要管理物理页数
	pages = (struct Page *)ROUNDUP(end, PGSIZE);

	for (int i = 0; i < npage; i++) {
		((struct Page *)(pages + i))->flag = 0;
	}

	uintptr_t freemem = pages + (sizeof(struct Page)) * npage;

	for (int i = 0; i < memmap->nr_map; i++) {
		uint64_t begin = memmap->map[i].addr, end = begin + memmap->map[i].size;
		if (memmap->map[i].type == E820ARM) {
			if (begin < freemem) {
				begin = freemem;
			}
			if (end > KMEMSIZE) {
				end = KMEMSIZE;
			}
			if (begin < end) {
				pmm_manager->init_memmap(pa2page(begin), (end - begin) / PGSIZE);
			}
		}
	}
}

/**
 * 在线性地址和物理地址之间建立分页映射
 * pgdir: 页目录项
 * la   : 线性地址
 * size : 要映射的内存大小
 * pa   : 物理地址
 * perm : 内存的对应权限
 */
static void
boot_map_segment(pde_t *pgdir, uintptr_t la, size_t size, 
	uintptr_t pa, uint32_t perm) {
	size_t n = ROUNDUP(size, PGSIZE) / PGSIZE;
    la = ROUNDDOWN(la, PGSIZE);
    pa = ROUNDDOWN(pa, PGSIZE);

	for (; n > 0; n--, la += PGSIZE, pa += PGSIZE) {
		pte_t *ptep = get_pte(pgdir, la);
		*ptep = pa | PET_P | perm;
	}
}

static pte_t *get_pte(pde_t *pgdir, uintptr_t la) {
	pdt_t *pdep = &pgdir[PDX(la)]; // pdep 自身就是虚拟地址;
	if (!(*pdep & PET_P)) {
		// 不存在
		struct Page *page;
		if ((page = pmm_manager->alloc_pages(1)) == NULL) {
			return NULL;
		}
		set_page_ref(page, 1);
		uintptr_t pa = page2pa(page);
		memset(KADDR(pa), 0, PGSIZE); // 对物理地址进行清空
		*pdep = pa | PTE_U | PTE_W | PTE_P;
	}

	return &((pte_t *)KADDR(PDE_ADDR(*pdep)))[PTX(la)];
}

void pmm_init()
{
	init_pmm_manager();

	page_init(); // 物理内存管理初始化


	boot_pgdir = pmm_manager->alloc_pages(1);
	memset(boot_pgdir, 0, PGSIZE);
	boot_cr3 = PADDR(boot_pgdir);


	boot_map_segment(boot_pgdir, KERNBASE, KMEMSIZE, 0, PTE_W);

	boot_pgdir[0] = boot_pgdir[PDX(KERNBASE)];

	enable_paging();

	gdt_init();

	boot_pgdir[0] = 0;
}
