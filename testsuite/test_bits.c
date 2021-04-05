#include "takebits.h"
#include "pushbits.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <string.h>

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
		(TAKE_BITS_REACHED_END | 0b10010100100));

	uint32_t retval = take_bits_inet(&addr, 24, 11);
	assert (retval & TAKE_BITS_REACHED_END);
	assert (retval & TAKE_BITS_ADDED_PADDING);
	assert ((retval & TAKE_BITS_VALUE_MASK) >> 3 == 0b10100100);

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

	assert (take_bits_inet6(&addr6, 0, 28) == \
		     0b0010000000000001000011011011);
	assert (take_bits_inet6(&addr6, 0, 21) == \
		     0b001000000000000100001);
	assert (take_bits_inet6(&addr6, 24, 16) == \
		     0b1011100010100011);
	assert (take_bits_inet6(&addr6, 29, 11) == \
		     0b00010100011);
	assert (take_bits_inet6(&addr6, 90, 13) == \
		     0b0000000000111);
	assert (take_bits_inet6(&addr6, 105, 23) == \
		     (TAKE_BITS_REACHED_END | 0b10111100101000110111001));

	/* push on a blank address the same bits as the previous address  */
	struct in6_addr unpush_addr6 = IN6ADDR_ANY_INIT;
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

	/* push extra bits, making sure the bits past the end are discarded */
	struct in6_addr two_addrs[2];
	two_addrs[0] = in6addr_any;
	two_addrs[1] = in6addr_any;
	push_bits_inet6(&two_addrs[0], 116, 0xffff, 16);
	assert (two_addrs[0].s6_addr[14] == 0x0f);
	assert (two_addrs[0].s6_addr[15] == 0xff);
	assert (two_addrs[1].s6_addr[0] == 0x00);

	two_addrs[0] = in6addr_any;
	two_addrs[1] = in6addr_any;
	push_bits_inet6(&two_addrs[0], 120, 0x5555, 16);
	push_bits_inet6(&two_addrs[0], 128, 0xffff, 16);
	assert (two_addrs[0].s6_addr[15] == 0x55);
	assert (two_addrs[1].s6_addr[0] == 0x00);

	/* push to the very last octet */
	two_addrs[0] = in6addr_any;
	push_bits_inet6(&two_addrs[0], 125, 0x5a, 7);
	assert (two_addrs[0].s6_addr[15] == 0x5);

	return 0;
}
