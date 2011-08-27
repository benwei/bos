#ifndef PCI_H
#define PCI_H

extern int pci_init(void);
extern int lspci(void);
extern const char *
get_pci_class_string(unsigned short classcode);

#endif /* PCI_H */
