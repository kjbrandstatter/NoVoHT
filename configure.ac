#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([NoVoHT], [0.2], [kbrandst@hawk.iit.edu])
AM_INIT_AUTOMAKE([1.10 no-define foreign])
#AC_CONFIG_SRCDIR([tests/gpbbench.cxx])
#AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

#check for basic library functions
AC_CHECK_LIB([c], [printf])
AC_CHECK_LIB([m], [pow], [])
AC_CHECK_LIB([pthread], [pthread_create])


# Checks for header files.
AC_CHECK_HEADERS([stddef.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_OFF_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([ftruncate gettimeofday pow sqrt])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_ARG_WITH(tests, [--without-tests disable tests])
if test "$with_tests" == "no"
then
	echo Tests Disabled
else
	AC_CONFIG_FILES([tests/Makefile])
fi
AC_OUTPUT
