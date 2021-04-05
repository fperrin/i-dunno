#include <config.h>
#include <i-dunno.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void usage(char *name)
{
	printf("Usage: %s [-msd] ADDRESS\n", name);
	printf("\
Prints an I-DUNNO representation of ADDRESS (IPv4 or IPv6).\n\
\n\
  -s	generate Satisfactory level of confusion\n\
  -d	generate Delightful level of confusion\n\
  -p	do not pad the source when forming address\n\
  -r	randomize list of codepoints considered when forming I-DUNNO\n\
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

	while ((opt = getopt(argc, argv, "sdprh")) != -1) {
		switch (opt) {
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
	char *addrp = argv[optind];

	struct in_addr addr;
	struct in6_addr addr6;
	char dest[I_DUNNO_ADDRSTRLEN];

	if (inet_pton(AF_INET, addrp, &addr)) {
		if (! i_dunno_form(AF_INET, &addr, dest,
				   I_DUNNO_ADDRSTRLEN, flag))
			errx(1, "Don't know how to form address\n");
	} else if (inet_pton(AF_INET6, addrp, &addr6)) {
		if (! i_dunno_form(AF_INET6, &addr6, dest,
				   I_DUNNO_ADDRSTRLEN, flag))
			errx(1, "Don't know how to form address\n");
	} else {
		err(1, "Couldn't parse address %s\n", argv[1]);
	}

	printf("%s\n", dest);
	return 0;
}
