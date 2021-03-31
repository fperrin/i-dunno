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
	struct in_addr rand_addr;
	rand_addr.s_addr = random();

	char dest[I_DUNNO_ADDRSTRLEN];
	if (! i_dunno_form(AF_INET, &rand_addr, dest, I_DUNNO_ADDRSTRLEN, 0))
		return 0;

	struct in_addr returned_addr = { 0 };
	assert (i_dunno_deform(AF_INET, dest, &returned_addr));

	if (rand_addr.s_addr != returned_addr.s_addr) {
		printf("Failed roundtrip: ");
		print_inet(&rand_addr);
		printf("-> ");
		print_inet(&returned_addr);
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
	int wanted_tests = 10000;
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