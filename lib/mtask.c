#include "os_sys.h"
#include "os_mtask.h"
#include "os_stdio2.h"
#include "string.h"

struct TIMER *mt_timer;
int mt_tr;
struct taskctl _taskctl = {0};
struct taskctl *ptaskctl;

extern void thread_lazyman_sleep();

#define MEM64K 64 * 1024

void task_init(struct MEMMAN *memman)
{
	struct task *t;
	struct TSS32 *tss; 
	ptaskctl = &_taskctl;
	ptaskctl->running = 0;
	ptaskctl->now = 0;
	t = &ptaskctl->tasks[ptaskctl->now];
	t->priority = 2; /* run per 0.02 second */
	tss = &t->tss;

	tss->ldtr = 0;
	tss->iomap = 0x40000000;
	set_segmdesc(gdt + TASK_SEG, 103, (int) tss, AR_TSS32);

	tss->eip = (int) &thread_lazyman_sleep;
	tss->eflags = 0x00000202; /* IF = 1; */
	tss->eax = 0;
	tss->ecx = 0;
	tss->edx = 0;
	tss->ebx = 0;
	tss->esp = memman_alloc_4k(memman, MEM64K) + MEM64K - 8;
	tss->ebp = 0;
	tss->esi = 0;
	tss->edi = 0;
	tss->es = 2 * 8;
	tss->cs = 1 * 8; // same segment with boot os 
	tss->ss = 2 * 8;
	tss->ds = 2 * 8;
	tss->fs = 2 * 8;
	tss->gs = 2 * 8;
	*((int *) (tss->esp + 4)) = (int) ptaskctl->now;
	t->flag = TASK_STOP;
	t->pid = 0;
	strcpy(t->processname, "sleep");
	task_run(t, 1);
}

int task_create(int func, struct MEMMAN *memman, const char *name)
{
	struct task *t;
	struct TSS32 *tss; 
	int i = 0;
	for (i = 0; i < MAX_TASK_NUM ; i++) {
		if (ptaskctl->tasks[i].flag == TASK_STOP) {
			break;
		}
	}

	if (i == MAX_TASK_NUM) {
		return -1;
	}

	t = &ptaskctl->tasks[i];
	t->priority = 2; /* run per 0.02 second */ 
	t->flag = TASK_RUN;
	tss = &t->tss;

	tss->ldtr = 0;
	tss->iomap = 0x40000000;
	set_segmdesc(gdt + TASK_SEG + i, 103, (int) tss, AR_TSS32);
	tss->eip = func;
	tss->eflags = 0x00000202; /* IF = 1; */
	tss->eax = 0;
	tss->ecx = 0;
	tss->edx = 0;
	tss->ebx = 0;
	tss->esp = memman_alloc_4k(memman, MEM64K) + MEM64K - 8;
	tss->ebp = 0;
	tss->esi = 0;
	tss->edi = 0;
	tss->es = 2 * 8;
	tss->cs = 1 * 8; // same segment with boot os 
	tss->ss = 2 * 8;
	tss->ds = 2 * 8;
	tss->fs = 2 * 8;
	tss->gs = 2 * 8;
	*((int *) (tss->esp + 4)) = (int) i;
	t->flag = TASK_STOP;
	strcpy(t->processname, name);
	t->pid = i;
	task_run(t, 1);
	return i;
}

void task_run(struct task *t, int priority)
{
	if (t->priority > 0) {
		t->priority = priority;	
	}

	if (t->flag != TASK_RUN) {
		t->flag = TASK_RUN;
		ptaskctl->taskring[ptaskctl->running] = t;
		ptaskctl->running++;
	}
}

void task_switch()
{
	struct task *t;

	ptaskctl->now++;
	if (ptaskctl->now == ptaskctl->running) {
		ptaskctl->now = 0;
	}

	t = ptaskctl->taskring[ptaskctl->now];
	farjmp(0, TASK_SEGNO(ptaskctl->now));
}

struct task *get_task(unsigned int task_id)
{
	if (task_id < MAX_TASK_NUM) {
		return &ptaskctl->tasks[task_id];
	}
	return NULL;
}
