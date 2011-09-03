#ifndef PCI_H
#define PCI_H

#include "types.h"
#include "os_stdio2.h"

#if USE_INLINE_ASM /* reference the MIT OSE API */
static __inline uint32_t inl(int port)
{
	uint32_t data;
	__asm __volatile("inl %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

static __inline void outl(int port, uint32_t data)
{
	__asm __volatile("outl %0,%w1" : : "a" (data), "d" (port));
}
#else /* use version written in nasm */
extern uint32_t _inl(int port);
extern void _outl(int port, uint32_t data);
#define inl(port) _inl(port)
#define outl(port, data) _outl(port, data)
#endif /* End of USE_INLINE_ASM */

#define MAX_BASE_ADDR 6

struct pci_bus ;

struct pci_data_st {
	//struct   pci_bus *bus;
	uint32_t busno;
	uint16_t slot;
	uint16_t func;
	uint16_t vendor;
	uint16_t device;
	uint16_t class_code;
	uint8_t  progif;
	uint8_t  revid;
	uint8_t  hdrtype;

	uint32_t addr_base[MAX_BASE_ADDR];
	uint32_t addr_size[MAX_BASE_ADDR];
	uint8_t  irq_line;
};

typedef struct pci_data_st *pci_pdata_t;

typedef struct pci_driver *pci_driver_t;

struct pci_driver {
	uint16_t vendor;
	uint16_t devid;
	int (*attach_dev)(pci_pdata_t pd);
};

struct pci_bus {
    pci_pdata_t parent_bridge;
    uint32_t busno;
};

extern int pci_init(void);

void pci_func_enable(pci_pdata_t f);

/* display pci information */
extern int lspci(void);

extern const char *
get_pci_class_string(unsigned short classcode);

static __inline void
show_reg_info(int regnum, uint32_t size, uint32_t base)
{
	printf("%s region %d: %d bytes at %p\n",
		regnum ? "io": "mem", regnum, size, base);
}

static __inline void
show_pci_enabled(pci_pdata_t f)
{
	printf("PCI function %02x:%02x.%d (%04x:%04x) enabled\n",
	f->busno, f->slot, f->func,
	f->vendor, f->device);
}
#endif /* PCI_H */
