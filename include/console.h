#ifndef CONSOLE_H
#define CONSOLE_H

typedef struct console_st {
	unsigned int vmem_addr;
	unsigned int vmem_current;
	unsigned int vmem_max;
	int x; /* cursor x postion */
	int y; /* cursor y postion */
} console;

extern console *cons_init(console *c); 

extern void scrollup(unsigned int linenum);

#endif /* CONSOLE_H */
