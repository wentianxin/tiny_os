#include "default_pmm.h"

const struct pmm_manager *pmm_manager;

struct Page *pages;  // 页帧起始地址(pages指向的是虚拟地址)
size_t npages;       // 页帧的总数量
pde_t *boot_pgdir;   // 页目录(虚拟地址)
uintptr_t boot_cr3;   // 页目录地址, 物理地址



/** ------------------------------------硬件相关 API ------------------------------*/


/**
 * 分页开启(可屏蔽掉的API)
 */
static void enable_paging(void) {
    lcr3(boot_cr3);  // lcr3指令: 页目录表的起始地址存入CR3寄存器中；
    // turn on paging  lcr0指令把cr0中的CR0_PG标志位设置上
    uint32_t cr0 = rcr0();
    cr0 |= CR0_PE | CR0_PG | CR0_AM | CR0_WP | CR0_NE | CR0_TS | CR0_EM | CR0_MP;
    cr0 &= ~(CR0_TS | CR0_EM);
    lcr0(cr0);
}

/**
 * 可屏蔽掉的API
 */
static void gdt_init(void) {
    // set boot kernel stack and default SS0
    load_esp0((uintptr_t)bootstacktop);
    ts.ts_ss0 = KERNEL_DS;

    // initialize the TSS filed of the gdt
    gdt[SEG_TSS] = SEGTSS(STS_T32A, (uintptr_t)&ts, sizeof(ts), DPL_KERNEL);

    // reload all segment registers
    lgdt(&gdt_pd);

    // load the TSS
    ltr(GD_TSS);
}


// --------------------------------- pmm_manager 初始化 -------------------------

/**
 * pmm_manager 对象初始化
 */
static void init_pmm_manager(void) {
	pmm_manager = &default_pmm_manager;
	pmm_manager->init();
}

// ---------------------------------  pmm_manager 接口再封装 ---------------------

// init_memmap接口
static void init_memmap(struct Page *page, size_t n) {
	pmm_manager->init_memmap(page, n);
}

/**
 * 分配页帧，并返回页帧指向的物理地址的虚拟地址
 */
static void *boot_alloc_page() {
	struct Page *page;
	if ((page = pmm->alloc_page(1)) == NULL) {
		panic("boot_alloc_page failed.\n");
	}
	return page2kva(page);
}





/**
 * 页帧初始化
 */
static void init_page(void) {
	struct e820map *mmap = (struct e820map*) (0x1000 + KERNBASE); // 探测的物理地址
	uint64_t maxpa = 0;  // 最大页地址
	int i = 0;
	for (; i < mmap->nr_map; i++) {
		// 探测的 begin 与 end 均为物理地址
		uint32_t begin = mmap->map[i].addr;
		uint32_t end   = begin + mmap->map[i].size;
        cprintf("  memory: %08llx, [%08llx, %08llx], type = %d.\n",
                memmap->map[i].size, begin, end - 1, memmap->map[i].type);
		if (mmap->map[i].type == E820_ARM) {
			// 映射关系 0~KMEMSIZE —— KERNBASE~KERNBASE+KMEMSIZE
			if (begin < KMEMSIZE && maxpa < end) {
				maxpa = end;
			}
		}
	}

	extern char []end;      // 内核代码所占的内存末尾地址(3M~4M大), 本身就是虚拟地址

	npage = maxpa / PGSIZE; // 一共多少页
	pages = (struct Page *)ROUNDUP((void *)end, PGSIZE); // 页帧的起始地址(虚拟地址)

	// 标记存放页帧的空间为保留页,不可分配与释放
	for (i = 0; i < npage; i++) {
		SetPageReserved(pages+i);
	}

	// pages(虚拟地址), 得到物理地址
	uintptr_t freemem = PADDR((uintptr_t)pages + npage * sizeof(struct Page)); //非保留页(可分配使用页帧)的起始地址
	for (i = 0; i < mmap->nr_map; i++) {
		uint64_t begin = mmap->map[i].addr;
		uint64_t end   = mmap->map[i].size + begin;
		if (mmap->map[i].type == E820_ARM) {
			if (begin < freemem) {
				begin = freemen;
			}
			if (end > KMEMSIZE) {
				end = KMEMSIZE;
			}
			if (begin < end) {
                begin = ROUNDUP(begin, PGSIZE);
                end   = ROUNDDOWN(end, PGSIZE);
                if (begin < end) {
                    init_memmap(pa2page(begin), (end - begin) / PGSIZE);
                }
			}
		}
	}
}


