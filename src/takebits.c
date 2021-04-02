#include <config.h>

#include <netinet/in.h>
#include <assert.h>

#include "takebits.h"

/*
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

static int take_bits32(const uint32_t *addr, int offset,
		       int nb_bits, int max_size)
{
	/*
	 * TODO: in that case return the last few bits of addr plus random
	 * padding
	 */
	assert (offset + nb_bits <= max_size);

	addr = addr + (offset / 32);
	offset = offset % 32;

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


#ifdef FALLBACK_S6_ADDR_FOR_STRUCT_IN6_ADDR

static int take_bits8(const uint8_t *addr, int offset,
		      int nb_bits, int max_size)
{
	/*
	 * TODO: in that case return the last few bits of addr plus random
	 * padding
	 */
	assert (offset + nb_bits <= max_size);

	addr += (offset / 8);
	offset %= 8;

	int retval = 0;

	if (offset) {
		assert (offset + nb_bits >= 8);
		uint8_t mask = (1 << (8 - offset)) - 1;
		retval +=  *addr & mask;
		addr++;
		nb_bits -= 8 - offset;
	}

	while (nb_bits >= 8) {
		retval <<= 8;
		retval += *addr;
		addr++;
		nb_bits -= 8;
	}

	if (nb_bits) {
		retval <<= nb_bits;
		retval += *addr >> (8 - nb_bits);
	}

	return retval;
}

#endif

int take_bits_inet(const void *src, int offset, int nb_bits)
{
	const struct in_addr *addr = src;
	const uint32_t *addr32 = &addr->s_addr;

	return take_bits32(addr32, offset, nb_bits, 32);
}

int take_bits_inet6(const void *src, int offset, int nb_bits)
{
	const struct in6_addr *addr = src;

#ifdef FALLBACK_S6_ADDR_FOR_STRUCT_IN6_ADDR
	const uint8_t *addr8 = &addr->s6_addr[0];
	return take_bits8(addr8, offset, nb_bits, 128);
#else
	const uint32_t *addr32 = &addr->s6_addr32[0];
	return take_bits32(addr32, offset, nb_bits, 128);
#endif
}
