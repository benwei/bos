#include "console.h"
#include "string.h"
#include "arch/i386/inc/vga.h"

#define CONS_VBASE OS_VIDEO_ADDR;
#define CONS_CHAR_PER_LINE 160
#define CONS_ROWS 25

/* active console */
static console cons_first;
static console *cons_act = & cons_first;

static inline void
k_cons_init(console *c)
{
	c->vmem_addr = CONS_VBASE;	
	c->vmem_max = c->vmem_addr + CONS_CHAR_PER_LINE * CONS_ROWS;
	c->vmem_current = c->vmem_addr;
	c->x = c->y = 0;
}

void
cons_init()
{
	k_cons_init(cons_act);
}

void
cons_clear(void)
{
	char *p = (char *)cons_act->vmem_addr;
	for ( ; p < (char *) cons_act->vmem_max; p+=2) {
		*p = ' ';
	}
	cons_act->x = cons_act->y = 0;
	cons_act->vmem_current = cons_act->vmem_addr;
	vga_setcursor(cons_act->x, cons_act->y); 
}


static inline void
k_cons_setcursor(console *c) {
	c->y = (c->vmem_current - c->vmem_addr);
	c->x = (c->y % CONS_CHAR_PER_LINE)/2;
	c->y /= CONS_CHAR_PER_LINE;	
	vga_setcursor(c->x, c->y);
}

void
scrollup(unsigned int linenum);

static inline void
k_cons_putc(console *c, unsigned char ch) {
	switch(ch) {
	case '\n':
	{
		int offset = (c->vmem_current - c->vmem_addr)
				% CONS_CHAR_PER_LINE;
		c->vmem_current += CONS_CHAR_PER_LINE - offset;
		if (c->vmem_current >= c->vmem_max) {
			scrollup(1);	
		}
	}
		break;
	case '\b':
		if (c->vmem_current > c->vmem_addr) {
			c->vmem_current-=2;
			*((unsigned char *) c->vmem_current) = ' ';
		}
		break;
	default:
		if (c->vmem_current >= c->vmem_max) {
			scrollup(1);
		}
		*((unsigned char *) c->vmem_current) = ch;
		c->vmem_current+=2;
	}
	k_cons_setcursor(c);
}

void
cons_putc(unsigned char c) {
	k_cons_putc(cons_act, c);
}

void
scrollup(unsigned int linenum)
{
	unsigned int offset;
	
	char *p=(char *) cons_act->vmem_addr;
	char *s, *pend;
	if (linenum == 0)
		return;

	pend = (char *) cons_act->vmem_max;
	offset = linenum * CONS_CHAR_PER_LINE;
	for (s = p + offset ; s < pend; ++p, ++s) {
		*p = *s;
	}

	cons_act->vmem_current = cons_act->vmem_max - offset;

	for (s = pend; s >= (char *) cons_act->vmem_current; s-=2) {
		*s = ' ';
	} 
}
