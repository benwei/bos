#ifndef KPROC_H
#define KPROC_H

struct proc {
	int handle;
};

struct ps {
	int pid;	
	int mtime;
	int state;
	const char *name;
};


void
proc_init(struct proc *p);

/**
 * if data existed, return msg string
 * else EOL, return NULL;
 **/
int
proc_read_ps(struct proc* p, struct ps *ps);

#endif /* KPROC_H */
