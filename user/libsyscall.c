#include "user/syscall.h"
#include "stdio.h"

#define TRAP_SYSCALL 68
unsigned int syscall(int num, unsigned int a1, unsigned int a2)
{
	unsigned int ret = 0;
	 __asm __volatile(
		"int %1\n"
		:"=a" (ret)
		:"i" (TRAP_SYSCALL),
		 "a" (num),
		 "d" (a1),
		 "c" (a2)
		:"cc", "memory");
	printf("syscal with ret=%d\n", ret);
	return ret;
}

