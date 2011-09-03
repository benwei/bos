#ifndef MEMORY_H
#define MEMORY_H
#include "types.h"
typedef struct e820_st {
	uint32_t addr_high;
	uint32_t addr_low;
	uint32_t len_high;
	uint32_t len_low;
	uint32_t type;
} e820_t;

#define E820_ADDR_RANGE    1
#define E820_ADDR_RESERVED 2
#define MAX_E820_NUM 20

#define PGSIZE 4096

/* from main.s */
int get_e820_num(void);
const unsigned int *get_e820_data(void);

/* for system init and retrieve memory information */
extern uint32_t e820_smap_num;
extern e820_t e820_info[];
extern unsigned int mem_upbound_size;
extern void calc_e820(void);

#endif /* MEMORY_H */
