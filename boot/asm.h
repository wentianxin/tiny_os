#ifndef _ASM_H_
#define _ASM_H_

// Segment types : Ex Dc RW Ac
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