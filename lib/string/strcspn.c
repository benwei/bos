/*
 * libc/string/strcspn.c
 */

#include <types.h>
#include <stddef.h>
#include <string.h>

size_t strcspn(const char * s, const char * reject)
{
	const char * p;
	const char * r;
	size_t count = 0;

	for (p = s; *p != '\0'; ++p)
	{
		for (r = reject; *r != '\0'; ++r)
		{
			if (*p == *r)
				return count;
		}
		++count;
	}
	return count;
}
