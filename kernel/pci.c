#include "os_stdio2.h"
#include "string.h"
#include "pcireg.h"
#include "os_pci.h"
#include "net_e100.h"

/* pci definitions */
static uint32_t pci_conf1_ioaddr = 0x0CF8;
static uint32_t pci_conf1_iodata = 0x0CFC;



static int pci_bridge_attach(pci_pdata_t pd);

struct pci_driver pci_attach_class[] = {
	{ PCI_CLASS_BRIDGE, PCI_SUBCLASS_BRIDGE_PCI, &pci_bridge_attach },
	{ 0, 0, 0 },
};

static __inline void
pci_conf_apply_addr(uint16_t bus, uint16_t slot,
		   uint16_t func, uint16_t offset)
{
	uint32_t addr;
	uint32_t lbus = (uint32_t)bus;
	uint32_t lslot = (uint32_t)slot;
	uint32_t lfunc = (uint32_t)func;
	/* 31    : Enable Bit
	   24-30 : Reserved
	   16-23 : Bus Number
	   11-15 : Slot Number = Device Number
	   08-10 : Function Number
	   02-07 : Register Number
	   00-01 : 00
	*/

	/* create configuration address as per Figure 1 */
	addr = (uint32_t)((lbus << 16) | (lslot << 11) |
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

uint16_t
pci_conf_readw(uint16_t bus, uint16_t slot,
		   uint16_t func, uint16_t offset)
{
	pci_conf_apply_addr(bus, func, slot, offset);
	/* read word in the data */
	return (uint16_t)((inl(pci_conf1_iodata)
		>> ((offset & 2) * 8)) & 0xffff);
}

static uint32_t
pci_conf_read(pci_pdata_t pd, uint32_t offset) {
	pci_conf_apply_addr(pd->busno, pd->func, pd->slot, offset);
	return inl(pci_conf1_iodata);
}

void
pci_conf_write(pci_pdata_t pd, uint32_t offset, uint32_t data)
{
	pci_conf_apply_addr(pd->busno, pd->func, pd->slot, offset);
	outl(pci_conf1_iodata, data);
}


struct pci_driver pci_attach_vendor[] = {
	{ E100_VENDOR, E100_DEVICE, &e100_attach },
	{ 0, 0, 0 },
};


#define MAX_SLOT 32

static int dev_count = 0;
static int pci_attach_match(uint32_t key1, uint32_t key2,
		 struct pci_driver *list, pci_pdata_t pd);

#define PCI_INTERRUPT_R 0x3c
#define PCI_HDRTYPE_MULTIFN_MASK 0x80
#define PCI_HDRTYPE_TYPE_MASK    0x7F

static int pci_scan_bus(pci_pdata_t dev)
{
	int busno = 0, slot = 0;

	uint16_t vendor;
	uint16_t tmp;
	for ( ; slot < MAX_SLOT ; slot++) {
		/* vendors that == 0xFFFF, it must be a non-existent device. */
		if ((vendor = pci_conf_readw(busno,slot,0,0)) != 0xFFFF) {
			dev->busno = busno;
			dev->slot = slot;
			dev->func = 0;

			dev->vendor = vendor;
			dev->device = pci_conf_readw(busno,slot,0,2);
			dev->class_code= pci_conf_readw(busno,slot,0,0xA);
			tmp = pci_conf_readw(busno,slot,0,0x8);
			dev->progif= tmp >> 8;
			dev->revid = tmp & 0xFF;
			tmp = pci_conf_readl(busno,slot,0,0xc);
			dev->hdrtype = (tmp >> 16) & 0xFF;
			{
				uint32_t intr = pci_conf_readl(
						busno, slot, 0, PCI_INTERRUPT_R);
				dev->irq_line = intr & 0xFF;
			}
			/* check attach */
			pci_attach_match(dev->class_code >> 8,
                        	 dev->class_code & 0xff,
                                 &pci_attach_class[0], dev);
                	pci_attach_match(dev->vendor,
                                 dev->device,
                                 &pci_attach_vendor[0], dev);
			dev_count++;
			dev++;
		}
	}
	return 0;
}

int pci_attach_match(uint32_t key1, uint32_t key2,
		 struct pci_driver *list, pci_pdata_t pd)
{
	uint32_t i;

	for (i = 0; list[i].attach_dev; i++) {
		if (list[i].vendor == key1 && list[i].devid == key2) {
			int r = list[i].attach_dev(pd);
			if (r > 0)
				return r;
			if (r < 0)
				printf("pci_attach_match: attaching "
					"%x.%x (%p): e\n",
					key1, key2, list[i].attach_dev, r);
		}
	}
	return 0;
}

static const char *pci_subclass_network_string[] = {
	"Ethernet controller", /* 0 */
};

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
	int subclass = classcode & 0xFF;
	classcode = classcode >> 8;
	if (classcode > 6) return pci_class_string[0];
	if (classcode == PCI_CLASS_NETWORK
	    && subclass == PCI_SUBCLASS_NETWORK_ETHERNET) {
		return pci_subclass_network_string[0];
	}
	return pci_class_string[classcode];
}

static struct pci_data_st pci_data[MAX_SLOT];


static int _lspci(pci_pdata_t dev)
{
	int i = 0;
	for (;i < dev_count; i++) {
		printf("%02x:%02x.%d vendor:%04x,devid:%04x,class:%04xh(%s)",
		   dev->busno, dev->slot, dev->func,
		   dev->vendor,dev->device,
		   dev->class_code, get_pci_class_string(dev->class_code)
		   );
		if (dev->revid > 0) {
		   printf(",r%x", dev->revid);
		}
		printf(",t%d", dev->hdrtype);
		printf(",irq%d\n", dev->irq_line);
		dev++;
	}
	return dev_count;
}

int lspci(void)
{
	return _lspci(pci_data);
}

/* for debug purpose which booting */
static int pci_show_addrs = 0;

void
pci_func_enable(pci_pdata_t f)
{
	pci_conf_write(f, PCI_COMMAND_STATUS_REG,
		       PCI_COMMAND_IO_ENABLE |
		       PCI_COMMAND_MEM_ENABLE |
		       PCI_COMMAND_MASTER_ENABLE);

	uint32_t bar_width;
	uint32_t bar;
	for (bar = PCI_MAPREG_START; bar < PCI_MAPREG_END;
	     bar += bar_width)
	{
		uint32_t oldv = pci_conf_read(f, bar);

		bar_width = 4;
		pci_conf_write(f, bar, 0xffffffff);
		uint32_t rv = pci_conf_read(f, bar);

		if (rv == 0)
			continue;

		int regnum = PCI_MAPREG_NUM(bar);
		uint32_t base, size;
		if (PCI_MAPREG_TYPE(rv) == PCI_MAPREG_TYPE_MEM) {
			if (PCI_MAPREG_MEM_TYPE(rv) == PCI_MAPREG_MEM_TYPE_64BIT)
				bar_width = 8;

			size = PCI_MAPREG_MEM_SIZE(rv);
			base = PCI_MAPREG_MEM_ADDR(oldv);
			if (pci_show_addrs)
				show_reg_info(regnum, size, base);
		} else {
			size = PCI_MAPREG_IO_SIZE(rv);
			base = PCI_MAPREG_IO_ADDR(oldv);
			if (pci_show_addrs)
				show_reg_info(regnum, size, base);
		}

		pci_conf_write(f, bar, oldv);
		f->addr_base[regnum] = base;
		f->addr_size[regnum] = size;

		if (size && !base)
			printf("PCI device %02x:%02x.%d (%04x:%04x) "
				"may be misconfigured: "
				"region %d: base 0x%x, size %d\n",
				f->busno, f->slot, f->func,
				f->vendor, f->device,
				regnum, base, size);
	}
}

int pci_init(void)
{
	memset(&pci_data, 0, sizeof(pci_data));
	return pci_scan_bus(pci_data);
}

static int
pci_bridge_attach(pci_pdata_t pcif) {

/* compatable issue on line, disable this part code in advance */
#if 0
	uint32_t ioreg  = pci_conf_read(pcif, PCI_BRIDGE_STATIO_REG);
	uint32_t busreg = pci_conf_read(pcif, PCI_BRIDGE_BUS_REG);

	if (PCI_BRIDGE_IO_32BITS(ioreg)) {
		printf("PCI: %02x:%02x.%d: 32-bit bridge IO not supported.\n",
			pcif->busno, pcif->slot, pcif->func);
		return 0;
	}

	struct pci_bus nbus;
	memset(&nbus, 0, sizeof(nbus));
	nbus.parent_bridge = pcif;
	nbus.busno = (busreg >> PCI_BRIDGE_BUS_SECONDARY_SHIFT) & 0xff;

	// if (pci_show_devs)
		printf("PCI: %02x:%02x.%d: bridge to PCI bus %d--%d\n",
			pcif->busno, pcif->slot, pcif->func,
			nbus.busno,
			(busreg >> PCI_BRIDGE_BUS_SUBORDINATE_SHIFT) & 0xff);

	// pci_scan_bus(&nbus);
#endif
	return 1;
}
