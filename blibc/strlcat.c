/*
 * libc/string/strlcat.c
 */

#include <types.h>
#include <string.h>

size_t strlcat(char * dest, const char * src, size_t n)
{
	size_t dsize = strlen(dest);
	size_t len = strlen(src);
	size_t res = dsize + len;

	dest += dsize;
	n -= dsize;

	if (len >= n)
		len = n-1;

	memcpy(dest, src, len);
	dest[len] = 0;

	return res;
}
