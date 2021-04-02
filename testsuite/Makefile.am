AM_CFLAGS = -I$(top_srcdir)/src -Wall -Werror

LDADD = ../src/libi-dunno.la testutils.o

check_PROGRAMS = \
	test_bits test_bits_no_s6_addr32 \
	test_basic \
	test_roundtrip_v4 test_roundtrip_v6

test_bits_LDADD = \
	$(top_srcdir)/src/libtakebits.la \
	$(top_srcdir)/src/libpushbits.la

test_bits_no_s6_addr32_SOURCES = test_bits.c
test_bits_no_s6_addr32_LDADD = \
	$(top_srcdir)/src/libtakebits_no_s6_addr32.la \
	$(top_srcdir)/src/libpushbits_no_s6_addr32.la

test_basic_LDFLAGS = @ICU_LIBS@

TESTS = $(check_PROGRAMS)
