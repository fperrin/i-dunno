#ifndef I_DUNNO_H
#define I_DUNNO_H

#include <sys/socket.h>

const char *i_dunno_form(int af, const void *src,
			 char *dst, socklen_t size,
			 int flags);

int i_dunno_deform(int af, const char *src, void *dst);

/* See testsuite/idunno_addrstrlen.c for how this was determined */
#define I_DUNNO_ADDRSTRLEN			29

/* Confusion levels */
#define I_DUNNO_HAS_NON_ASCII			(1 << 0)
#define I_DUNNO_HAS_DISALLOWED_IDNA		(1 << 1)
#define I_DUNNO_HAS_NON_PRINTABLE		(1 << 2)
#define I_DUNNO_HAS_SEVERAL_SCRIPTS		(1 << 3)
#define I_DUNNO_HAS_SYMBOL			(1 << 4)
#define I_DUNNO_HAS_SEVERAL_DIRECTIONS		(1 << 5)
#define I_DUNNO_HAS_CONFUSABLE			(1 << 6)
#define I_DUNNO_HAS_EMOJI			(1 << 7)

/* Other flags for i_dunno_form */
#define I_DUNNO_NO_PADDING			(1 << 8)
#define I_DUNNO_RANDOMIZE			(1 << 9)


#define I_DUNNO_MINIMUM_CONFUSION		\
	(I_DUNNO_HAS_NON_ASCII |		\
	 I_DUNNO_HAS_DISALLOWED_IDNA)

#define I_DUNNO_SATISFACTORY_CONFUSION		\
	(I_DUNNO_MINIMUM_CONFUSION |		\
	 I_DUNNO_HAS_NON_PRINTABLE |		\
	 I_DUNNO_HAS_SEVERAL_SCRIPTS |		\
	 I_DUNNO_HAS_SYMBOL)

#define I_DUNNO_DELIGHTFUL_CONFUSION		\
	(I_DUNNO_SATISFACTORY_CONFUSION |	\
	 I_DUNNO_HAS_SEVERAL_DIRECTIONS |	\
 	 I_DUNNO_HAS_CONFUSABLE |		\
	 I_DUNNO_HAS_EMOJI)

#define I_DUNNO_CONFUSION_MASK I_DUNNO_DELIGHTFUL_CONFUSION

#endif
