#include <arpa/inet.h>
#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "i-dunno.h"
#include "testutils.h"

int do_one_roundtrip(int flags, int print)
{
	struct in6_addr rand_addr;
	for (int i = 0; i < 16; i++)
		rand_addr.s6_addr[i] = random();

	char dest[I_DUNNO_ADDRSTRLEN];
	if (! i_dunno_form(AF_INET6, &rand_addr, dest,
			   I_DUNNO_ADDRSTRLEN, flags))
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
	} else if (print) {
		print_inet6(&rand_addr);
		printf(" -> ");
		print_idunno(dest);
	}
	return 1;
}

int do_one_roundtrip_noprint(int flags)
{
	return do_one_roundtrip(flags, false);
}

int do_one_roundtrip_print(int flags)
{
	return do_one_roundtrip(flags, true);
}

int main(int argc, char **argv)
{
	int wanted_tests = 10000;
	set_rand_seed(argc, argv);

	do_one_roundtrip_print(I_DUNNO_MINIMUM_CONFUSION);
	do_many_checks(do_one_roundtrip_noprint,
	    I_DUNNO_MINIMUM_CONFUSION,
	    wanted_tests);
	do_many_checks(do_one_roundtrip_noprint,
	    I_DUNNO_MINIMUM_CONFUSION | I_DUNNO_RANDOMIZE,
	    wanted_tests);
	do_many_checks(do_one_roundtrip_noprint,
	    I_DUNNO_MINIMUM_CONFUSION | I_DUNNO_NO_PADDING,
	    wanted_tests);
	do_many_checks(do_one_roundtrip_noprint,
	    I_DUNNO_MINIMUM_CONFUSION | I_DUNNO_RANDOMIZE | I_DUNNO_NO_PADDING,
	    wanted_tests);

	do_one_roundtrip_print(I_DUNNO_SATISFACTORY_CONFUSION);
	do_many_checks(do_one_roundtrip_noprint,
	    I_DUNNO_SATISFACTORY_CONFUSION,
	    wanted_tests);
	do_many_checks(do_one_roundtrip_noprint,
	    I_DUNNO_SATISFACTORY_CONFUSION | I_DUNNO_RANDOMIZE,
	    wanted_tests);
	do_many_checks(do_one_roundtrip_noprint,
	    I_DUNNO_SATISFACTORY_CONFUSION | I_DUNNO_NO_PADDING,
	    wanted_tests);
	do_many_checks(do_one_roundtrip_noprint,
	    I_DUNNO_SATISFACTORY_CONFUSION | I_DUNNO_RANDOMIZE | I_DUNNO_NO_PADDING,
	    wanted_tests);

	/* It's harder to find addresses of delightful confusion, so reduce
	 * number of wanted tests */
	wanted_tests = 100;
	do_one_roundtrip_print(I_DUNNO_DELIGHTFUL_CONFUSION);
	do_many_checks(
	    do_one_roundtrip_noprint,
	    I_DUNNO_DELIGHTFUL_CONFUSION,
	    wanted_tests);
	do_many_checks(do_one_roundtrip_noprint,
	    I_DUNNO_DELIGHTFUL_CONFUSION | I_DUNNO_RANDOMIZE,
	    wanted_tests);
	do_many_checks(do_one_roundtrip_noprint,
	    I_DUNNO_DELIGHTFUL_CONFUSION | I_DUNNO_NO_PADDING,
	    wanted_tests);
	do_many_checks(do_one_roundtrip_noprint,
	    I_DUNNO_DELIGHTFUL_CONFUSION | I_DUNNO_RANDOMIZE | I_DUNNO_NO_PADDING,
	    wanted_tests);

	return 0;
}
