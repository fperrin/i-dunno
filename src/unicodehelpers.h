#ifndef UNICODEHELPERS_H
#define UNICODEHELPERS_H

#include <unicode/uchar.h>
#include <unicode/uscript.h>
#include <unicode/utf8.h>

/* Helpers to get properties of Unicode codpoints */

#define is_unassigned(cp)	(cp == 0 || u_charType(cp) == U_UNASSIGNED)
				/* NUL *is* assigned, but we don't want it */
#define is_nonascii(cp)		(! U8_IS_SINGLE(cp))
#define is_nonprint(cp)		(! u_isprint(cp))
#define is_symbol(cp)		(U_GET_GC_MASK(cp) & U_GC_S_MASK)
#define u_getScript(cp)		((UScriptCode) u_getIntPropertyValue(cp, UCHAR_SCRIPT))

/* The ASCII digits 0-9 have the Emoji property; reject these. But add those
 * with the extend
 * https://util.unicode.org/UnicodeJsps/list-unicodeset.jsp?a=%5B%3ABasic_Emoji%3DYes%3A%5D%5B%3AEmoji%3DYes%3A%5D%5B%3AExtended_Pictographic%3DYes%3A%5D&c=on&g=&i= */
#define is_emoji(cp)	\
	((u_hasBinaryProperty(cp, UCHAR_EMOJI) ||			\
	  u_hasBinaryProperty(cp, UCHAR_EXTENDED_PICTOGRAPHIC)) &&	\
	 cp > '9')

static const int utf8strides[] = { 7, 11, 16, 21 };
static const int utf8strides_max = 4;

/* How many bits in this codepoint? */
#define nb_bits_in_cp(cp)	(U8_LENGTH(cp)? utf8strides[U8_LENGTH(cp) - 1]: 0)

/* U8_APPEND only checks the capacity of the destination buffer when the
 * codepoint we append is non-ASCII */
#define U8_APPEND_SAFE(s, i, capacity, c, isError)	\
	do {						\
		if (i >= capacity) {			\
			isError = true;			\
			break;				\
		}					\
		U8_APPEND(s, i, capacity, c, isError);	\
	} while (0)
#endif
