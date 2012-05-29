#include <stdio.h>

void spinlock(void *lock)
{
	*(unsigned int *) lock = 1;

	while (*(unsigned int *)lock)
		sleep(1);

	return ;
}

void spinunlock(void *lock)
{
	*(unsigned int *)lock = 0;
	return;
}
