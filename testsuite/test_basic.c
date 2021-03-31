#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unicode/utf8.h>

#include "i-dunno.h"
#include "testutils.h"

const char *basic_ip = "198.51.100.164";
const char *basic_form = "c\flҤ";

void test_basic_form(void)
{
	char dest[I_DUNNO_ADDRSTRLEN];

	struct in_addr addr;
	assert (inet_pton(AF_INET, basic_ip, &addr));

	assert (i_dunno_form(AF_INET, &addr, dest, 32,
			     I_DUNNO_MINIMUM_CONFUSION));
	printf("%s", dest);
	assert (strcmp(dest, basic_form) == 0);
}

void test_basic_deform(void)
{
	struct in_addr addr;
	assert (i_dunno_deform(AF_INET, basic_form, &addr));

	char addrp[INET_ADDRSTRLEN];
	assert (inet_ntop(AF_INET, &addr, addrp, INET_ADDRSTRLEN));
	printf("%s\n", addrp);
	assert (strcmp(addrp, "198.51.100.164") == 0);
}


int main(int argc, char **argv)
{
	test_basic_form();
	test_basic_deform();
	return 0;
}
