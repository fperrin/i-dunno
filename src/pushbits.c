#include <config.h>
#include "pushbits.h"

#include <assert.h>
#include <netinet/in.h>

static void push_bits32(uint32_t *dst, int offset, int value, int nb_bits,
			int max_size)
{
	assert (nb_bits < 32);

	/* TODO: in that case, throw away the low bits (were random padding) */
	assert (offset + nb_bits <= max_size);

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

#ifdef FALLBACK_S6_ADDR_FOR_STRUCT_IN6_ADDR

static void push_bits8(uint8_t *dst, int offset, int value, int nb_bits,
		       int max_size)
{
	/* TODO: in that case, throw away the low bits (were random padding) */
	assert (offset + nb_bits <= max_size);

	assert ((value >> nb_bits) == 0);

	dst += offset / 8;
	offset %= 8;

	if (offset) {
		assert (nb_bits >= offset);
		*dst |= value >> (nb_bits - 8 + offset);
		dst++;
		nb_bits -= (8 - offset);
	}

	while (nb_bits >= 8) {
		*dst = value >> (nb_bits - 8);
		dst++;
		nb_bits -= 8;
	}

	if (nb_bits) {
		*dst = value << (8 - nb_bits);
	}
}

#endif

void push_bits_inet(void *dst, int offset, int value, int nb_bits)
{
	struct in_addr *addr = dst;
	push_bits32(&addr->s_addr, offset, value, nb_bits, 32);
}

void push_bits_inet6(void *dst, int offset, int value, int nb_bits)
{
	struct in6_addr *addr6 = dst;

#ifdef FALLBACK_S6_ADDR_FOR_STRUCT_IN6_ADDR
	push_bits8(&addr6->s6_addr[0], offset, value, nb_bits, 128);
#else
	push_bits32(&addr6->s6_addr32[0], offset, value, nb_bits, 128);
#endif
}
