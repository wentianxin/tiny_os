/**
 * 实现了最小的C函数库，除了一些与系统调用无关的函数，其他函数是对访问系统调用的包装。
 */

#include <syscall.h>

void exit(int error_code) {
	sys_exit(error_code);

	while(1);
}

int fork(void) {
	return sys_fork();
}

void yield(void) {

}

int getpid(void) {
	return sys_getpid();
}

void print_pgdir(void) {

}