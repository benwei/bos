#ifndef __OS_MEMORY_H__
#define __OS_MEMORY_H__

#include "types.h"

#define MEMMAN_FREES		4090 /* 32KB */
#define MEMMAN_ADDR		0x003c0000

/*
+----------+ 00400000
|          |
|  MEMMAN  |
|          |
+----------+ 003c0000
*/
struct FREEINFO {
	uint32_t addr, size;
};
struct MEMMAN {	
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];
};
uint32_t memtest(uint32_t start, uint32_t end);
void memman_init(struct MEMMAN *man);
uint32_t memman_total(struct MEMMAN *man);
uint32_t memman_alloc(struct MEMMAN *man, uint32_t size);
int memman_free(struct MEMMAN *man, uint32_t addr, uint32_t size);
uint32_t memman_alloc_4k(struct MEMMAN *man, uint32_t size);
int memman_free_4k(struct MEMMAN *man, uint32_t addr, uint32_t size);

#endif /* __OS_MEMORY_H__ */
