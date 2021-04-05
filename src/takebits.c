#include <config.h>
#include "takebits.h"

#include <assert.h>
#include <netinet/in.h>
#include <stdlib.h>

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

static uint32_t take_bits32(const uint32_t *addr, int offset,
			    int nb_bits, int max_size)
{
	uint32_t retval = 0;
	assert (offset <= max_size);
	assert (nb_bits < TAKE_BITS_MAX_BITS);

	addr += (offset / 32);
	max_size -= 32 * (offset / 32);
	offset %= 32;

	if (offset + nb_bits <= 32) {
		uint32_t mask = 0xffffffff >> (32 - nb_bits);
		retval = (ntohl(*addr) >> (32 - nb_bits - offset)) & mask;
	} else {
		int nb_highbits = 32 - offset;
		uint32_t highmask = (1 << nb_highbits) - 1;
		uint32_t high = ntohl(*addr) & highmask;

		int nb_lowbits = offset + nb_bits - 32;
		uint32_t low = 0;
		if (offset + nb_bits <= max_size) {
			low = ntohl(*(addr + 1)) >> (32 - nb_lowbits);
		} else {
			low = random() & ((1 << nb_lowbits) - 1);
			retval |= TAKE_BITS_ADDED_PADDING;
		}
		retval |= (high << nb_lowbits) + low;
	}

	if (offset + nb_bits >= max_size)
		retval |= TAKE_BITS_REACHED_END;

	return retval;
}


#ifdef FALLBACK_S6_ADDR_FOR_STRUCT_IN6_ADDR

static uint32_t take_bits8(const uint8_t *addr, int offset,
			   int nb_bits, int max_size)
{
	assert (offset <= max_size);
	assert (nb_bits < TAKE_BITS_MAX_BITS);

	int retval = 0;

	addr += (offset / 8);
	max_size -= (offset - (offset % 8));
	offset %= 8;

	if (offset) {
		/* We always take at least 7 bits at a time. Don't bother
		 * handling the case where we only get the middle of the
		 * first octet. */
		assert (offset + nb_bits >= 8);

		uint8_t mask = (1 << (8 - offset)) - 1;
		retval +=  *addr & mask;
		addr++;
		max_size -= 8;
		nb_bits -= 8 - offset;
	}

	while (nb_bits >= 8 && max_size >= 8) {
		retval <<= 8;
		retval += *addr;
		addr++;
		max_size -= 8;
		nb_bits -= 8;
	}

	if (nb_bits) {
		retval <<= nb_bits;
		if (max_size) {
			/* we have enough bits to finish reading into retval */
			retval += *addr >> (8 - nb_bits);
		} else {
			/* not enough, add padding */
			retval += random() & ((1 << nb_bits) - 1);
			retval |= TAKE_BITS_ADDED_PADDING;
		}
	}

	if (max_size == 0)
		retval |= TAKE_BITS_REACHED_END;

	return retval;
}

#endif

uint32_t take_bits_inet(const void *src, int offset, int nb_bits)
{
	const struct in_addr *addr = src;
	const uint32_t *addr32 = &addr->s_addr;

	return take_bits32(addr32, offset, nb_bits, 32);
}

uint32_t take_bits_inet6(const void *src, int offset, int nb_bits)
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
