#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#include "kern/pci.h"

#define E1000_VENDER_ID_82540EM 0x8086
#define E1000_DEV_ID_82540EM 0x100E

#define E1000_STATUS   0x00008  /* Device Status - RO */
#define E1000REG(offset) (void *)(bar_va + offset)

int e1000_attachfn(struct pci_func *pcif);


#endif  // SOL >= 6
