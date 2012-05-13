#include "os_mtask.h"

int g_mtime[MAX_TASK_NUM];

/* functions for app used */
int getpid_from_task(struct task *t)
{
	return t->pid;
}

int getmtime_by_pid(int pid)
{
	return g_mtime[pid];
}
int update_mtime_by_pid(int pid, int offset)
{
	g_mtime[pid] += offset;
	return 0;
}
