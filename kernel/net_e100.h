#ifndef NET_E100_H
#define NET_E100_H

#include "os_pci.h"

#define E100_VENDOR 0x8086
#define E100_DEVICE 0x100E
/* pci_attach_vendor matches the vendor ID and device ID of a PCI device */
int e100_attach(pci_pdata_t pd);

#endif /* NET_E100_H */
