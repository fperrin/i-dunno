#ifndef UNICODEHELPERS_H
#define UNICODEHELPERS_H

#include <unicode/uscript.h>
#include <unicode/utf8.h>

/* Helpers to get properties of Unicode codpoints */

#define is_unassigned(cp)	(cp == 0 || u_charType(cp) == U_UNASSIGNED)
				/* NUL *is* assigned, but we don't want it */
#define is_nonascii(cp)		(! U8_IS_SINGLE(cp))
#define is_nonprint(cp)		(! u_isprint(cp))
#define is_symbol(cp)		(U_GET_GC_MASK(cp) & U_GC_S_MASK)
#define is_emoji(cp)		(u_hasBinaryProperty(cp, UCHAR_EMOJI))
#define u_getScript(cp)		((UScriptCode) u_getIntPropertyValue(cp, UCHAR_SCRIPT))

static const int utf8strides[] = { 7, 11, 16, 21 };

/* How many bits in this codepoint? */
#define nb_bits_in_cp(cp)	(utf8strides[U8_LENGTH(cp) - 1])

#endif
