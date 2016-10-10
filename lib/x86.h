/*
 * 本头文件涉及到内敛汇编知识
 * asm [volatile] (
 *     : Output Operands
 * )
 */
#ifndef _X86_H_
#define _X86_H_

/*
 * 读取 port 端口数据
 */
static inline uint8_t inb(uint16_t port) {

	uint8_t data;

	asm volatile (
		"inb %1, %0"
		: "=a" (data)
		: "d" (port)
		: "memory");

	return data;
}

// repne insl (%dx), %es:(%edi)
static inline void insl(uint32_t port, void *addr, int cnt) {
	asm volatile (
		"cld;"
		"repne; insl;"
		: "=D" (addr), "=c" (cnt)
		: "d" (port), "0" (addr), "1" (cnt)
		: "memory", "cc");
}

static inline void outb(uint32_t port, uint8_t data) {
	asm volatile (

		"outb %1, %0"
		:
		: "d" (port), "a" (data)
		: "memory"
		);
}

static inline void lcr3(uintptr_t cr3) {
	asm volatile (
		"movw %0, %%cr3"::"r"(cr3):"memory"
	)；
}
#endif