#include <config.h>
#include <i-dunno.h>
#include "takebits.h"
#include "unicodehelpers.h"

#include <netinet/in.h>
#include <assert.h>
#include <errno.h>
#include <stdbool.h>

#include <unicode/uidna.h>
#include <unicode/utf8.h>
#include <unicode/uspoof.h>

static const char *form_rec(take_bits_t take_bits,
			    const void *addr, char *dst, socklen_t dst_size,
			    int addr_idx, int dst_idx, int flags);
static const char *form_inet(const void *src,
			     char *dst, socklen_t size,
			     int flags);
static const char *form_inet6(const void *src,
			      char *dst, socklen_t size,
			      int flags);
static int confusing_enough(const char *dst, int flags);

const char *i_dunno_form(int af, const void *src,
			 char *dst, socklen_t size,
			 int flags)
{
	flags |= I_DUNNO_MINIMUM_CONFUSION;
	switch (af) {
	case AF_INET:
		return form_inet(src, dst, size, flags);
	case AF_INET6:
		return form_inet6(src, dst, size, flags);
	default:
		errno = EAFNOSUPPORT;
		return NULL;
	}
}


static const char *form_rec(take_bits_t take_bits,
			    const void *addr, char *dst, socklen_t dst_size,
			    int addr_idx, int dst_idx, int flags)
{
	for (int i = 0; i < 4; i++) {
		int stride = utf8strides[i];
		uint32_t taken_bits;
		int cp;
		UBool error = false;

		/* Take one codepoint from the address */
		TAKE_BITS(take_bits, taken_bits, addr, addr_idx, stride);
		cp = taken_bits & TAKE_BITS_VALUE_MASK;

		if ((flags & I_DUNNO_NO_PADDING) &&
		    (taken_bits & TAKE_BITS_ADDED_PADDING)) {
			/* If caller asked for no padding but we added some,
			 * skip that stride */
			UNTAKE_BITS(addr, addr_idx, stride);
			continue;
		}

		if (is_unassigned(cp) || nb_bits_in_cp(cp) != stride) {
			/* Don't include unassigned codepoints.
			 * Skip codepoints that would be represented with
			 * fewer bits than expected in the resulting string */
			UNTAKE_BITS(addr, addr_idx, stride);
			continue;
		}

		/* Append that codepoint to the destination form */
		U8_APPEND(dst, dst_idx, dst_size, cp, error);

		if (error)
			return NULL;

		/* If we've reached the end of the source address, see whether
		 * the form is confusing enough. */
		if (taken_bits & TAKE_BITS_REACHED_END) {
			U8_APPEND(dst, dst_idx, dst_size, '\0', error);
			if (error)
				return NULL;
			if (confusing_enough(dst, flags))
				return dst;
			else
				/* 'unput' the \0 and go on to unput the
				 * last char */
				U8_BACK_1((unsigned char *) dst, 0, dst_idx);
		}
		/* If we haven't reached the end, keep trying */
		else if (form_rec(take_bits, addr, dst, dst_size,
				  addr_idx, dst_idx, flags)) {
			return dst;
		}

		/* else unput the last char, and try with a different stride */
		U8_BACK_1((unsigned char*) dst, 0, dst_idx);
		UNTAKE_BITS(addr, addr_idx, stride);
	}

	return NULL;
}

static const char *form_inet(const void *src,
			     char *dst, socklen_t size,
			     int flags)
{
	return form_rec(take_bits_inet, src, dst, size, 0, 0, flags);
}

static const char *form_inet6(const void *src,
			      char *dst, socklen_t size,
			      int flags)
{
	return form_rec(take_bits_inet6, src, dst, size, 0, 0, flags);
}

static int confusing_enough(const char *str, int flags)
{
	int confusion_level = 0;
	int req_confusion = flags & I_DUNNO_CONFUSION_MASK;
	int i = 0;
	int cp;

	UCharDirection baseDirection = U_CHAR_DIRECTION_COUNT;
	UScriptCode baseScript = USCRIPT_INVALID_CODE;

	do {
		U8_NEXT(str, i, -1, cp);
		if (is_nonascii(cp))
			confusion_level |= I_DUNNO_HAS_NON_ASCII;
		if (is_nonprint(cp))
			confusion_level |= I_DUNNO_HAS_NON_PRINTABLE;
		if (is_symbol(cp))
			confusion_level |= I_DUNNO_HAS_SYMBOL;
		if (is_emoji(cp))
			confusion_level |= I_DUNNO_HAS_EMOJI;

		if (baseDirection == U_CHAR_DIRECTION_COUNT)
			baseDirection = u_charDirection(cp);
		else if (u_charDirection(cp) != baseDirection)
			confusion_level |= I_DUNNO_HAS_SEVERAL_DIRECTIONS;

		if (baseScript == USCRIPT_INVALID_CODE)
			baseScript = u_getScript(cp);
		else if (u_getScript(cp) != baseScript)
			confusion_level |= I_DUNNO_HAS_SEVERAL_SCRIPTS;
	} while (cp != 0);

	confusion_level |= I_DUNNO_HAS_CONFUSABLE;

	if (req_confusion & I_DUNNO_HAS_DISALLOWED_IDNA) {
		UErrorCode err = U_ZERO_ERROR;
		UIDNA *idna = uidna_openUTS46(UIDNA_USE_STD3_RULES, &err);
		UIDNAInfo info = UIDNA_INFO_INITIALIZER;
		char buf[255];
		uidna_labelToASCII_UTF8(idna, str, -1, buf, 255, &info, &err);
		if (info.errors & UIDNA_ERROR_DISALLOWED)
			confusion_level |= I_DUNNO_HAS_DISALLOWED_IDNA;
		uidna_close(idna);
	}


	return (confusion_level & req_confusion) == req_confusion;
}
