
#include <atomic.h>

typedef uintptr_t pte_t;
typedef uintptr_t pde_t;


#define E820MAX  20
struct e820map {
	int nr_map;
	struct {
		uint64_t addr;
		uint64_t size;
		uint32_t type;
	} __attribute__((packed) map[E820MAX]);
};


struct Page {
	atmoic_t ref;   // 原子变量，引用的数量
	uint32_t flags; // 是否被占用
	list_entry_t page_link; /// 链表连接点
};

/* Page 成员flags(32bit) 位的含义 */
#define PG_RESERVED  0   // flags 0位 : 标志该页帧是否保留 1:保留(即该页帧不是空闲页,无法分配与释放)； 0:未保留
#define PG_PROPERTY  1   // flags 1位 : 标志该页帧是否分配 1:分配； 0:未分配
/* 对PG_RESERVED位的操作 */
#define SetPageReserved(page)   set_bit(PG_RESERVED, &(page->flags))
#define ClearPageREserved(page) clear_bit(PG_RESERVED, &(page->flags))
#define PageReserved(page)      test_bit(PG_RESERVED, &(page->flags))




typedef struct {
	list_entry_t free_list;
	unsigned int nr_free; // 空闲量
} free_area_t;



/* 页帧链表项转化为对应的页帧结构体(根据结构体成员的偏移量计算结构体首地址) */
#define le2page (le)
	(to_struct(le, struct Page, page_link))
