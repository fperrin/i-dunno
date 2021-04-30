AM_CFLAGS = -I$(top_srcdir)/src -Wall -Werror

LDADD = $(top_builddir)/src/libi-dunno.la libtestutils.a

noinst_LIBRARIES = libtestutils.a
libtestutils_a_SOURCES = testutils.c testutils.h

check_PROGRAMS = \
	test_bits test_bits_no_s6_addr32 \
	idunno_addrstrlen short_addrstrlen \
	test_basic \
	test_roundtrip_v4 test_roundtrip_v6

check_SCRIPTS = \
	interop-py-i-dunno.py

test_bits_LDADD = \
	$(top_builddir)/src/libtakebits.la \
	$(top_builddir)/src/libpushbits.la

test_bits_no_s6_addr32_SOURCES = test_bits.c
test_bits_no_s6_addr32_LDADD = \
	$(top_builddir)/src/libtakebits_no_s6_addr32.la \
	$(top_builddir)/src/libpushbits_no_s6_addr32.la

test_basic_CFLAGS = $(AM_CFLAGS) @ICU_CFLAGS@ @ICU_I18N_CFLAGS@
test_basic_LDFLAGS = @ICU_LIBS@

TESTS = $(check_PROGRAMS) $(check_SCRIPTS)
EXTRA_DIST = $(check_SCRIPTS)
