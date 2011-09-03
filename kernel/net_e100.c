#include "net_e100.h"

static struct pci_data_st e100 = {0};

void e100_init()
{
	// reset device before using
	// create receive queue
}

struct nic {
	uint16_t io_addr;
	uint16_t io_size;
};

struct nic nic;

#define E100_IO_INDEX  1
int e100_attach(pci_pdata_t pd) {
	pci_func_enable(pd);
	e100.bus = pd->bus;
	e100.slot = pd->slot;
	e100.func = pd->func;
	e100.device = pd->device;
	e100.vendor = pd->vendor;
	e100.class_code= pd->class_code;
	int i;
	for (i = 0; i < 6; i++) {
		e100.addr_base[i] = pd->addr_base[i];
		e100.addr_size[i] = pd->addr_size[i];
	}
	e100.irq_line = pd->irq_line;

	// initialize network interface controller
	nic.io_addr = pd->addr_base[E100_IO_INDEX];
	nic.io_size = pd->addr_size[E100_IO_INDEX];

	// initize e100 driver
	e100_init();

	return 0;
}

int if_e100(void)
{
	show_pci_enabled(&e100);
	show_reg_info(0, e100.addr_size[0], e100.addr_base[0]);
	show_reg_info(1, nic.io_size, nic.io_addr);
	return 3; /* screen line for print */
}
