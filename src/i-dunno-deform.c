#include <config.h>
#include <i-dunno.h>
#include "pushbits.h"
#include "unicodehelpers.h"

#include <netinet/in.h>
#include <assert.h>
#include <errno.h>

#include <unicode/utf8.h>

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
	dst->s_addr = INADDR_ANY;
	return deform_generic(push_bits_inet, src, dst, 32);
}

static int deform_inet6(const char *src, struct in6_addr *dst)
{
	*dst = in6addr_any;
	return deform_generic(push_bits_inet6, src, dst, 128);
}

static int deform_generic(push_bits_t push_bits, const char *src, void *dst, int size)
{
	int src_idx = 0;

	for (int dst_idx = 0; dst_idx < size; ) {
		if (dst_idx >= size)
			return 0;

		/* Take one codepoint from the I-DUNNO form */
		int cp;
		U8_NEXT(src, src_idx, I_DUNNO_ADDRSTRLEN, cp);
		if (cp < 0)
			/* invalid UTF-8 */
			return 0;
		if (cp == 0)
			/* short string */
			return 0;

		int nb_bits = nb_bits_in_cp(cp);
		if (! nb_bits)
			/* invalid codepoint */
			return 0;

		/* And push that codepoint onto the address */
		PUSH_BITS(push_bits, dst, dst_idx, cp, nb_bits);
	}
	return 1;
}
