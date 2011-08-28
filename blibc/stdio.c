#include "os_stdio.h"
#include "os_stdio2.h"
#include "os_keyboard.h"
#include "os_bits.h"
#include "string.h"

#define hex(l) ((l < 10) ? (0x30+l) : (0x41+l-10))

/************************************************************
* api for number to string
************************************************************/
char *itoa(int l, char *tol);

static inline void hexc(char c, char *s, int i)  { 
	char l = c & 0x000f;
	s[i] = hex(l);
	l = (c & 0x00f0) >> 4;
	s[i-1] = hex(l);
}

void itohex(int c, char *s)
{
	int i;
	char *p = (char *) &c;
	for (i = sizeof(c)*2 - 1; i > 0; i-=2) {
		hexc(*p, s, i);
		p++;
	}
}

/************************************************************
* api for string processing
************************************************************/

void *memcpy(void *dest, const void *src, int cnt)
{
	char *s1 = dest;
	const char *s2 = src;
	char *endptr = (char *) dest + cnt;
	
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

/* internal helper functions */
static void _PrintChar(char *fmt, char c);
void _PrintDec(char *fmt, int value);
static void _PrintHex(char *fmt, int value);
static void _PrintString(char *fmt, char *cptr);

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
	_cputs(s);
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
	char str[2] = {0};
	str[0] = c;
	puts(str);
}

unsigned getc(void)
{
	_wait_output_full();
	return inb(0x60);
}
#define false 0
#define true  1

static int atol(char *s, long *retval)
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

#define MCPY(d, t)	{ while(*t != '\0')	*d++ = *t++; }

int vsprintf(char *buf, const char *fmt, va_list args) 
{
	char snum[12] = {0};
	char const *p = fmt, *t;
	char *d = buf;
	int  i;
	char format[10];
	while(*p) {
		if (*p != '%') {
			*d++ = *p++;
			continue;
		}
		p++;
		*format = 0;
		for (i = 0; i < 10; ) {
			if (*p == 0) {
				break;
			} else if (*p=='c' || *p=='d' || *p=='x' ||
				*p=='s' || *p=='%') {
				format[i++] = *p;
				format[i] = '\0';
				break;
			}
			format[i++] = *p++;
		}

		switch(*p++) {
		case 'd': // follow next
		{
			int b = va_arg(args, int);
			t=itoa((int)b, snum);
			if (*format == '0') 
				t = snum;

			MCPY(d, t);
		}
		break;
		case 's':
			t=(const char *) va_arg(args, const char *);
			MCPY(d, t);
			break;
		case 'x':
		{
			// buf[8] without null terminated char
			int b = va_arg(args, int);
			itohex(b, snum);
			snum[8] = 0;
			t=snum;
			MCPY(d, t);
			break;
		}
		case 'c':
			*d++ = *(const char *)va_arg(args, const char *); 
			break;
		case '%':
			*d++ = '%';
			break;
		default: // unknown argument
			break;
		}
	}
	*d = '\0';
	return d - buf - 1;
}

int sprintf(char *buf, const char *fmt, ...)
{
	int rc = 0;
	va_list args;
	va_start(args, fmt);
	rc = vsprintf(buf, fmt, args);
	va_end(args);		
	return rc;
}

#define BUFSIZE 128

int printf(const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE];
	char snum[12] = {0};
	char const *p = fmt, *t;
	char *d = buf;
	va_start(args, fmt);
	int  i = 0;
	char format[10];
	while(*p) {
		if (*p != '%') {
			*d++ = *p++;
			continue;
		}
		p++;

		*format = 0;
		for (i = 0; i < 10; ) {
			if (*p == 0) {
				break;
			} else if (*p=='c' || *p=='d' || *p=='x' ||
				*p=='s' || *p=='%') {
				format[i++] = *p;
				format[i] = '\0';
				break;
			}
			format[i++] = *p++;
		}

		switch(*p++) {
		case 'd': // follow next
		{
			int b = va_arg(args, int);
			t=itoa((int)b, snum);
			if (*format == '0')
				t = snum;

			MCPY(d, t);
		}
		break;
		case 's':
			t=(const char *) va_arg(args, const char *);
			MCPY(d, t);
			break;
		case 'x':
		{
			// buf[8] without null terminated char
			int b = va_arg(args, int);
			itohex(b, snum);
			snum[8] = 0;
			t=snum;
			if (*format != '0') {
				while(*t == '0') t++;
			} else if (format[1] != 'x') {
				b = format[1] - '0';		
				if (b <= 7) {
					t += (8 - b);
				}
			}
			MCPY(d, t);
			break;
		}
		case 'c':
			*d++ = * ( (const char *)va_arg(args, const char *) );
			break;
		case '%':
			*d++ = '%';
			break;
		default: // unknown argument
			break;
		}
	}
	*d = '\0';
	va_end(args);
	puts(buf);
	return d - buf - 1;
}

/**
 * @brief printf() family produce output according to a format
 * The simple implementation in CuRT supports the following format:
 *   "%s", "%c", "%d", and "%x"
 * and the correspoding variants: "%08x", "%8x".
 */
