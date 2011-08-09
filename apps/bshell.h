#ifndef BSHELL_H
#define BSHELL_H

#include "screen.h"

#define MAX_CMD_BUF_SIZE 80
struct session {
	int x;
	int y;
	int buflen;
	char buf[MAX_CMD_BUF_SIZE];
};

extern int kb_input_handling (struct session *s, int c);
extern char keytable[];
extern char keytable_shift[];
extern int key_shift;
extern int g_mtime[];

void bshell_init(struct session *s, int taskid);

#endif
