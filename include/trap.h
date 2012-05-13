#ifndef TRAP_H
#define TRAP_H
#include "types.h"

struct tregs {
	uint32_t ecx;
	uint32_t edx;
	uint32_t eax;
}__attribute__((packed));

struct trapframe {
	struct tregs regs;
	int trapno;
	int errcode;
	int eip;
	int cs;   /* use 16 bit*/
	int eflags;
	/* below only crossing rings, such user to kernel */
	int esp;
	int ss;   /* use 16 bit */
};

#define OS_KERN_CS 0x00000008

extern int trap_handlers[];

#endif
