#include <arpa/inet.h>
#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "i-dunno.h"
#include "testutils.h"

int do_one_roundtrip(void)
{
	struct in6_addr rand_addr;
	for (int i = 0; i < 4; i++)
		rand_addr.s6_addr32[i] = random();

	char dest[I_DUNNO_ADDRSTRLEN];
	if (! i_dunno_form(AF_INET6, &rand_addr, dest,
			   I_DUNNO_ADDRSTRLEN, I_DUNNO_DELIGHTFUL_CONFUSION))
		return 0;

	struct in6_addr returned_addr = { 0 };
	assert (i_dunno_deform(AF_INET6, dest, &returned_addr));

	if (memcmp(&rand_addr, &returned_addr, sizeof (struct in6_addr)) != 0) {
		printf("Failed roundtrip: ");
		print_inet6(&rand_addr);
		printf(" -> ");
		print_inet6(&returned_addr);
		printf("\n");
		printf("I-DUNNO address: ");
		print_idunno(dest);
		printf("\n");
		errx(1, "addresses don't match after roundtrip");
	}
	return 1;
}

int main(int argc, char **argv)
{
	int wanted_tests = 1;
	int nb_tests = 0;
	int nb_attempted_tests = 0;
	while (nb_tests < wanted_tests) {
		if (do_one_roundtrip())
			nb_tests++;
		nb_attempted_tests++;
	}

	printf("Did %d valid tests, drawing %d random IPs\n",
	       nb_tests, nb_attempted_tests);
	return 0;
}
