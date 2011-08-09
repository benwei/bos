#include "os_stdio.h"
#include "os_stdio2.h"

char get_floppy_info(void)
{
	unsigned char c;
	outb(0x70, 0x10);
	c=inb(0x71);
	return c;
}

void show_floppy_info(unsigned char c)
{
	unsigned char h = 0;
	unsigned char l = 0;
	h = c >> 4;
	l = c & 0x0f;
	if (h>0) {
		puts("\nfloppy a:");
		puts(get_fd_msg(h));
	}
	if (l > 0) {
		puts("\nfloppy b:");
		puts(get_fd_msg(l));
	}
	if (c>0) {
		puts("\n");
	}
}
