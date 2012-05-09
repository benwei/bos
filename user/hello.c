#include "stdio.h"
#include "user/syscall.h"

void uputs(const char *msg);

int hello_main(void)
{
	uputs("Hello world!");
	return 0;		
}
