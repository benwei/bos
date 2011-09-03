#include "types.h"

char *itoa(int l, char *tol, int size)
{
	int r;
	int neg = 0;
	int count = 0;
	int x = l;
	if (size < 2) {
		*tol = 0;
		return tol;
	}

	if (l == 0) {
		tol[0] = '0';
		tol[1] = '\0';
		return tol;
	} else if (x < 0) {
		neg = 1;
	}

	count = size - 2;
	while (x != 0 && count > 0) {
		r = x % 10;
		tol[count--] = '0' + ((r > 0) ? r : -r);
		x /= 10;
	}
	if (neg) {
		tol[count] = '-';
	} else {
		count++;
	}
	return tol + count;/* num only without sign and prefix '0' */
}


#define hex(l) ((l < 10) ? (0x30+l) : (0x41+l-10))
static inline void
hexc(char c, char *s, int i)  {
	char l = c & 0x000f;
	s[i] = hex(l);
	l = (c & 0x00f0) >> 4;
	s[i-1] = hex(l);
}

const char *itohex(uint32_t c, char *s, int size)
{
	int i;
	char *p = (char *) &c;
	for (i = sizeof(c)*2 - 1; i > 0; i-=2) {
		hexc(*p, s, i);
		p++;
	}

	s[size-1] = 0;

	p = s;
	while (*p != 0 && *p == '0') p++;
	return p;
}

