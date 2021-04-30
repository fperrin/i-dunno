#include <arpa/inet.h>
#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "i-dunno.h"
#include "testutils.h"

int do_one_roundtrip(int flags)
{
	struct in_addr rand_addr;
	rand_addr.s_addr = random();

	char dest[I_DUNNO_ADDRSTRLEN];
	if (! i_dunno_form(AF_INET, &rand_addr, dest,
			   I_DUNNO_ADDRSTRLEN,
			   flags))
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
	set_rand_seed(argc, argv);

	do_many_checks(do_one_roundtrip,
		       I_DUNNO_MINIMUM_CONFUSION,
		       wanted_tests);
	do_many_checks(do_one_roundtrip,
		       I_DUNNO_SATISFACTORY_CONFUSION,
		       wanted_tests);
	do_many_checks(do_one_roundtrip,
		       I_DUNNO_DELIGHTFUL_CONFUSION,
		       wanted_tests);

	return 0;
}
