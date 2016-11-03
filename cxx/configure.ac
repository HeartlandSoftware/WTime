AC_INIT([Times], [1.0], [rbryce@heartlandsoftware.ca])
AC_CONFIG_HEADERS([config.h:config.h.in])
AM_INIT_AUTOMAKE([foreign])
AM_SILENT_RULES([yes])
AC_ARG_ENABLE(debug,
AS_HELP_STRING([--enable-debug],
			   [enable debugging, default: no]),
[case "${enableval}" in
			 yes) debug=true ;;
			 no)  debug=false ;;
			 *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],
[debug=false])
AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")
LT_INIT
AC_PROG_CXX
AC_CONFIG_MACRO_DIR([m4])
AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([cstdio stdio.h], [break;], [])
AC_CHECK_HEADERS([cstdlib stdlib.h], [break;], [])
AC_CHECK_HEADERS([cstdint stdint.h], [break;], [])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT