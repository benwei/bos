#include "os_mtask.h"
#include "syscall.h"
#include "stdio.h"
#include "string.h"

int kern_syscall(uint32_t num, uint32_t a1, uint32_t a2)
{
	int ret = 0;
	if (num == BSYS_PUTS) {
		do {
		struct task *t = get_now_task();
		printf("task_id(%d) syscall(BSYS_PUTS,a2=%d),a1: %s\n", t->pid, a2, a1);
		ret = strlen((const char *) a1);
		} while(0);
	}
	return ret;
}
