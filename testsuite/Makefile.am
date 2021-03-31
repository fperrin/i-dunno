AM_CFLAGS = -I$(top_srcdir)/src -Wall -Werror

LDADD = ../src/libi-dunno.la testutils.o

check_PROGRAMS = \
	test_bits \
	test_basic \
	test_roundtrip_v4 test_roundtrip_v6

test_basic_LDFLAGS = @ICU_LIBS@

TESTS = $(check_PROGRAMS)
