#include "stdio.h"
#include "user/syscall.h"

void uputs(const char *msg);

int hello_main(void)
{
	uputs("hello world\n");
	return 0;		
}
