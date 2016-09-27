#include <default_pmm.h>
#include <list.h>

static free_area_t free_area; // 确保无法被外部文件访问(OO)

#define free_list (free_area.free_list)
#define nr_free (free_area.nr_free)

static void default_init(void) {
	list_init(&free_list);
	nr_free = 0;
}

static void default_init_memmap(struct Page *base, size_t n) {
	struct Page *p = base;
	for (; p != base + n; p++) {
		set_page_ref(p, 0)
		p->flags = 0;
		list_add(&free_list, p);
	}
	nr_free += n;
}

/**
 * 返回 struct Page 指针, 存放指针自身是虚拟地址,而指针的指向是物理地址
 */
static struct Page *
default_alloc_pages(size_t n) {
	// 目前只实现分配一个物理页
	assert(n == 1);
	list_entry_t *le;
	if ((le = list_next(&free_list)) != &free_list) {
		nr_free--;
		list_del(le);
		return le2page(le, page_link);
	}
	return NULL;
}

static void default_free_pages(struct Page *base, size_t n) {
	base->flags = 0;
	nr_free++;
	set_page_ref(base, 0);
	list_add(&free_list, &(base->page_link));
}

const struct pmm_manager default_pmm_manager = {
    .name = "default_pmm_manager",
    .init = default_init,
    .init_memmap = default_init_memmap,
    .alloc_pages = default_alloc_pages,
    .free_pages = default_free_pages,
    .nr_free_pages = default_nr_free_pages,
    .check = default_check,
}