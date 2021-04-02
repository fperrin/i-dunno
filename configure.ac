AC_INIT([i-dunno], [0.1], [fred@fperrin.net])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC
AM_PROG_AR
AC_REPLACE_FUNCS
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile src/Makefile testsuite/Makefile
])
LT_INIT([disable-static])

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([ICU], [icu-uc])
PKG_CHECK_MODULES([ICU_I18N], [icu-i18n])

AC_CHECK_MEMBER([struct in6_addr.s6_addr32],
		[], [],
		[[#include <netinet/in.h>]])

if test "x$ac_cv_member_struct_in6_addr_s6_addr32" != "xyes"; then
    AC_DEFINE_UNQUOTED(FALLBACK_S6_ADDR_FOR_STRUCT_IN6_ADDR, 1,
	[Fallback to using 8-bit s6_addr to access struct in6_addr elements])
fi

AC_OUTPUT
