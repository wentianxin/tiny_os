/**
 * 系统中断号描述 
 */

#ifndef __LIBS_UNISTD_H__
#define __LIBS_UNISTD_H__

#define T_SYSCALL   0x80

/* 系统调用号 */
#define SYS_exit    1
#define SYS_fork    2
#define SYS_wait    3
#define SYS_exec    4
#define SYS_clone   5

#define SYS_yield   10
#define SYS_sleep   11
#define SYS_kill    12


#define SYS_gettime 17
#define SYS_getpid  18
#define SYS_brk     19
#define SYS_mmap    20
#define SYS_mumap   21
#define SYS_shmem   22


#define SYS_putc    30
#define SYS_pgdir   31



#endif