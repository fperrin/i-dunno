#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <strings.h>

#include <unicode/utf8.h>

#include "i-dunno.h"
#include "pushbits.h"

static int deform_inet(const char *src, struct in_addr *dst);
static int deform_inet6(const char *src, struct in6_addr *dst);
static int deform_generic(push_bits_t push_bits, const char *src, void *dst, int size);

int i_dunno_deform(int af, const char *src, void *dst)
{
	switch (af) {
	case AF_INET:
		return deform_inet(src, dst);
	case AF_INET6:
		return deform_inet6(src, dst);
	default:
		errno = EAFNOSUPPORT;
		return -1;
	}
}

static int deform_inet(const char *src, struct in_addr *dst)
{
	return deform_generic(push_bits_inet, src, dst, 32);
}

static int deform_inet6(const char *src, struct in6_addr *dst)
{
	return deform_generic(push_bits_inet6, src, dst, 128);
}

static int deform_generic(push_bits_t push_bits, const char *src, void *dst, int size)
{
	int src_idx = 0;
	int strides[] = { 7, 11, 16, 21 };

	bzero(dst, size / 8);

	for (int dst_idx = 0; dst_idx < size; ) {
		int cp;
		int old_src_idx = src_idx;

		U8_NEXT(src, src_idx, I_DUNNO_ADDRSTRLEN, cp);
		if (cp < 0)
			/* invalid UTF-8 */
			return 0;
		if (cp == 0)
			/* short string */
			return 0;

		int nb_bits = strides[src_idx - old_src_idx - 1];
		/* TODO handle padding */
		assert (dst_idx + nb_bits <= size);

		PUSH_BITS(push_bits, dst, dst_idx, cp, nb_bits);
	}
	return 1;
}

