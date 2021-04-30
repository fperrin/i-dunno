#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <netinet/in.h>

void print_inet(struct in_addr *addr);
void print_inet6(struct in6_addr *addr);
void print_idunno(char *addr);
int u_strcmp(const unsigned char *s1, const unsigned char *s2);
void set_rand_seed(int argc, char **argv);

typedef int (*check_one_t)(int);
void do_many_checks(check_one_t do_one_check, int flags, int wanted_tests);

#endif
