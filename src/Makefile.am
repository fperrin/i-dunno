AM_CFLAGS = -I$(top_srcdir)/src -Wall -Werror @ICU_CFLAGS@ @ICU_I18N_CFLAGS@

lib_LTLIBRARIES = libi-dunno.la
libi_dunno_la_SOURCES = i-dunno-form.c i-dunno-deform.c unicodehelpers.h
libi_dunno_la_LDFLAGS = @ICU_LIBS@ @ICU_I18N_LIBS@
libi_dunno_la_LIBADD = takebits.lo pushbits.lo

bin_PROGRAMS = i-dunno
i_dunno_LDADD = libi-dunno.la

include_HEADERS = i-dunno.h

check_LTLIBRARIES =
check_LTLIBRARIES += libtakebits.la libtakebits_no_s6_addr32.la
libtakebits_la_SOURCES = takebits.c takebits.h
libtakebits_no_s6_addr32_la_SOURCES = takebits.c
libtakebits_no_s6_addr32_la_CPPFLAGS = \
	$(AM_CPPFLAGS) -DFALLBACK_S6_ADDR_FOR_STRUCT_IN6_ADDR

check_LTLIBRARIES += libpushbits.la libpushbits_no_s6_addr32.la
libpushbits_la_SOURCES = pushbits.c pushbits.h
libpushbits_no_s6_addr32_la_SOURCES = pushbits.c
libpushbits_no_s6_addr32_la_CPPFLAGS = \
	$(AM_CPPFLAGS) -DFALLBACK_S6_ADDR_FOR_STRUCT_IN6_ADDR
