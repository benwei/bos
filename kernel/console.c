#include "console.h"

#define CONS_VBASE 0xB8000;
#define CONS_CHAR_PER_LINE 160
#define CONS_ROWS 25

/* active console */
static console *cons;

console *cons_init(console *c) {
	c->vmem_addr = CONS_VBASE;	
	c->vmem_max = CONS_CHAR_PER_LINE * CONS_ROWS;
	c->vmem_current = 0;
	c->x = c->y = 0;
	cons = c;
	return cons;
}

void cons_active(console *be_active) {
	cons = be_active;
}

void scrollup(unsigned int linenum){
	unsigned int offset;
	
	char *p=(char *) cons->vmem_addr;
	char *s;
	char *pend;
	if (linenum == 0)
		return;

	pend = p + cons->vmem_max;
	offset = linenum * CONS_CHAR_PER_LINE;
	for (s = p + offset ; s < pend; ++p, ++s) {
		*p = *s;
	}

	/* disable clear*/
	return;
	for (s = pend - offset; s < pend ; s+=2) {
		*s = ' ';
	} 
}
