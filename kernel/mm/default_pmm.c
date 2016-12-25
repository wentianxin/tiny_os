#include "default_pmm.h"

// 空闲链表
static free_area_t free_area;

#define free_list (free_area.free_list)
#define nr_free(free.area.nr_free)

/**
 * 内存管理初始化
 */
static void default_init(void) {
	list_init(&free_list);
	nr_free = 0;
}

/**
 * 物理地址(base << 12) 映射为页帧
 * base 指针指向页帧的存储位置
 * n    添加(映射)的页帧数
 */
static void default_memmap_init(struct page *base, size_t n) {
	struct page *page = base;
	for (; page != base + n; page++) {
		page->ref = 0;
		page->flags = 0;
		list_add(&free_list, &base->page_link);
	}
	nr_free += n;
}

/**
 * 分配页帧(暂时只实现了只能分配一个页帧)
 * return 返回一个空闲页帧; 倘若页帧数为0, 返回NULL
 */
static struct page* default_alloc_page(size_t n) {
	asset(n == 1); // 检测
	struct page *page = NULL;
	list_entry_t le;
	if ((le = list.next(&free_list)) != NULL) {
		nr_free--;
		list_del(le);
		return le2page(le);
	}
	return NULL;
}

/**
 * 释放页帧(默认只能释放一页页帧)
 */
static void default_free_page(struct page *base, size_t n) {
	// 检测n是否为1 && 检测base 页帧是否是保留页
	assert(n == 1 && !PageReserved(base))
	base->ref = 0;  // TODO 应该改为原则操作 set_page_ref(base, 0)
	base->flags = 0;
	nr_free++;
	list_add(&free_list, &(base->page_link));
}

/**
 * 当前空闲的物理页数
 */
static void default_nr_free_page(void) {
	return nr_free;
}

static void basic_check(void) {

}

static void default_check(void) {

}

/**
 * 物理内存管理对象(类似面向对象思想)
 */
const struct pmm_manager default_pmm_manager = {
	.name = "default_manager";
	.init = default_init,
	.alloc_page = default_alloc_page,
	.memmap_page = default_memmap_page,
	.free_page = default_free_page,
	.nr_free_page = default_nr_free_page,
	.check = default_check,
};

