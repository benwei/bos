#include "string.h"

void strcpy(char *dest, const char *src)
{
	while(*src) *dest++ = *src++;
}

size_t strlen(const char *s)
{
	const char *end = s;
	while(*end++ != 0) ;
	return (size_t)(end - s) - 1;
}
