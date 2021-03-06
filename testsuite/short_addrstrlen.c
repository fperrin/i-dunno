#include <arpa/inet.h>
#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "i-dunno.h"
#include "testutils.h"

/* canary value that can't be generated by any UTF-8 codepoint */
const char canary = '\xff';

int do_one_check(int flags)
{
	struct in6_addr rand_addr;
	for (int i = 0; i < 16; i++)
		rand_addr.s6_addr[i] = random();

	char dest[I_DUNNO_ADDRSTRLEN + 1];
	dest[I_DUNNO_ADDRSTRLEN] = canary;
	if (! i_dunno_form(AF_INET6, &rand_addr, dest,
			   I_DUNNO_ADDRSTRLEN, flags))
		return 0;

	assert (dest[I_DUNNO_ADDRSTRLEN] == canary);

	size_t shortlen;
	do {
		shortlen = strlen(dest);

		char dest2[I_DUNNO_ADDRSTRLEN];
		dest2[shortlen] = canary;
		if (! i_dunno_form(AF_INET6, &rand_addr, dest2,
				   shortlen, flags)) {
			/* Could not find any other forms with the new
			 * shortlen. Check the canary value, then return
			 * success */
			assert (dest2[shortlen] == canary);
			return 1;
		}

		/* We were able find a shorter form. It must be different
		 * from the previous one, and check the canary too */
		assert (dest2[shortlen] == canary);
		assert (strlen(dest2) < strlen(dest));
		assert (strcmp(dest, dest2) != 0);
		strncpy(dest, dest2, I_DUNNO_ADDRSTRLEN);
	} while (shortlen > 0);
	assert (false);		/* how come we didn't exit? */

	return 1;
}

int main(int argc, char **argv)
{
	int wanted_tests = 1000;
	set_rand_seed(argc, argv);

	do_many_checks(do_one_check,
	    I_DUNNO_SATISFACTORY_CONFUSION,
	    wanted_tests);
	do_many_checks(do_one_check,
	    I_DUNNO_SATISFACTORY_CONFUSION | I_DUNNO_NO_PADDING,
	    wanted_tests);

	return 0;
}
