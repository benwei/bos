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

int strncmp(const char *s1, const char *s2, int n)
{
	while (*s1 != '\0' && *s2 != '\0' && *s1 == *s2 && (--n)>0) {
		s1++;
		s2++;
	}
	return (*(unsigned char *) s1) - (*(unsigned char *) s2);
}

int strcmp(const char *s1, const char *s2)
{
	while (*s1 != '\0' && *s2 != '\0' && *s1 == *s2) {
		s1++;
		s2++;
	}
	return (*(unsigned char *) s1) - (*(unsigned char *) s2);
}

