#include "memory.h"

e820_t e820_info[MAX_E820_NUM];
uint32_t e820_smap_num;
unsigned int mem_upbound_size = 0;

void calc_e820(void)
{
	int i; 
	e820_smap_num = get_e820_num();
	const unsigned int *p = get_e820_data();
	e820_t *e = e820_info;
	for (i = 0; i < e820_smap_num; i++, e++) {
		e->addr_low = *(p++);
		e->addr_high= *(p++);
		e->len_low  = *(p++);
		e->len_high = *(p++);
		e->type     = *(p++);
		if (e->type == E820_ADDR_RANGE){
			uint32_t tmp = e->addr_low + e->len_low;
			if (mem_upbound_size < tmp)
				mem_upbound_size = tmp;
		}
	}
}

