#include "user/syscall.h"
#include "stdio.h"

/*
 * num: BSYS_PUTS, a1 is string for display, a2 is undefined.
 */
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
	printf("syscall with ret=%d\n", ret);
	return ret;
}

void uputs(const char *msg)
{
	syscall(BSYS_PUTS, (unsigned int)msg, 202);
}

