#ifndef TAKEBITS_H
#define TAKEBITS_H

/* 
 * 11000110 00110011 01100100 10100100
 * 
 *
 * 11000110 00110011 01100100 10100100 || 10001010 01100010 01100101 11011001
 *    ^_____________________^
 * <->    nb_bits = 21.       <------>
 * offset = 3                  32 - nb_bits - offset
 * 
 * <---------------->^________________________^
 *    offset = 16     nb_bits = 21
 *                    nb_highbits = 16 || nb_lowbits = 5
 *
 */

static int take_bits(const uint32_t *addr, int offset,
		     int nb_bits, int max_size)
{
	/*
	 * TODO: in that case return the last few bits of addr plus random
	 * padding
	 */
	assert (offset + nb_bits <= max_size);

	for (; offset > 32; offset -= 32)
		addr++;
	if (offset + nb_bits <= 32) {
		uint32_t mask = 0xffffffff >> (32 - nb_bits);
		return (ntohl(*addr) >> (32 - nb_bits - offset)) & mask;
	} else {
		int nb_highbits = 32 - offset;
		int nb_lowbits = offset + nb_bits - 32;
		uint32_t highmask = (1 << nb_highbits) - 1;
		uint32_t high = ntohl(*addr) & highmask;
		uint32_t low = ntohl(*(addr + 1)) >> (32 - nb_lowbits);
		return (high << nb_lowbits) + low;
	}
}

#define TAKE_BITS(cp, addr, offset, nb_bits, max_size)			\
	do {								\
		cp = take_bits(addr, offset, nb_bits, max_size);	\
		offset += nb_bits;					\
	} while (0)

#define UNTAKE_BITS(addr, offset, nb_bits) \
	do {				   \
		offset -= nb_bits;	   \
	} while (0)

#endif
