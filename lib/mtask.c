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
static void task_add(struct task *t);

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
	task_add(t);
}

#define TSS_SIZE 103

int
task_create(int func, struct MEMMAN *memman, const char *name, int active)
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
	if (i == 2) {
		#define AR_RING3 0x0
		set_segmdesc(gdt + TASK_SEG + i, TSS_SIZE, (int) tss, 0x89| AR_RING3<<5);
		tss->cs = 1 * 8; 
	} else {
		set_segmdesc(gdt + TASK_SEG + i, TSS_SIZE, (int) tss, AR_TSS32);
		tss->cs = 1 * 8; 
	}
	tss->ss = 2 * 8;
	tss->ds = 2 * 8;
	tss->fs = 2 * 8;
	tss->gs = 2 * 8;

	*((int *) (tss->esp + 4)) = (int) i;
	t->flag = TASK_STOP;
	strcpy(t->processname, name);
	t->pid = i;
	if (active) {
		task_run(t, 1);
	}
	task_add(t);
	return i;
}

void task_add(struct task *t)
{
	ptaskctl->taskring[ptaskctl->running] = t;
	ptaskctl->running++;
}

static inline void
task_remove(struct task *t)
{
	if (ptaskctl->taskring[ptaskctl->running] == t)
	    ptaskctl->running--;
}

void task_run(struct task *t, int priority)
{
	t->priority = priority;	
	t->flag = TASK_RUN;
}

int task_getstate(struct task *t)
{
	return t->flag;
}


void task_setstate(struct task *t, int state)
{
	t->flag = state;
}

void task_schedule()
{
	struct task *t;

	do {
		ptaskctl->now++;
		if (ptaskctl->now == ptaskctl->running) {
			ptaskctl->now = 0;
		}

		t = ptaskctl->taskring[ptaskctl->now];
	} while(t->flag != TASK_RUN);

	farjmp(0, TASK_SEGNO(ptaskctl->now));
}

struct task *get_now_task(void)
{
	return ptaskctl->taskring[ptaskctl->now];
}

struct task *get_task(unsigned int task_id)
{
	if (task_id < MAX_TASK_NUM) {
		return &ptaskctl->tasks[task_id];
	}
	return NULL;
}

void task_idle(unsigned int task_id)
{
	struct task *t = get_task(task_id);
	t->flag = TASK_IDLE;
}

void task_stop(unsigned int task_id)
{
	struct task *t = get_task(task_id);
	task_remove(t);
}

void thread_wakeup(struct task *t);

void task_start(unsigned int task_id)
{
	struct task *t = get_task(task_id);
	if (t->flag == TASK_IDLE)
		thread_wakeup(t);

	t->flag = TASK_RUN;
}

int
task_wait(unsigned int task_id)
{
       struct task *t = get_task(task_id);
       while (t->flag == TASK_RUN){
               ;;
       }
       return 0;
}
