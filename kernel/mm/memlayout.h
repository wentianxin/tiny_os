
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
	atmoic_t ref;   // 原子变量
	uint32_t flags;
	list_entry_t page_link;
};

typedef struct {
	list_entry_t free_list;
	unsigned int nr_free; // 空闲量
} free_area_t;

#define le2page (le, member)
	(to_struct(le, struct Page, member))