#ifndef PUSHBITS_H
#define PUSHBITS_H

#include <stdint.h>

void push_bits_inet(void *dst, int offset, uint32_t value, int nb_bits);
void push_bits_inet6(void *dst, int offset, uint32_t value, int nb_bits);

typedef void (*push_bits_t)(void *, int, uint32_t, int);


#define PUSH_BITS(push_bits, dst, offset, value, nb_bits)	\
	do {							\
		push_bits(dst, offset, value, nb_bits);		\
		offset += nb_bits;				\
	} while(0)

#endif
