#include "os_stdio.h"
#include "os_stdio2.h"
#include "os_keyboard.h"
// #include "os_bits.h"
#include "string.h"

char *itoa(int l, char *tol, int size);
#define MCPY(d, t)	{ while(*t != '\0')	*d++ = *t++; }
#define BUFSIZE 256
#define MAX_NUMSIZE 12
#define MAX_FMTSIZE 10

int vsprintf(char *buf, const char *fmt, va_list args)
{
	char snum[MAX_NUMSIZE] = {0};
	char const *p = fmt, *t;
	char *d = buf;
	int  i = 0;
	char format[MAX_FMTSIZE] = {0};
	while(*p) {
		if (*p != '%') {
			*d++ = *p++;
			continue;
		}
		p++;
		*format = 0;
		for (i = 0; i < MAX_FMTSIZE; ) {
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
			t=itoa((int)b, snum, sizeof(snum));

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
			t = itohex(b, snum, 9);
			if (*format == '0') {
				unsigned char flen = 0;
				t = snum;
				if (format[1] != 'x') {
					flen = format[1] - '0';
					t+= flen;
				}
			}
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

int printf(const char *fmt, ...)
{
	int rc = 0;
	va_list args;
	char buf[BUFSIZE];
	va_start(args, fmt);
	rc = vsprintf(buf, fmt, args);
	va_end(args);
	puts(buf);
	return rc;
}

