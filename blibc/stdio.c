#include "os_stdio.h"
#include "os_stdio2.h"
#include "os_keyboard.h"
#include "os_bits.h"
#include "string.h"
#include "console.h"

/************************************************************
* api for string processing
************************************************************/

void *memcpy(void *dest, const void *src, size_t n)
{
	char *s1 = dest;
	const char *s2 = src;
	char *endptr = (char *) dest + n;

	while (s1 < endptr)
		*s1++ = *s2++;
	return dest;
}

void *memset(void *dest, int c, unsigned int size)
{
	char *s = dest;
	char *tof=s+size;
	while(s < tof) {
		*s++ = c;
	}
	return dest;
}


static const char *fd_msg[] = {"N/A", "360KB", "1.2MB", "720KB", "1.44MB", 0};
const char *get_fd_msg(unsigned char c)
{
	if (c > 4) c = 0;
	return fd_msg[c];
}

int KEY_STATBIT_SHIFT = 1;
static int key_statbits = 0;
unsigned char getchar(void)
{
	int doloop = 0;
	int k;
	unsigned char c;
	do {
		do {
			c=getc();
			if (c == KEY_SHIFT) {
				BIT_SET(key_statbits, KEY_STATBIT_SHIFT);
			}
		} while (! (c & 0x80) || c==KEY_SHIFT);

		k = c & 0x7f;
		if (k == KEY_SHIFT) {
			BIT_USET(key_statbits, KEY_STATBIT_SHIFT);
			doloop = 1;
		} else {
			if (KEY_1 <= k && KEY_0 >= k) {
				k = (((k - 1)%10) + 0x30);
			} else if (BIT_ISSET(key_statbits, KEY_STATBIT_SHIFT) ) {
				/* char upper case */
			}
			doloop = 0;
		}
	} while (doloop==1);
	return k;
}

typedef int bool;

int _x = 0;
int _y = 0;
void move_cursor(int x, int y) {
	_x = x;
	_y = y;
	_movcursor(x, y);
}

#define HAVE_KERNEL_PUTS 1
#if HAVE_KERNEL_PUTS
int _cputchar(int c);
void putchar(char c)
{
	_cputchar(c);
}

void puts(const char *s)
{
	/* use console api
 	* _cputs(s); 
 	*/
	for (;*s != 0 ; s++)
		cons_putc(*s);	
}

#else
#define MAX_X 80
#define MAX_Y 25
#define VMEMXY(x, y) ((x + y * MAX_X) * 2)
static char *vmem = (char *) 0xB8000000;
void putchar(char c) 
{
	if (_x + 1 > MAX_X) {
		_x = 0;
		_y++;
	}
	if (_y + 1 > MAX_Y) {
		_y = 0;
	}
	*(vmem + VMEMXY(_x, _y)) = c ;
	_x++;
}

void puts(char *s)
{
	while(*s != '\0') {
		if (_x + 1 > MAX_X) {
			_x = 0;
			_y++;
		}
		if (_y + 1 > MAX_Y) {
			_y = 0;
		}
		if (*s == '\n') 
		_y++;
		else
		*(vmem + VMEMXY(_x++, _y)) = *s++ ;
		_x++;
	}
}
#endif


void putc(unsigned char c)
{
	cons_putc(c);
}

unsigned getc(void)
{
	_wait_output_full();
	return inb(0x60);
}
#define false 0
#define true  1

int atol(char *s, long *retval)
{
	long remainder;
	if (!s || !s[0])
		return false;

	for (*retval = 0; *s; s++) {
		if (*s < '0' || *s > '9')
			return false;
		remainder = *s - '0';
		*retval = *retval * 10 + remainder;
	}

	return true;
}


#define BS 0x08
#define CR 0x0a

int gets(char *s)
{
	int cnt = 0;
	char  c;

	while ((c = getc()) != CR) {
		if (c != BS) {
			cnt++;
			*s++ = c;
			printf("%c",c );
		}
		else {
			if (cnt > 0) {
				cnt--;
				*s-- = ' ';
				printf("\b \b");
			}
		}
	}
	*s = 0;
	return cnt;
}

static char * ___strtok;
char * strtok(char * s,const char * ct)
{
	char *sbegin, *send;

	sbegin  = s ? s : ___strtok;
	if (!sbegin) {
		return NULL;
	}
	sbegin += strspn(sbegin, ct);
	if (*sbegin == '\0') {
		___strtok = NULL;
		return( NULL );
	}
	send = strpbrk(sbegin, ct);
	if (send && *send != '\0')
		*send++ = '\0';
	___strtok = send;
	return (sbegin);
}


unsigned int strspn(const char *s, const char *accept)
{
	const char *p;
	const char *a;
	unsigned int count = 0;

	for (p = s; *p != '\0'; ++p) {
		for (a = accept; *a != '\0'; ++a) {
			if (*p == *a)
				break;
		}
		if (*a == '\0')
			return count;
		++count;
	}
	return count;
}

char * strpbrk(const char * cs,const char * ct)
{
	const char *sc1, *sc2;

	for (sc1 = cs; *sc1 != '\0'; ++sc1) {
		for (sc2 = ct; *sc2 != '\0'; ++sc2) {
			if (*sc1 == *sc2)
				return (char *) sc1;
		}
	}
	return NULL;
}

unsigned long strtoul(const char *str, char **endptr, int requestedbase)
{
	unsigned long num = 0;
	char c;
	unsigned char digit;
	int base = 10;
	int nchars = 0;
	int leadingZero = 0;
	unsigned char strtoul_err = 0;

	while ((c = *str) != 0) {
		if (nchars == 0 && c == '0') {
			leadingZero = 1;
			goto step;
		}
		else if (leadingZero && nchars == 1) {
			if (c == 'x') {
				base = 16;
				goto step;
			}
			else if (c == 'o') {
				base = 8;
				goto step;
			}
		}
		if (c >= '0' && c <= '9') {
			digit = c - '0';
		}
		else if (c >= 'a' && c <= 'z') {
			digit = c - 'a' + 10;
		}
		else if (c >= 'A' && c <= 'Z') {
			digit = c - 'A' + 10;
		}
		else {
			strtoul_err = 3;
			return 0;
		}
		if (digit >= base) {
			strtoul_err = 4;
			return 0;
		}
		num *= base;
		num += digit;
step:
		str++;
		nchars++;
	}
	return num;
}

