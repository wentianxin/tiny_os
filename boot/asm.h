#ifndef _ASM_H_
#define _ASM_H_

/*
 *  段选择子 与 段描述符 
 *
 *  参考资料 ： https://pdos.csail.mit.edu/6.828/2007/readings/i386/s05_01.htm
 *             http://wiki.osdev.org/Global_Descriptor_Table 
 *             https://chyyuu.gitbooks.io/ucorebook/content/zh/chapter-2/privilege_level.html
 *  
 *
 *  Segment Selectors （段选择子）
 *  ------------------------
 *  |15            3|2 |1 0|
 *  ------------------------
 *  |     Index     |TI|RPL|
 *  -----------------------
 *
 *  RRL   : Request's privliege level
 *
 *
 *
 *  Global Description Table - 8bytes (全局描述符表)
 *  -------------------------------------------------------------------
 *  |31                            16 | 15                           0|
 *  -------------------------------------------------------------------
 *  |          Base 0:15              |          Limit 0:15           |
 *  -------------------------------------------------------------------
 *  |63                            48 | 47          40 | 39         32|
 *  -------------------------------------------------------------------
 *  |Base 24:31 | Flags | Limit 16:19 |   Access Byte  |  Base 16:23  |
 *  -------------------------------------------------------------------
 *
 * 
 *  Accses Byte 
 *  ------------------------
 *  |7                    0|
 *  ------------------------
 *  |Pr|Privl|1|Ex|DC|RW|Ac|
 *  ------------------------
 *
 *  Pr   : Present bit;   1bist.This must be 1 for all vaild selectors.
 *  Privl: Privliege  ;   2bits.From 0(ring level)to 3(user application)
 *  Ex   : Executable bit;1bits.1-code can be executed(code selector); 0-code can't be executed(data selector)
 *  DC   : Direction bit/Conforming bit.
 *  RW   : Readable bit/Writeable bit.
 *  Ac   : Accessed bit. The CPU sets this to 1 when the segment is accessed
 *
 *  Flags
 *  -----------
 *  |7       4|
 *  -----------
 *  |Gr|Sz|0|0|
 *  -----------
 *
 *  Gr   : Granularity bit(). 0 - 1 Blocks(byte granularity); 1- the limit is in 4KiB blocks(page granularity)
 *  Sz   : Size bit.          0 - the selector defines 16 bit protected mode. 1 - 32bit protected mode
 *
 *  
 *  
 *  请求权限判断过程:
 *
 *  CS 段寄存器（即段选择子）的最低两位代表 ： 当前执行代码本身所拥有的权限（即CPL）。
 *  倘若去获取其他数据或是其他段的内容，首先经过最终访问段的段选择子（最低两位的特权级的含义是：我们想要请求的特权级，即PRL），
 *  根据该段选择子找到GDT中的段描述符，对比段描述符中的 Privl位（即DPL，代表最终访问段的真正特权级）
 *  判断 max(RPL, CPL) <= DPL 是否成立
 */ 


/* Segment types: Ex Dc RW Ac */
#define STA_X 8  // 如果是1，则在段中的代码是可执行的；如果是0，则是数据选择子（ds,ss,es,fs,gs）
#define STA_C 4   
#define STA_E 4
#define STA_R 2
#define STA_W 2
#define STA_A 1

#define SEG_NULLASM
	.word 0, 0;
	.byte 0, 0, 0, 0;

#define SEG_ASM(type, base, lim)
	.word (lim) & 0xffff, (base) & 0xffff;
	.byte ((base) >> 16) & 0xff, (0x90 | (type)) & 0xff,
		  (0xC0 | (lim >> 16)) & 0xff, (base >> 24) & 0xff;


#endif