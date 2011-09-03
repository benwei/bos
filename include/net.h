#ifndef NET_H
#define NET_H

#include "types.h"

extern int show_nic(void);
extern int net_transmit (const char *data, uint16_t len);

#endif /* NET_H */
