#include "stdio.h"
#include "user/syscall.h"

void uputs(const char *msg)
{
	syscall(8, (unsigned int)"msg", 0);
}

int hello_main(void)
{
	uputs("hello world\n");
	return 0;		
}
