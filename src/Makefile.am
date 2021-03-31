AM_CFLAGS = -I$(top_srcdir)/src -Wall -Werror

lib_LTLIBRARIES = libi-dunno.la
libi_dunno_la_SOURCES = i-dunno-form.c i-dunno-deform.c
libi_dunno_la_LDFLAGS = @ICU_LIBS@ @ICU_I18N_LIBS@

bin_PROGRAMS = i-dunno
LDADD = libi-dunno.la

include_HEADERS = i-dunno.h
