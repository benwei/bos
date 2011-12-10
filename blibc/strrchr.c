/*
 * libc/string/strrchr.c
 */

#include <types.h>
#include <stddef.h>
#include <string.h>

char * strrchr(const char * s, int c)
{
	const char * p = s + strlen(s);

	do {
	   if (*p == (char)c)
		   return (char *)p;
	} while (--p >= s);

	return NULL;
}
