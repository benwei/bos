#include "os_stdio.h"
#include "stdio.h"
#include "trap.h"

/* 
 * using push esp to stack, trap_handler get trapframe struct
 */
void trap_handler(struct trapframe *tf)
{
	if (tf->trapno == 0) {
		printf("exception: divzero\n eip:%08x,cs:%08x,eflags:%08x\n", tf->eip, tf->cs, tf->eflags);

		if (tf->cs == OS_KERN_CS) {
			/* encounter an issue that repeat trap message in kernel cs */
			_syshalt();
		}
	} else {
		printf("uncaught except:%d\n eip:%08x,cs:%08x,eflags:%8x\n", tf->trapno, tf->eip, tf->cs, tf->eflags);
		_syshalt();
	}
}
