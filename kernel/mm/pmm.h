
#define KADDR(pa) ({
	uintptr_t __m_pa = (pa);
	(void *) (__m_pa + KERNBASE);
})


extern struct Page *pages; // 管理物理页表的数组
extern size_t npage;       // 物理页面的总数

struct pmm_manager{
	const char *name;
	void (*init)(void);

	struct Page *(*alloc_pages)(size_t n);
	void (*free_pages)(struct Page *base, size_t n);
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
