#ifndef PUSHBITS_H
#define PUSHBITS_H

#include <assert.h>

static void push_bits(uint32_t *dst, int offset, int value, int nb_bits)
{
	assert (nb_bits < 32);
	for (; offset > 32; offset -= 32)
		dst++;
	if (offset + nb_bits <= 32) {
		uint32_t bits = htonl(value << (32 - nb_bits - offset));
		*dst = *dst | bits;
		return;
	}

	/* int nb_highbits = 32 - offset; */
	int nb_lowbits = offset + nb_bits - 32;
	uint32_t high = htonl(value >> nb_lowbits);
	uint32_t lowmask = (1 << nb_lowbits) - 1;
	uint32_t low = htonl((value & lowmask) << (32 - nb_lowbits));
	*dst = *dst | high;
	*(dst + 1) = *(dst + 1) | low;
}

#define PUSH_BITS(dst, offset, value, nb_bits)		\
	do {						\
		push_bits(dst, offset, value, nb_bits); \
		offset += nb_bits;			\
	} while(0)

#endif
