#include <assert.h>
#include <err.h>
#include <stdio.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include "takebits.h"
#include "pushbits.h"

void assert_equal(uint32_t actual, uint32_t expected)
{
	if (actual == expected)
		return;
	errx(1, "actual: %x, expected: %x\n", actual, expected);
}

int main(int argc, char **argv)
{
	struct in_addr addr;
	assert (inet_pton(AF_INET, "198.51.100.164", &addr));
	/* 198.51.100.164 -> 11000110 00110011 01100100 10100100 */
	assert (take_bits_inet(&addr, 0, 21) == \
		0b110001100011001101100);
	assert (take_bits_inet(&addr, 0, 7) == \
		0b1100011);
	assert (take_bits_inet(&addr, 7, 7) == \
		0b0001100);
	assert (take_bits_inet(&addr, 14, 7) == \
		0b1101100);
	assert (take_bits_inet(&addr, 21, 11) == \
		0b10010100100);

	struct in6_addr addr6;
	assert (inet_pton(AF_INET6,
			  "2001:db8:a3b8:64a4:c633::ede:51b9", &addr6));
	/*
	 * 2001 = 00100000 00000001
	 * 0db8 = 00001101 10111000    32
	 * a3b8 = 10100011 10111000
	 * 64a4 = 01100100 10100100    64
	 * c633 = 11000110 00110011
	 * 0000 = 00000000 00000000    96
	 * 0ede = 00001110 11011110
	 * 51b9 = 01010001 10111001    128
	 */

	assert (take_bits_inet6(&addr6, 0, 32) ==	\
		     0b00100000000000010000110110111000);
	assert (take_bits_inet6(&addr6, 0, 21) == \
		     0b001000000000000100001);
	assert (take_bits_inet6(&addr6, 24, 16) == \
		     0b1011100010100011);
	assert (take_bits_inet6(&addr6, 29, 11) == \
		     0b00010100011);
	assert (take_bits_inet6(&addr6, 90, 13) == \
		     0b0000000000111);
	assert (take_bits_inet6(&addr6, 105, 23) == \
		     0b10111100101000110111001);

	struct in6_addr unpush_addr6 = { 0 };
	int offset = 0;
	PUSH_BITS(push_bits_inet6, &unpush_addr6,
		  offset, 0b001000000000000100001, 21);
	PUSH_BITS(push_bits_inet6, &unpush_addr6, offset,
		  0b1011011100010100, 16);
	PUSH_BITS(push_bits_inet6, &unpush_addr6,
		  offset, 0b01110111000, 11);
	PUSH_BITS(push_bits_inet6, &unpush_addr6,
		  offset, 0b0110010, 7);
	PUSH_BITS(push_bits_inet6, &unpush_addr6,
		  offset, 0b01010010011, 11);
	PUSH_BITS(push_bits_inet6, &unpush_addr6, offset, 0b00011000110011, 14);
	PUSH_BITS(push_bits_inet6, &unpush_addr6, offset, 0b0000000, 7);
	PUSH_BITS(push_bits_inet6, &unpush_addr6,
		  offset, 0b000000000000011101101, 21);
	PUSH_BITS(push_bits_inet6, &unpush_addr6,
		  offset, 0b11100101000110111001, 20);
	assert (offset == 128);

	assert (IN6_ARE_ADDR_EQUAL(&addr6, &unpush_addr6));
	return 0;
}
