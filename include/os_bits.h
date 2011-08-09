#ifndef _OS_BITS_H
#define _OS_BITS_H

#define BIT_ZERO(s)   s=0
#define BIT_All(s)   s=~0
#define BIT_SET(s, b) s|=(1 << b)
#define BIT_USET(s, b) s &= ~(s & (1 << b))
#define BIT_ISSET(s, b) s & (1<<b)

#endif /* _OS_BITS_H */
