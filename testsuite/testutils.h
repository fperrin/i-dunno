#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <netinet/in.h>

void print_inet(struct in_addr *addr);
void print_inet6(struct in6_addr *addr);
void print_idunno(char *addr);
int u_strcmp(const unsigned char *s1, const unsigned char *s2);

#endif
