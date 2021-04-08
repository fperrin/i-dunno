/* -*- coding: utf-8 -*- */

#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unicode/uidna.h>
#include <unicode/utf8.h>

#include "i-dunno.h"
#include "testutils.h"

/* Note we're using a very slightly different example from the RFC. While
 * "CYRILLIC CAPITAL LIGATURE EN GHE" is an IDNA2008 disallowed character,
 * it's only because it's upper case; "CYRILLIC SMALL LIGATURE EN GHE" **is**
 * allowed. It's more interesting to test with the latter.
 * https://util.unicode.org/UnicodeJsps/idna.jsp?a=c%D2%A5 */
const char *basic_ip = "198.51.100.165";
/* const char *basic_form = "c\flҥ"; */
#define EN_GHE_LIGATURE "\xd2\xa5"
const char *basic_form = "c\fl" EN_GHE_LIGATURE;

void test_uidna_disallowed(void)
{
	UErrorCode err = U_ZERO_ERROR;
	UIDNA *idna = uidna_openUTS46(UIDNA_USE_STD3_RULES, &err);
	assert (U_SUCCESS(err));
	assert (idna);
	UIDNAInfo info = UIDNA_INFO_INITIALIZER;
	char buf[255];

	/* check the basic form is indeed rejected */
	err = U_ZERO_ERROR;
	uidna_labelToASCII_UTF8(idna, basic_form, -1, buf, 255, &info, &err);
	assert (info.errors & UIDNA_ERROR_DISALLOWED);

	/* check that the rejection is due to the \f... */
	err = U_ZERO_ERROR;
	uidna_labelToASCII_UTF8(idna, "c\fa", -1,
				buf, 255, &info, &err);
	assert (info.errors & UIDNA_ERROR_DISALLOWED);

	/* ...and not to the Cyrillic character */
	err = U_ZERO_ERROR;
	uidna_labelToASCII_UTF8(idna, "c" EN_GHE_LIGATURE, -1,
				buf, 255, &info, &err);
	assert (! info.errors);
	assert (strcmp(buf, "xn--c-xzb") == 0);

	uidna_close(idna);
}

void test_basic_form(void)
{
	char dest[I_DUNNO_ADDRSTRLEN] = { 0, };

	struct in_addr addr;
	assert (inet_pton(AF_INET, basic_ip, &addr));

	assert (i_dunno_form(AF_INET, &addr, dest, I_DUNNO_ADDRSTRLEN,
			     I_DUNNO_MINIMUM_CONFUSION | I_DUNNO_NO_PADDING));
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
	assert (strcmp(addrp, basic_ip) == 0);
}


int main(int argc, char **argv)
{
	printf("Test IDNA\n");
	test_uidna_disallowed();
	printf("Test basic form\n");
	test_basic_form();
	printf("Test basic deform\n");
	test_basic_deform();
	printf("All done\n");
	return 0;
}
