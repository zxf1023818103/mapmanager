#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(mapmanager, 0.0.1, 17204129@stu.nchu.edu.cn)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([main.c])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([iconv], [iconv], [iconv_open])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h inttypes.h stddef.h stdint.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_CHECK_FUNCS([atexit ftruncate munmap strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