/**
 * 获取二级页表项(&pgdir[PDX(la]); 倘若二级页表不存在,则创建二级页表,并建立页目录(一级页表)与二级页表之间的映射关系
 * 并返回线性地址对应的二级页表项
 * pgdir  页目录
 * la     映射的线性地址
 * create 1-创建二级页表; 0-不创建二级页表
 */
pte_t *get_pte(pde_t *pgdir, uintptr_t la, bool create) {
	pde_t *pdep =  &pgdir[PDX(la)];// 页目录项
	if (!(*pdep & PTE_P)) {
		// 页目录项对应的二级页表不存在, 分配页帧并初始化该页帧，初始化该页目录项
		struct Page *page = NULL;
		if (!create || (page = alloc_page(1)) == NULL) {
			return NULL;
		}
		set_page_ref(page, 1);
		uintptr_t pa = page2pa(page);   // 返回该页帧对应真实的物理地址
		memset(KADDR(pa), 0, PGSIZE);   // 清空该物理页块(memset的参数是虚拟地址:pa + KERNBASE)
		// TODO 为什么还有 user的权限
		*pdep = pa | PTE_P | PTE_W | PTE_U;  // 二级页表权限设置 Present, Write, User
	}
	return &((pte_t *)KADDR(PDE_ADDR(pdep)))[PTX(la)];
}


/**
 * 根据la线性地址建立页表(二级页表)
 * pgdir 页目录(pde_t *pgdir 为页目录首地址, pgdir[n] 为具体页目录项, &pgdir[n]为页目录项的地址)
 * la    分页机制中要映射的线性起始地址
 * size  分页机制中要映射的地址大小
 * pa    分页机制中要映射的物理地址起始地址
 * perm  页表项(page table entry)对应的具体权限
 */
static void boot_memmap_page(pde_t *pgdir, uintptr_t la, size_t size, uintptr_t pa, uint32_t prem) {
	assert(PGOFF(la) == PGOFF(pa));
	la = ROUNDDOWN(la, PGSIZE);
	pa = ROUNDDOWN(pa, PGSIZE)
	size_t n = ROWNDUP(size + PGOFF(la), PGSIZE) / PGSIZE; // 当size恰好为PGSIZE时, 要根据la所处位置, 来分配页帧
	for (; n > 0; n--, la += PGSIZE, pa += PGSIZE) {
		// 根据线性地址获取二级页表项, 倘若二级页表不存在,则建立
		pte *pte = get_pte(pgdir, la, 1);
		assert(pte == NULL);
		*pte = pa | PTE_P | perm;  // 二级页表中映射的物理地址与权限(Present, Write);
	}
}



void pmm_init(void) {
	/* 此时 va = la + KMEMSIZE = pa + KMEMSIZE(基于段机制) */
	init_pmm_manager();

	// 页帧的建立与初始化
	init_page();

	// 检测alloc/free函数的正确性
	check_alloc_page();

	// 页目录的建立
	boot_pgdir = boot_alloc_page();
	memset(boot_pgdir, 0, PGSIZE);
	boot_cr3 = PADDR(boot_pgdir); // 虚拟地址转化为物理地址

	// 页目录与二级页表的初始化与建立 KERNBASE ~ KERNBASE+ KEMESIZE ---- 0 ~ 0 + KMEMSIZE
	boot_memmap_page(boot_pgdir, KERNBASE, KMEMSIZE, 0, PTE_W);

	// la: 0~4M ----- KERNBASE~KERNBASE + 4M
	boot_pgdir[0] = boot_pgdir[PDX(KERNBASE)];

	// 开启分页
	enable_paging();
	/* 分页机制开启后, 段的寻址未还原: 映射关系如下
	 * 现在的la - pa 的映射关系:
	 *     		KERNBASE ~ KERNBASE + KMEMSIZE ----- 0 ~ KMEMSIZE
	 *     	va - la 的映射关系
	 *     		KERNBASE ~ KERNBASE + KMEMSIZE ----- 0 ~ KMEMSIZE
	 * 所以需要再建立一个 la - pa的映射关系
	 * 			0 ~ KMEMSIZE                   ----- 0 ~ KMEMSIZE
	 * 这样便能够让 va - pa正确映射:
	 * 			KERNBASE ~ KERNBASE + KMEMSIZE ----- 0 ~ KMEMSIZE
	 * 所以: boot_pgdir[0] = boot_pgdir[PDX(KERNBASE)];只映射1个页是因为我们的内核大小不会超过4M
	 */


	// GDT全局表初始化, 重新调整分段机制，使得 va = la
	gdt_init();

	// 关闭 la 0~4M ----- KERNBASE~KERNBASE + 4M的映射
	boot_pgdir[0] = 0;
}
