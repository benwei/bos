#ifndef SYSCALL_H
#define SYSCALL_H

#include "user/syscall.h"
extern int
kern_syscall(uint32_t num, uint32_t a1, uint32_t a2);

#endif
