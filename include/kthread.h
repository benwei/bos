#ifndef KTHREAD_H
#define KTHREAD_H

/* for apps */
#include "os_mtask.h"
int getpid_from_task(struct task *t);
int getmtime_by_pid(int pid);

/* for init.c */
void thread_lazyman_sleep(int task_id);
void thread_kb_io(int task_id);

#endif /* KTHREAD_H */
