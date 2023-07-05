#include <arpa/inet.h>
#include <err.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unicode/uchar.h>
#include <unicode/utf8.h>
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
	printf("%s\n", addr);

	int cp = -1, idx = 0, namelen = 128;
	UErrorCode err = U_ZERO_ERROR;
	char name[namelen];
	for (;;) {
		U8_NEXT(addr, idx, -1, cp);
		if (cp == 0)
			return;
		u_charName(cp, U_EXTENDED_CHAR_NAME, name, namelen, &err);
		printf("U+%04x: %s\n", cp,
		       U_SUCCESS(err)? name: "???");
	}
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

struct many_check_args_t {
	check_one_t do_one_check;
	pthread_t thread;
	int flags;
	int wanted_tests;
	int nb_tests;
	int attempted_tests;
};

static void *do_many_checks_thread(void *args)
{
	struct many_check_args_t *checkargs = args;
	checkargs->thread = pthread_self();

	while (checkargs->nb_tests < checkargs->wanted_tests) {
		if (checkargs->do_one_check(checkargs->flags))
			checkargs->nb_tests++;
		checkargs->attempted_tests++;
	}
	pthread_exit(NULL);
	return NULL;
}

void do_many_checks(check_one_t do_one_check, int flags, int wanted_tests)
{
	int nb_threads = 8;
	pthread_t threads[nb_threads];
	struct many_check_args_t thread_args[nb_threads];
	time_t start = time(NULL);

	for (int i = 0; i < nb_threads; i++) {
		int thread_wanted_tests = wanted_tests / nb_threads;
		thread_args[i].do_one_check = do_one_check;
		thread_args[i].flags = flags;
		thread_args[i].wanted_tests = thread_wanted_tests;
		thread_args[i].nb_tests = 0;
		thread_args[i].attempted_tests = 0;

		if (i == 0)
			thread_wanted_tests += wanted_tests % nb_threads;
		pthread_create(&threads[i], NULL,
			       &do_many_checks_thread, &thread_args[i]);
	}

	int nb_tests = 0;
	int nb_attempted_tests = 0;
	for (int i = 0; i < nb_threads; i++) {
		int rval = pthread_join(threads[i], NULL);
		if (rval) {
			errno = rval;
			err(1, "pthread_join");
		}
		nb_tests += thread_args[i].nb_tests;
		nb_attempted_tests += thread_args[i].attempted_tests;
	}

	printf("Did %d valid tests in %.0fs, drawing %d random IPs\n",
	       nb_tests, difftime(time(NULL), start), nb_attempted_tests);
}
