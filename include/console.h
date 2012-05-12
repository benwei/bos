#ifndef CONSOLE_H
#define CONSOLE_H

typedef struct console_st {
	unsigned int vmem_addr;
	unsigned int vmem_current;
	unsigned int vmem_max;
	int x; /* cursor x postion */
	int y; /* cursor y postion */
} console;

extern void
cons_init(void); 

extern void
cons_setpos(unsigned int x, unsigned int y);

void
cons_clear(void);

extern void
cons_putc(unsigned char c); 

#endif /* CONSOLE_H */
