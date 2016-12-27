#include <elf.h>
#include <x86.h>
#include <types.h>

#define SECTSIZE 512

/*
 *
 */
void waitdisk(void) {
	while (inb(0x1F7) & 0xC0 != 0x40)
		; 
}

/*
 * 读取第 secno 个扇区到 dst 中
 * det  : 目的地址
 * secno: 扇区号
 */
void readsect(void *dst, uint32_t secno){

    // wait for disk to be ready
    waitdisk();

    outb(0x1F2, 1);                         // count = 1
    outb(0x1F3, secno & 0xFF);
    outb(0x1F4, (secno >> 8) & 0xFF);
    outb(0x1F5, (secno >> 16) & 0xFF);
    outb(0x1F6, ((secno >> 24) & 0xF) | 0xE0);
    outb(0x1F7, 0x20);                      // cmd 0x20 - read sectors

    // wait for disk to be ready
    waitdisk();

    // read a sector
    insl(0x1F0, dst, SECTSIZE / 4);
}

/*
 * 从硬盘扇区中获取 conut 个字节加载到虚拟地址为 va 的内存中
 */
void readseg(uintptr_t va, uint32_t count, uint32_t offset) {

	// 如何得到 扇区号， 从1开始
	/*
	uint32_t secsno = (offset / SECTSIZE) + 1;

	va -= (offset % SECTSIZE);

	uint32_t seceno = secsno + count / SECTSIZE;

	if (count % SECTSIZE != 0) {
		seceno++;
	}

	for (;secsno < seceno; secsno++, va+=SECTSIZE) {
		readsect(va, secsno);
	}
	*/

	uint32_t  secsno = (offset / SECTSIZE) + 1;

	uintptr_t end_va = va + count;

	va -= (offset % SECTSIZE);

	for (; va < end_va; va += SECTSIZE, secsno) {
		readsect((void *)va, secno);
	}

}

/*
 * 将kernel（位于第二个扇区）加载到内存中
 */
void bootmain() {

	// 读取ELF文件头:第一页(4K)
	readseg((uintptr_t)ELFHDR, SECTSIZE * 8, 0);

	// 检验ELF是否有效
	if (ELFHDR->e_magic != ELF_MAGIC) {
		goto bad;
	}
	
	struct prohdr *ph, *eph;

	ph  = (struct prohdr *) ((uintptr_t)ELFHDR + ELFHDR->e_phoff);
	eph = ph + ELFHDR->e_phnum;

	for (; ph < eph; ph++) {
		readseg(ph->p_va & 0xFFFFFF, ph->p_memsz, ph->p_offset);
	}

	((void (*)(void)) (ELFHDR->e_entry & 0xFFFFFF))();

bad:
	outw(0x8A00, 0x8A00);
	outw(0x8A00, 0x8E00);

	while(1) ;
}

