#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "testutils.h"

void print_inet(struct in_addr *addr)
{
	char addrp[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, addr, addrp, INET_ADDRSTRLEN);
	printf("%s", addrp);
}

void print_inet6(struct in6_addr *addr)
{
	char addrp[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET6, addr, addrp, INET6_ADDRSTRLEN);
	printf("%s", addrp);
}

void print_idunno(char *addr)
{
	for (char *c = addr; *c; c++)
		printf("%hhx", *c);
}

int u_strcmp(const unsigned char *s1, const unsigned char *s2)
{
	return strcmp((char *)s1, (char *)s2);
}

void set_rand_seed(int argc, char **argv)
{
	unsigned int seed;
	if (argc >= 2)
		seed = atoi(argv[1]);
	else
		seed = time(NULL);
	printf("Setting srandom(%u)\n", seed);
	srandom(seed);
}
