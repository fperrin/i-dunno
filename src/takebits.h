#ifndef TAKEBITS_H
#define TAKEBITS_H

int take_bits_inet(const void *addr, int offset, int nb_bits);
int take_bits_inet6(const void *addr, int offset, int nb_bits);

typedef int (*take_bits_t)(const void *, int, int);


#define TAKE_BITS(take_bits, cp, addr, offset, nb_bits)			\
	do {								\
		cp = take_bits(addr, offset, nb_bits);			\
		offset += nb_bits;					\
	} while (0)

#define UNTAKE_BITS(addr, offset, nb_bits) \
	do {				   \
		offset -= nb_bits;	   \
	} while (0)

#endif
