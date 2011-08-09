/*
* @file init.c
* setup irq for keyboard and timer
* initial the multi-tasking
* startup sleep and keyboard_io kernel thread
* shell is running at keyboard_io kernel thread
*/

#include "os_mtask.h"
#include "os_stdio.h"
#include "os_stdio2.h"
#include "os_bits.h"
#include "bshell.h"
#include "string.h"

#ifndef BOS_VERSION
#define BOS_VERSION "0.2"
#endif

int key_shift = 0;

#define MAX_KEYTB_SIZE 0x54
/* map for keyboard with 101 key */
char keytable[MAX_KEYTB_SIZE] = {
      //0    1    2     3    4    5    6    7    8    9    A    B    C    D    E   F
	0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,   '\t',
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0,   0,   'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', 0,   0,   '\\', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.'
};

char keytable_shift[MAX_KEYTB_SIZE] = {
      //0    1    2     3    4    5    6    7    8    9    A    B    C    D    E   F
	0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '_', '+', 0,   '\t',
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0,   0,   'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', 0,   0,   '|', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '=', '1',
	'2', '3', '0', '.'
};

static struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
static unsigned int memtotal;

#define KEY_ENTER 0x1C
#define KEY_BS    0x0e /* Back Space */
#define KB_SHIFT_L 1
#define KB_SHIFT_R 2
#define KB_RELEASED 0x80

#define KEY_SHIFT_L 0x2a
#define KEY_SHIFT_R 0x36
#define KEY_TAB     0x0f
#define KEY_SHIFT_RL KEY_SHIFT_L | KB_RELEASED 
#define KEY_SHIFT_RR KEY_SHIFT_R | KB_RELEASED 

/* console definitions */
#define MEM_MB(x) ((x)/1024/1024)



#define new_line(s) { \
	move_cursor(s->x,s->y); \
	clearline(); \
	move_cursor(s->x,s->y); }

int strlen(const char *s);

void command_free(struct session *s)
{
	int freemem = memman_total(memman);
	new_line(s);
	printf("Free/Totoal: %d/%d MB",
		MEM_MB(freemem), 
		MEM_MB(memtotal)); 
	s->y++; /* mem line */
}


#define STR_PROMPT "bos$"

static inline void show_prompt(struct session *s, const char *prompt)
{
	new_line(s);
	puts(prompt);
	s->x+=strlen(prompt) + 1;
}

void command_help(struct session *s)
{
	printf( "free  - memory info\n"
		"clear - clear screen\n"
		"ps    - process status\n"
		"uname - show system info\n"
		"type  - dump memory info\n");
	s->y+=5;
}

extern int getpid_from_task(struct task *t);

static int getmtime_by_pid(int pid)
{
	return g_mtime[pid];
}

void command_ps(struct session *s)
{
	struct task *t;
	int i;
	for (i = 0; i < MAX_TASK_NUM ; i++) {
		t = &ptaskctl->tasks[i];
		if (t->flag == TASK_RUN) {
			int pid = getpid_from_task(t);
			new_line(s);
			printf("%d %s - running %d\n", pid, t->processname, getmtime_by_pid(pid));
			s->y++;
		}
	}
}

void command_clear(struct session *s)
{
	clrscr();
	s->y=0;
}

void command_uname(struct session *s)
{
	new_line(s);
	printf("BenOS "BOS_VERSION"\n");
	s->y++; 
}

void hexdump(struct session *s, const char *addr , int len)
{
	int i = len;
	char c = 0;
	const char *p = addr;
	printf("%08x ", (int) addr);
	for (; i > 0; --i, ++p) {
		printf("%02x ", *p);
	}
	p = addr;
	for (; len > 0; --len, ++p) {
		c = *p;
		if ( ('0' <= c && c <= '9') ||
		     ('a' <= c && c <= 'z') ||
		     ('A' <= c && c <= 'Z') || c == ' ') {
			printf("%c", &c);
		} else {
			printf(".");
		}
	}
}

void command_type(struct session *s)
{
	int i = 0;
	const char *arg = s->buf + 5;
	unsigned long tadr = strtoul(arg, NULL, 16);
	const char *p = (const char *) tadr;

	new_line(s);
	printf("type command %x\n", tadr);
	
	s->y++; 
	for  (; i < 16; i++) {
		new_line(s);
		hexdump(s, p, 16);
		p+=16;
		s->y++;
	}
}

void command_exec(struct session *s)
{
	s->x=0;
	s->y++;
	if (s->buflen == 0) {
		; /* new line */
	} else if (strcmp(s->buf, "help") == 0 ) {
		new_line(s);
		command_help(s);
	} else if (strcmp(s->buf, "ps") == 0 ) {
		command_ps(s);
	} else if (strcmp(s->buf, "clear") == 0 ) {
		command_clear(s);
	} else if (strcmp(s->buf, "free") == 0 ) {
		command_free(s);
	} else if (strcmp(s->buf, "uname") == 0 ) {
		command_uname(s);
	} else if (strncmp(s->buf, "type ", 5) == 0 ) {
		command_type(s);
	} else {
		new_line(s);
		printf("invalid command\n");
		s->y++;
	} 

	// display prompt string bos$ 
	s->x = 0;
	show_prompt(s, STR_PROMPT);
}

void bshell_init(struct session *s, int task_id)
{
	move_cursor(s->x,s->y);
	printf("%s started(pid=%d)\n", "bshell", (void *) task_id);
	s->y++;
	show_prompt(s, STR_PROMPT);
}

/*
 shell within keyboard_io kernel thread
 */
int kb_input_handling (struct session *s, int c)
{
	if (256 <= c && c <=511) {
		c-= 256;
		switch(c) {
		case KEY_SHIFT_L: /* left shift on */
			key_shift |= KB_SHIFT_L;	
		break;
		case KEY_SHIFT_R: /* right shift on */
			key_shift |= KB_SHIFT_R;	
		break;
		case KEY_SHIFT_RL: /* left shift off */
			key_shift &= ~KB_SHIFT_L;	
		break;
		case KEY_SHIFT_RR: /* right shift off */
			key_shift &= ~KB_SHIFT_R;	
		break;
		case KEY_TAB:
			putc('\t');
			s->x+=8;
		break;
		default:
			if (c < MAX_KEYTB_SIZE) {
				int ch;
				if ((key_shift & KB_SHIFT_L)
				     || (key_shift & KB_SHIFT_R))
					ch = keytable_shift[c];
				else
					ch = keytable[c];

				if (ch != 0) {
					move_cursor(s->x,s->y);
					putc(ch); s->x++;
					if (s->buflen < MAX_CMD_BUF_SIZE - 1) {
						s->buf[s->buflen++] = ch;
					}
				} else if (c == KEY_BS) { /* process backspace key */
					if (s->buflen > 0) {
						s->buf[--s->buflen] = '\0';
						s->x--;
						move_cursor(s->x,s->y);
						printf(" ");
						move_cursor(s->x,s->y);
					} 
				} else if (c == KEY_ENTER) {
					s->buf[s->buflen] = 0;
					command_exec(s);
					s->buflen = 0;
					s->buf[0] = 0;
				} else {
					move_cursor(s->x,s->y);
					clearline();
					move_cursor(s->x,s->y);
					printf("code:%x\n", (void *) c, NULL);
					s->x= 0;
					s->y++;
				}
			}
		}
	}

	if (s->x > 79) {
		s->x = 0;
		s->y++;
	}

	if (s->y >= 24) {
		s->y = 0;
	}
	return 0;
}

