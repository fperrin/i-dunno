#ifndef TAKEBITS_H
#define TAKEBITS_H

#include <stdint.h>

uint32_t take_bits_inet(const void *addr, int offset, int nb_bits);
uint32_t take_bits_inet6(const void *addr, int offset, int nb_bits);

typedef uint32_t (*take_bits_t)(const void *, int, int);

/* Reached the end of the address, no more to do */
#define TAKE_BITS_REACHED_END		(1 << 31)
/* Reached (and went past) the end of the address, the low order bits are
 * random */
#define TAKE_BITS_ADDED_PADDING		(1 << 30)

#define TAKE_BITS_MAX_BITS		29
#define TAKE_BITS_VALUE_MASK		((1 << 30) - 1)

#define TAKE_BITS(take_bits, cp, addr, offset, nb_bits)			\
	do {								\
		cp = take_bits(addr, offset, nb_bits);			\
		offset += nb_bits;					\
	} while (0)

#define UNTAKE_BITS(addr, offset, nb_bits) \
	do {				   \
		offset -= nb_bits;	   \
	} while (0)

#endif
