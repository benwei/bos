#include "os_stdio2.h"
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;

#if USE_INLINE_ASM
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
#else
extern uint32_t _inl(int port);
extern void _outl(int port, uint32_t data);
#define inl(port) _inl(port)
#define outl(port, data) _outl(port, data)
#endif
/* pci definitions */
static uint32_t pci_conf1_ioaddr = 0x0CF8;
static uint32_t pci_conf1_iodata = 0x0CFC;


static __inline void
pci_conf_apply_addr(uint16_t bus, uint16_t slot,
		   uint16_t func, uint16_t offset)
{
	unsigned long addr;
	unsigned long lbus = (unsigned long)bus;
	unsigned long lslot = (unsigned long)slot;
	unsigned long lfunc = (unsigned long)func;
	/* 31    : Enable Bit
	   24-30 : Reserved
	   16-23 : Bus Number
	   11-15 : Slot Number = Device Number
	   08-10 : Function Number
	   02-07 : Register Number
	   00-01 : 00
	*/

	/* create configuration address as per Figure 1 */
	addr = (unsigned long)((lbus << 16) | (lslot << 11) |
			  (lfunc << 8) | (offset & 0xfc) | 1<<31);
 
	/* write out the address */
	outl(pci_conf1_ioaddr, addr);
}

uint32_t
pci_conf_readl(uint16_t bus, uint16_t slot,
		   uint16_t func, uint16_t offset)
{
	pci_conf_apply_addr(bus, func, slot, offset);
	return inl(pci_conf1_iodata);
}

unsigned short
pci_conf_readw(uint16_t bus, uint16_t slot,
		   uint16_t func, uint16_t offset)
{
	pci_conf_apply_addr(bus, func, slot, offset);
	/* read word in the data */
	return (unsigned short)((inl(pci_conf1_iodata) 
		>> ((offset & 2) * 8)) & 0xffff);
}

typedef unsigned char uint8_t;
struct pci_data_st {
	uint16_t vendor;
	uint16_t device;
	uint16_t class_code;
	uint8_t  progif;
	uint8_t  revid;
};

typedef struct pci_data_st * pci_data_t;

#define MAX_SLOT 32

static struct pci_data_st pci_data[MAX_SLOT];
static int dev_count = 0;

static int pci_scan_bus()
{
	int bus = 0, slot = 0;
	pci_data_t dev = pci_data;

	uint16_t vendor;
	uint16_t tmp;
	for ( ; slot < MAX_SLOT ; slot++) {
		/* vendors that == 0xFFFF, it must be a non-existent device. */
		if ((vendor = pci_conf_readw(bus,slot,0,0)) != 0xFFFF) {
			dev->vendor = vendor;
			dev->device = pci_conf_readw(bus,slot,0,2);
			dev->class_code= pci_conf_readw(bus,slot,0,0xA);
			tmp = pci_conf_readw(bus,slot,0,0x8);
			dev->progif= tmp >> 8;
			dev->revid = tmp & 0xFF;
			dev_count++;
			dev++;
		}
	}
	return 0;
}

static const char *pci_class_string[] =
{
        "Unknown", 	      /* 0 */
        "Storage controller", /* 1 */
        "Network controller", /* 2 */
        "Display controller", /* 3 */
        "Multimedia device",  /* 4 */
        "Memory controller",  /* 5 */
        "Bridge device",      /* 6 */
};

const char *get_pci_class_string(unsigned short classcode) {
	if (classcode > 6) return pci_class_string[0];	
	return pci_class_string[classcode];
}

int lspci(void)
{
	int i = 0;
	pci_data_t dev = pci_data;
	for (;i < dev_count; i++) {
		printf("\nvendor:0x%04x, devid:0x%04x,class:%04x(%s)",
		   dev->vendor,dev->device,
		   dev->class_code, get_pci_class_string(dev->class_code >> 8)
		   );
		if (dev->revid > 0) {
		   printf(",rev=%01x",
		   dev->revid);
		}
		dev++;
	}
	return dev_count;
}

int pci_init(void)
{
	return pci_scan_bus();
}
