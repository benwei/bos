#include "types.h"

void *memmove(void *dest, const void *src, size_t n) {
	char *s1 = dest;
	const char *s2 = src;
	char *endptr = (char *) dest + n;

	while (s1 < endptr)
		*s1++ = *s2++;
	return dest;
}
