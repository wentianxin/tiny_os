

/* kva - pa KERNBASE~KMEMSIZE - 0~KERNBASE */
#define PADDR(kva) ({                           \
	uintptr_t __m_kva = (kva);                  \
	if (__m_kva < KERNBASE) {                   \
		panic("PADDR called with invaild kva"); \
	}                                           \
	__m_kva - KERNBASE;                         \
})

/* pa - kva  0~KMEMSIZE - KERNBASE~KMEMSIZE*/
#define KADDR(pa) ({                            \
	uintptr_t __m_pa = (pa);                    \
	(void *) (__m_pa + KERNBASE);               \
})


extern struct Page *pages; // 管理物理页表(页帧)的数组(存放的是虚拟地址)
extern size_t npage;       // 物理页帧的总数

struct pmm_manager{
	const char *name;
	void (*init)(void);   // 对象初始化
	struct Page *(*alloc_pages)(size_t n); // 页帧分配
	void (*memmap_page)(struct page *base, size_t n); // 物理地址映射到页帧
	void (*free_pages)(struct Page *base, size_t n);  // 页帧释放
	size_t (*nr_free_pages)(void);  // 空闲页帧数
	void (*check)(void);
};


static inline void
set_page_ref(struct Page *page, int val) {
	atomic_set(&(page->ref), val);
}

/* 根据物理地址返回对应的管理页面 struct Page */
static inline struct Page *
pa2page(uintptr_t pa) {
	return &pages[PPN(pa)];
}
