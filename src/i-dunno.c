#include <config.h>
#include <i-dunno.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void usage(char *name)
{
	printf("Usage: %s [-msdpr0h] ADDRESS...\n", name);
	printf("\
Prints an I-DUNNO representation of ADDRESS (IPv4 or IPv6).\n\
\n\
  -m	generate Minimum level of confusion (default)\n\
  -s	generate Satisfactory level of confusion\n\
  -d	generate Delightful level of confusion\n\
  -p	do not pad the source when forming address\n\
  -r	randomize list of codepoints considered when forming I-DUNNO\n\
  -0	print a null character between each form\n\
  -h	print this help\n\
\n\
The Mininum level of confusion is always enabled.\n\
\n\
Exits with a non-zero status if the requested level of confusion cannot be\n\
reached.\n\
");
}

int main(int argc, char **argv)
{
	int flag = I_DUNNO_MINIMUM_CONFUSION;
	int opt;
	char outseparator = '\n';
	int exitval = 0;

	while ((opt = getopt(argc, argv, "msdpr0h")) != -1) {
		switch (opt) {
		case 'm':
			/* Nothing, already set as default */
			break;
		case 's':
			flag |= I_DUNNO_SATISFACTORY_CONFUSION;
			break;
		case 'd':
			flag |= I_DUNNO_DELIGHTFUL_CONFUSION;
			break;
		case 'p':
			flag |= I_DUNNO_NO_PADDING;
			break;
		case 'r':
			flag |= I_DUNNO_RANDOMIZE;
			/* bad randomization, for the sake of not getting
			 * the same form twice */
			srandom(time(NULL));
			break;
		case '0':
			outseparator = '\0';
			break;
		case 'h':
			usage(argv[0]);
			return 0;
			break;
		default:
			usage(argv[0]);
			return 1;
		}
	}

	if (optind >= argc) {
		printf("missing operand\n");
		usage(argv[0]);
		return 1;
	}

	for (int i = optind; i < argc; i++) {
		char *addrp = argv[i];

		int af;
		void *addr;
		struct in_addr addr4;
		struct in6_addr addr6;
		char dest[I_DUNNO_ADDRSTRLEN];

		if (inet_pton(AF_INET, addrp, &addr4)) {
			af = AF_INET;
			addr = &addr4;
		} else if (inet_pton(AF_INET6, addrp, &addr6)) {
			af = AF_INET6;
			addr = &addr6;
		} else {
			errx(1, "Couldn't parse address %s\n", addrp);
		}
		if (! i_dunno_form(af, addr, dest, I_DUNNO_ADDRSTRLEN, flag)) {
			exitval = 1;
			printf("%c", outseparator);
			continue;
		}
		printf("%s%c", dest, outseparator);
	}

	return exitval;
}
