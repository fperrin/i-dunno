#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <arpa/inet.h>

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
	assert_equal(take_bits(&addr.s_addr, 0, 21, 32),
		      0b110001100011001101100);
	assert_equal(take_bits(&addr.s_addr, 0, 7, 32),
		     0b1100011);
	assert_equal(take_bits(&addr.s_addr, 7, 7, 32),
		     0b0001100);
	assert_equal(take_bits(&addr.s_addr, 14, 7, 32),
		     0b1101100);
	assert_equal(take_bits(&addr.s_addr, 21, 11, 32),
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

	assert_equal(take_bits(&addr6.s6_addr32[0], 0, 32, 128),
		     0b00100000000000010000110110111000);
	assert_equal(take_bits(&addr6.s6_addr32[0], 0, 21, 128),
		     0b001000000000000100001);
	assert_equal(take_bits(&addr6.s6_addr32[0], 24, 16, 128),
		     0b1011100010100011);
	assert_equal(take_bits(&addr6.s6_addr32[0], 29, 11, 128),
		     0b00010100011);
	assert_equal(take_bits(&addr6.s6_addr32[0], 90, 13, 128),
		     0b0000000000111);
	assert_equal(take_bits(&addr6.s6_addr32[0], 105, 23, 128),
		     0b10111100101000110111001);

	struct in6_addr unpush_addr6 = { 0 };
	int offset = 0;
	PUSH_BITS(&unpush_addr6.s6_addr32[0], offset, 0b001000000000000100001, 21);
	PUSH_BITS(&unpush_addr6.s6_addr32[0], offset, 0b1011011100010100, 16);
	PUSH_BITS(&unpush_addr6.s6_addr32[0], offset, 0b01110111000, 11);
	PUSH_BITS(&unpush_addr6.s6_addr32[0], offset, 0b0110010, 7);
	PUSH_BITS(&unpush_addr6.s6_addr32[0], offset, 0b01010010011, 11);
	PUSH_BITS(&unpush_addr6.s6_addr32[0], offset, 0b00011000110011, 14);
	PUSH_BITS(&unpush_addr6.s6_addr32[0], offset, 0b0000000, 7);
	PUSH_BITS(&unpush_addr6.s6_addr32[0], offset, 0b000000000000011101101, 21);
	PUSH_BITS(&unpush_addr6.s6_addr32[0], offset, 0b11100101000110111001, 20);
	for (int i = 0; i < 4; i++)
		assert_equal(addr6.s6_addr32[i], unpush_addr6.s6_addr32[i]);
	return 0;
}
