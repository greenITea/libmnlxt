#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libmnlxt], [0.2.1])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_SRCDIR([src/mnlxt.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

# Checks for libraries.
LT_INIT
AC_CHECK_LIB([mnl], [mnl_socket_open], [], [AC_MSG_ERROR([mnl_socket_open was not found in libmnl])])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h netinet/in.h stdlib.h string.h sys/socket.h])

AC_MSG_CHECKING([for IFA_FLAGS])
AC_COMPILE_IFELSE(
  [AC_LANG_PROGRAM([#include <linux/if_addr.h>],[int flags = IFA_FLAGS;])],
  [
    AC_MSG_RESULT([yes])
    AC_DEFINE_UNQUOTED([HAVE_IFA_FLAGS], 1, [Define to 1 if IFA_FLAGS is usable.])
  ],[
    AC_MSG_RESULT([no])
    AC_DEFINE_UNQUOTED([HAVE_IFA_FLAGS], 0, [Define to 1 if IFA_FLAGS is usable.])
  ]
)

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strchr strdup strtol])

CFLAGS="$CFLAGS -Wall -Werror"

AC_MSG_CHECKING(whether to enable rtm support)
AC_ARG_ENABLE(rtm,
  AS_HELP_STRING([--enable-rtm],
                 [enable RTM support, default: yes]),
  [case "${enableval}" in
    yes)
      rtm=true
      ;;
    no)
      rtm=false
      ;;
    *)
      AC_MSG_ERROR([bad value ${enableval} for --enable-rtm])
      ;;
  esac],
  rtm=true
)

if [ $rtm = true ]; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi

AM_CONDITIONAL(ENABLE_RTM, [ $rtm = true ])

AC_MSG_CHECKING(whether to enable xfrm support)
AC_ARG_ENABLE(xfrm,
  AS_HELP_STRING([--enable-xfrm],
                 [enable XFRM support, default: yes]),
  [case "${enableval}" in
    yes)
      xfrm=true
      ;;
    no)
      xfrm=false
      ;;
    *)
      AC_MSG_ERROR([bad value ${enableval} for --enable-xfrm])
      ;;
  esac],
  xfrm=true
)

if [ $xfrm = true ]; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi

AM_CONDITIONAL(ENABLE_XFRM, [ $xfrm = true ])


#dnl enable-tests
AC_MSG_CHECKING(whether to compile the test applications)
AC_ARG_ENABLE(tests,
  [  --enable-tests           compile the test applications],
  [  case "$enableval" in
	no) AC_MSG_RESULT(no) ;;
	*)  AC_MSG_RESULT(yes)
		AC_SUBST([build_tests],[tests])
		AC_CONFIG_FILES([tests/Makefile
		                 tests/rtnl/Makefile
		                 tests/xfrm/Makefile])
	;;
	esac],
  AC_MSG_RESULT(no))

AC_CONFIG_FILES([Makefile
                 include/Makefile
                 src/Makefile])
AC_OUTPUT
