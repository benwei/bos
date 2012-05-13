#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#define BSYS_PUTS 8
#define TRAP_SYSCALL 68
unsigned int syscall(int num, unsigned int a1, unsigned int a2);

#endif