int printf0(const char *fmt, ...)
{
	int i;
	va_list args;
	const char *s = fmt;
	char format[10];
	
	va_start(args, fmt);
	while (*s) {
		if (*s == '%') {
			s++;
			/* s in "% 08lx" format to get the record format. */
			format[0] = '%';
			for (i = 1; i < 10; ) {
				if (*s=='c' || *s=='d' || *s=='x' ||
				    *s=='s' || *s=='%') {
					format[i++] = *s;
					format[i] = '\0';
					break;
				}
				else {
					format[i++] = *s++;
				}
			}
			/* "% s", "% c", "% d", "% x" to find the print
			 * function calls */
			switch (*s++) {
				case 'c' :
					_PrintChar(format, va_arg(args, int));
					break;
				case 'd' :
					_PrintDec(format, va_arg(args, int));
					break;
				case 'x' :
					_PrintHex(format, va_arg(args, int));
					break;
				case 's' :
					_PrintString(format, va_arg(args, char *));
					break;
				case '%' :
					_PrintChar("%c", '%');
					break;
				default:
					/* it shall not happend. */
					break;
			}
		}
		else {
			_PrintChar("%c", *s);
			s++;
		}
	}
	va_end(args);

	/* FIXME: Upon successful return, printf function shall return the
	 * number of characters printed.
	 */
	return 1;
}


static void _PrintChar(char *fmt, char c)
{
	putchar(c);
	return;

}

char *itoa(int l, char *tol)
{
	int remainder;
	int neg = 0;
	int count = 0;
	char *p;
	if (l == 0) {
		tol[0] = '0';
		return tol;
	}
	if (l < 0) {
		neg = 1;
		l *= -l;
	}
	/* FIXME: we don't intend to handle the number bigger than 100000. */
	if (l > 200000000)
		return "\0";

	count = 10;
	do {
		remainder = l % 10;
		tol[count--] = '0' + remainder;
		l = (l - remainder) / 10;
	} while (count > 0);
	tol[0] = neg ? '-' : ' ';
	
	p = tol + 1;
	while(*p == '0') p++;
	return p; /* num only without sign and prefix '0' */
}

void _PrintDec(char *fmt, int l)
{
	char tol[12] = {0};
	itoa(l, tol);
	_PrintString(fmt, tol);
}

#define ATOH(c) ((c < 10) ? c + '0' : c + 'A' - 10)

static void _PrintHex(char *fmt, int l)
{
	int i, j;
	char c, *s = fmt, tol[10];
	bool flag0 = false, flagl = false;
	long flagcnt = 0;
	bool leading_zero = true;
	char uHex, lHex;
	int cnt;	
	/* format like "%081" is interpreted for '0', '8', 'l' individually. */
	for (i = 0; (c = s[i]) != 0; i++) {
		if (c == 'x')
			break;
		else if (c >= '1' && c <= '9') {
			for (j = 0; s[i] >= '0' && s[i] <= '9'; j++) {
				tol[j] = s[i++];
			}
			tol[j] = '\0';
			i--;
			atol(tol, &flagcnt);
		}
		else if (c == '0')
			flag0 = true;
		else if (c == 'l')
			flagl = true;
		else
			continue;
	}

	s = (char *)(&l);
	l = SWAP32(l);	/* depends on the endianess. */
	
	/* output, based on the flag */
	if (flagcnt) {
		if (flagcnt & 0x01) {	/* upper ignored, lower the output. */
			c = s[(8 - (flagcnt + 1)) / 2];
			
			/* check if lower 4 bits becomes ASCII code. */
			lHex = ((c >> 0) & 0x0f);
			if (lHex != 0)
				leading_zero = false;

			lHex=ATOH(lHex);

			/* lower 4 bits */
			if (leading_zero) {
				putchar(flag0 ? '0' : ' ');
			}
			else putchar(lHex);
			
			flagcnt--;
		}

		/* byte-level data, the output Hex */
		for (cnt = 0, i = (8 - flagcnt) / 2; i < 4; i++) {
			c = s[i];
				
			/* get upper 4 bits and lower 4 bits. */
			uHex = ((c >> 4) & 0x0f);
			lHex = ((c >> 0) & 0x0f);

			/* upper 4 bits and lower 4 bits to '0'~'9', 'A'~'F'.
			   upper 4 bits: ascii code */
			if (uHex != 0)
				leading_zero = false;
			uHex = ATOH(uHex);

			/* upper 4 bits */
			if (leading_zero) {
				putchar(flag0 ? '0' : ' ');
			} else 
				putchar(uHex);
			
			/* lower 4 bits: ascii code */
			if (lHex != 0)
				leading_zero = false;

			lHex = ATOH(lHex);

			/* lower 4 bits */
			if (leading_zero) {
				putchar(flag0 ? '0' : ' ');
			} else
				putchar(lHex);
		}
		return ;
	}
	for (i = 0; i < 4; i++){
		c = s[i];

		/* get upper 4 bits and lower 4 bits. */
		uHex = ((c >> 4) & 0x0f);
		lHex = ((c >> 0) & 0x0f);

		/* upper 4 bits and lower 4 bits to '0'~'9', 'A'~'F'. */
		if (uHex != 0)
			leading_zero = false;

		uHex = ATOH(uHex);

		if (!leading_zero)
			putchar(uHex);
		
		if (lHex != 0 || i == 3)
			leading_zero = false;
		lHex = ATOH(lHex);

		if (!leading_zero)
			putchar(lHex);
	}
	return;
}

static void _PrintString(char *fmt, char *s)
{
	if (!fmt || !s)
		return;
	puts(s);
	return;
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

