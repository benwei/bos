#ifndef TRAP_H
#define TRAP_H

struct trapframe {
	int trapno, errcode, eip, cs, eflags;
};

#define OS_KERN_CS 0x00000008

extern int trap_handlers[];

#endif
