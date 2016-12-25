#include "default_pmm.h"

const struct pmm_manager *pmm_manager;

struct Page *pages;  // 页帧起始地址(虚拟地址)
size_t npages;       // 页帧的总数量
struct Page *boot_pgdir; // 页目录
static void init_pmm_manager(void) {
	pmm_manager = &default_pmm_manager;
	pmm_manager->init();
}


static void init_memmap(struct Page *page, size_t n) {
	pmm_manager->init_memmap(page, n);
}

/**
 * 页帧初始化
 */
static void init_page(void) {
	struct e820map *mmap = (struct e820map*) (0x1000 + KERNBASE);
	uint64_t maxpa = 0; // 最大页地址
	int i = 0;
	for (; i < mmap->nr_map; i++) {
		// 探测的 begin 与 end 均为物理地址
		uint32_t begin = mmap->map[i].addr;
		uint32_t end   = begin + mmap->map[i].size;
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
 * 获取二级页表项
 */
pte *get_pte(pde_t *boot_pgdir, uintptr_t la, bool create) {

}

static void boot_memmap_page(pde_t *pgdir, uintptr_t la, size_t size, uintptr_t pa, uint32_t prem) {

}

static void pmm_init(void) {
	init_pmm_manager();

	init_page();

	boot_pgdir = alloc_page(1);


}
