#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libmnlxt], [0.1.1])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_SRCDIR([src/mnlxt.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
LT_INIT
AC_CHECK_LIB([mnl], [mnl_socket_open], [], [AC_MSG_ERROR([mnl_socket_open was not found in libmnl])])

# Checks for header files.
AC_CHECK_HEADERS([netinet/in.h stdlib.h string.h sys/socket.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strdup])

#dnl enable-tests
AC_MSG_CHECKING(whether to compile the test applications)
AC_ARG_ENABLE(tests,
  [  --enable-tests           compile the test applications],
  [  case "$enableval" in
	no) AC_MSG_RESULT(no) ;;
	*)  AC_MSG_RESULT(yes)
		AC_SUBST([build_tests],[tests])
		AC_CONFIG_FILES([tests/Makefile])
	;;
	esac],
  AC_MSG_RESULT(no))

AC_CONFIG_FILES([Makefile
                 include/Makefile
                 src/Makefile])
AC_OUTPUT
