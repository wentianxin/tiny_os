#ifndef _LIBS_TYPE_H
#define _LIBS_TYPE_H

// 因为一些头文件可能已经定义了 NULL
#ifndef NULL
#define NULL ((void *) 0)

typedef int bool;

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef int32_t intptr_t;
typedef uint32_t uintptr_t;

/* 内存空间大小*/
typedef uintptr_t size_t;

/* 文件偏移量 */
typedef intptr_t off_t;

/* 获取结构体 member 成员的偏移量 */
#define offsetof(type, member) 
	((size_t) (&((type *)0)->member ))	

/* 获取结构体: 根据 ptr 指针 */
#define to_struct(ptr, type, memeber) 
	((type *)((char *)(ptr) - offsetof(type, memeber)))


#endif