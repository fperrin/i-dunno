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

AC_OUTPUT
