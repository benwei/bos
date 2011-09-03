#include "os_stdio.h"
#include "os_stdio2.h"
#include "os_keyboard.h"
#include "string.h"

char *itoa(int l, char *tol, int size);
const char *itohex(int c, char *s, int size, int upper);

#define MCPY(d, t)	{ while(*t != '\0')	*d++ = *t++; }
#define BUFSIZE       256
#define MAX_NUMSIZE    12
#define MAX_FMTSIZE    10
#define MAX_HEX_NUMLEN  8

#define is_num(c) (c >= '0' && c <= '9')
#define numval(c)  (c - '0')

static int
fmt_handling(uint32_t value, const char *fmt, int fmtlen, char *buf, int buflen)
{
	const char *t;
	uint8_t flen = 0;
	int fmttype = fmt[fmtlen-1];
	int upper = 1;

	if (fmttype - 'a' > 0) 
		upper = 0;

	t = itohex(value, buf, buflen+1, upper);
	if (*fmt != '0') { /* no leading zero char */
		return t - buf;
	}
	t = buf;
	if (is_num(fmt[1])) {
		flen = numval(fmt[1]);
		if (flen < buflen) {
			t+= buflen - flen;
		}
	}
	return t - buf;
}

/* Notice: if buffer size is too small, you will hit buffer overflow */
/* known issue: this part need to modify if we want to support 64bit */
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
				*p=='s' || *p=='%' || *p == 'p') {
				format[i++] = *p;
				format[i] = '\0';
				break;
			}
			format[i++] = *p++;
		}

		switch(*p++) {
		case 'd':
		{
			int b = va_arg(args, int);
			t=itoa((int)b, snum, MAX_NUMSIZE);

			MCPY(d, t);
		}
		break;
		case 's':
			t=(const char *) va_arg(args, const char *);
			MCPY(d, t);
			break;
		case 'p': 
		{ 
			uint32_t b = va_arg(args, uint32_t);
			uint32_t offset = fmt_handling(b, format, i, snum + 2, MAX_HEX_NUMLEN);
			char *p = snum+offset;
			p[0] = '0';
			p[1] = 'x';
			MCPY(d, p);
			break;
		}
		case 'x': 
		{
			uint32_t b = va_arg(args, uint32_t);
			uint32_t offset = fmt_handling(b, format, i, snum, MAX_HEX_NUMLEN);
			t = snum+offset;
			MCPY(d, t);
			break;
		}
		case 'c':
			*d++ = *(const char *)va_arg(args, const char *);
			break;
		case '%':
			*d++ = '%';
			break;
		default: /* unknown argument */
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


void panic(const char *fmt, ...) {
	int rc = 0;
	char buf[BUFSIZE];
	va_list args;
	va_start(args, fmt);
	rc = vsprintf(buf, fmt, args);
	va_end(args);
	puts(buf);
	/* need kernel exception dump ; system halt */
}
