#include "os_mtask.h"
#include "stdio.h"
#include "kproc.h"
#include "kthread.h"

void proc_init(struct proc *p)
{
	p->handle = 0;
}

/**
 * if data existed, return msg string
 * else EOL, return NULL;
 **/

int
proc_read_ps(struct proc* p, struct ps *ps)
{
	if (p->handle >= ptaskctl->running)
		return 0;

	struct task *t = &ptaskctl->tasks[p->handle++];
	ps->name = t->processname;
	ps->pid = getpid_from_task(t);
	ps->mtime = getmtime_by_pid(t->pid);

	switch(t->flag) {
	case TASK_RUN:
		ps->state=0; break;
	case TASK_STOP:
		ps->state=1; break;
	case TASK_IDLE:
	default:
		ps->state=2; break;
	}
	return 1;
}

