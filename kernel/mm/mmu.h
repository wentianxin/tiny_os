#ifndef _KERN_MM_MMU_H_
#define _KERN_MM_MMU_H_

#include <types.h>

/* bitfield http://stackoverflow.com/questions/3186008/in-c-what-does-a-colon-mean-inside-a-declaration */
struct segdesc {
	unsigned sd_lim_15_0 : 16;
	unsigned sd_base_15_0 : 16;
	unsigned sd_base_23_16 : 8;
	unsigned sd_access : 8;
	unsigned sd_lim_19_16 : 4;
	unsigned sd_flags : 4;
	unsigned sd_base_31_24 : 8;
};

struct pseudodesc{
	uint16_t  pd_lim;
	uintptr_t pd_base;
};

/* task state segment */
/* https://pdos.csail.mit.edu/6.828/2010/readings/i386/s07_01.htm */
struct taskstate {
	uint16_t ts_link;
	uint16_t ts_padding0;
	uint32_t ts_esp0;
	uint16_t ts_ss0;
	uint16_t ts_padding1;
	uint16_t ts_ss1;
	uint16_t ts_padding2;
	uint32_t ts_esp2;
	uint16_t ts_ss2;
	uint16_t ts_padding3;
	uint32_t ts_sr3;
	uint32_t ts_eip;
	uint32_t ts_eflags;
	uint32_t ts_eax;
	uint32_t ts_ecx;
	uint32_t ts_edx;
	uint32_t ts_ebx;
	uint32_t ts_esp;
	uint32_t ts_ebp;
	uint32_t ts_esi;
	uint32_t ts_edi;
	uint16_t ts_es;
	uint16_t ts_padding4;
	uint16_t ts_cs;
	uint16_t ts_padding5;
	uint16_t ts_ss;
	uint16_t ts_padding6;
	uint16_t ts_ds;
	uint16_t ts_padding7;
	uint16_t ts_fs;
	uint16_t ts_padding8;
	uint16_t ts_gs;
	uint16_t ts_padding9;
	uint16_t ts_ldt;
	uint16_t ts_padding10;
	uint16_t ts_t;
	uint16_t ts_iomb;
};

// +--------10------+-------10-------+---------12----------+
// | Page Directory |   Page Table   | Offset within Page  |
// |      Index     |     Index      |                     |
// +----------------+----------------+---------------------+
//  \--- PDX(la) --/ \--- PTX(la) --/ \---- PGOFF(la) ----/
//  \----------- PPN(la) -----------/
//

#define PPN(la) ((uintptr_t)(la) >> PTXSHIFT) // 右移12位, 即除以4096(一页)

#define PGOFF(la) ((uintptr_t)(la) & 0xFFF)


#define PTE_ADDR(pte)  ((uintptr_t)(pte) & ~0xFFF)
#define PDE_ADDR(pde)  PTE_ADDR(pde)



#define PTXSHIFT   22

/* 页表/页目录项标志 */
#define PTE_P        0x001                   // Present(resident in memory not swapped out)
#define PTE_W        0x002
#define PTE_U        0x004

#endif
