#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>

#include <unicode/uidna.h>
#include <unicode/utf8.h>
#include <unicode/uscript.h>
#include <unicode/uspoof.h>

#include "i-dunno.h"
#include "takebits.h"
/* #include "cpprops.h" */

static const char *form_rec(take_bits_t take_bits,
			    const void *addr, int addr_size,
			    char *dst, socklen_t dst_size,
			    int addr_idx, int dst_idx, int flags);
static const char *form_inet(const void *src,
			     char *dst, socklen_t size,
			     int flags);
static const char *form_inet6(const void *src,
			      char *dst, socklen_t size,
			      int flags);
static int confusing_enough(const char *dst, int dst_size, int flags);

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

#define is_unassigned(cp)	(cp == 0 || u_charType(cp) == U_UNASSIGNED)
				/* NUL *is* assigned, but we don't want it */

static const char *form_rec(take_bits_t take_bits,
			    const void *addr, int addr_size,
			    char *dst, socklen_t dst_size,
			    int addr_idx, int dst_idx, int flags)
{
	int cp;
	UBool error = FALSE;

	int strides[] = { 7, 11, 16, 21 };
	for (int i = 0; i < 4; i++) {
		int stride = strides[i];
		int utf8len = i + 1;
		if (addr_idx + stride > addr_size) {
			/* TODO: allow with extra padding */
			return NULL;
		}

		TAKE_BITS(take_bits, cp, addr, addr_idx, stride);
		if (is_unassigned(cp) ||  U8_LENGTH(cp) != utf8len) {
			/* Don't include unassigned codepoints.
			 * Skip codepoints that would be represented with
			 * fewer bits than expected in the resulting string */
			UNTAKE_BITS(addr, addr_idx, stride);
			continue;
		}

		U8_APPEND(dst, dst_idx, dst_size, cp, error);

		if (error)
			return NULL;

		if (addr_idx == addr_size) {
			U8_APPEND(dst, dst_idx, dst_size, '\0', error);
			if (error)
				return NULL;
			if (confusing_enough(dst, dst_size, flags))
				return dst;
			else
				/* 'unput' the \0 and go on to unput the
				 * last char */
				U8_BACK_1((unsigned char *) dst, 0, dst_idx);
		}
		else if (form_rec(take_bits, addr, addr_size, dst, dst_size,
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
	return form_rec(take_bits_inet, src, 32, dst, size, 0, 0, flags);
}

static const char *form_inet6(const void *src,
			      char *dst, socklen_t size,
			      int flags)
{
	return form_rec(take_bits_inet6, src, 128, dst, size, 0, 0, flags);
}

#define is_nonascii(cp)		(! U8_IS_SINGLE(cp))
#define is_disallowed_idna(cp)	1
#define is_nonprint(cp)		(! u_isprint(cp))
#define is_symbol(cp)		(U_GET_GC_MASK(cp) & U_GC_S_MASK)
#define is_emoji(cp)		(u_hasBinaryProperty(cp, UCHAR_EMOJI))
#define u_getScript(cp)		((UScriptCode) u_getIntPropertyValue(cp, UCHAR_SCRIPT))

static int confusing_enough(const char *str, int str_size, int flags)
{
	int confusion_level = 0;
	int req_confusion = flags & I_DUNNO_CONFUSION_MASK;
	int i = 0;
	int cp;

	UCharDirection baseDirection = U_CHAR_DIRECTION_COUNT;
	UScriptCode baseScript = USCRIPT_INVALID_CODE;

	while (i < str_size) {
		U8_NEXT(str, i, str_size, cp);
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
	}

	confusion_level |= I_DUNNO_HAS_CONFUSABLE;

	if (req_confusion & I_DUNNO_HAS_DISSALLOWED_IDNA) {
		UErrorCode err = U_ZERO_ERROR;
		UIDNA *idna = uidna_openUTS46(UIDNA_DEFAULT, &err);
		UIDNAInfo info = UIDNA_INFO_INITIALIZER;
		char buf[255];
		uidna_labelToASCII_UTF8(idna, str, str_size, buf, 255,
					&info, &err);
		if (info.errors & UIDNA_ERROR_DISALLOWED)
			confusion_level |= I_DUNNO_HAS_DISSALLOWED_IDNA;
		uidna_close(idna);
	}


	return (confusion_level & req_confusion) == req_confusion;
}
