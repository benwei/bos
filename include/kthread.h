#ifndef KTHREAD_H
#define KTHREAD_H

/* for apps */
#include "os_mtask.h"
#include "os_fifo.h"
int getpid_from_task(struct task *t);
int getmtime_by_pid(int pid);

/* for init.c */
void thread_lazyman_sleep(int task_id);
void thread_kb_io(int task_id);
void thread_events(int task_id);

#define MAX_FIFO_BUF_SIZE 128

struct thread_message {
	struct FIFO32 fifo;
	int fifobuf[MAX_FIFO_BUF_SIZE];
	struct TIMER *tsw;
	int timer_id;
	int task_id;
	int event;
};

#define THREAD_MESSAGE_MAX 10


#endif /* KTHREAD_H */
