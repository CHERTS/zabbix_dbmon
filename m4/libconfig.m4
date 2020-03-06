# LIBCONFIG_CHECK_CONFIG ([DEFAULT-ACTION])
# ----------------------------------------------------------
#    Mikhail Grigorev                      Mar-06-2020
#
# Checks for libconfig functions.

AC_DEFUN([LIBCONFIG_TRY_LINK],
[
am_save_LIBS="$LIBS"
LIBS="$LIBS $1"
AC_TRY_LINK(
[
#include <libconfig.h>
],
[
	config_t cfg;

	config_init(&cfg);
	config_destroy(&cfg);
],
found_libconfig="yes"
CONFIG_LIBS="$1")
LIBS="$am_save_LIBS"
])dnl

AC_DEFUN([LIBCONFIG_CHECK_CONFIG],
[
	AC_MSG_CHECKING(for libconfig functions)

	LIBCONFIG_TRY_LINK([])

	if test "x$found_libconfig" != "xyes"; then
		LIBCONFIG_TRY_LINK([-lconfig])
	fi

	if test "x$found_libconfig" = "xyes"; then
		AC_DEFINE([LIBCONFIG_API], 1, [Define to 1 if you have the libconfig functions])
	else
		AC_MSG_RESULT(no)
	fi

	AC_MSG_RESULT($found_libconfig)

	AC_SUBST(CONFIG_LIBS)
])dnl
