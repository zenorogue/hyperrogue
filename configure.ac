#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([hyperrogue], [6.6])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([hyperpoint.cpp])
#AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lGL':
AC_CHECK_LIB([GL], [main], [], [AC_MSG_ERROR([GL library was not found])])
# FIXME: Replace `main' with a function in `-lSDL':
AC_CHECK_LIB([SDL], [main], [], [AC_MSG_ERROR([SDL library was not found])])
# FIXME: Replace `main' with a function in `-lSDL_gfx':
AC_CHECK_LIB([SDL_gfx], [main], [], [AC_MSG_ERROR([SDL_gfx library was not found])])
# FIXME: Replace `main' with a function in `-lSDL_mixer':
AC_CHECK_LIB([SDL_mixer], [main], [], [AC_MSG_ERROR([SDL_mixer library was not found])])
# FIXME: Replace `main' with a function in `-lSDL_ttf':
AC_CHECK_LIB([SDL_ttf], [main], [], [AC_MSG_ERROR([SDL_ttf library was not found])])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h unistd.h])
AC_CHECK_HEADERS([SDL/SDL.h], [], [AC_MSG_ERROR([SDL/SDL.h header was not found])])
#AC_CHECK_HEADERS([SDL/SDL_gfx.h], [], [AC_MSG_ERROR([SDL/SDL_gfx.h header was not found])])
AC_CHECK_HEADERS([SDL/SDL_mixer.h], [], [AC_MSG_ERROR([SDL/SDL_mixer.h header was not found])])
AC_CHECK_HEADERS([SDL/SDL_ttf.h], [], [AC_MSG_ERROR([SDL/SDL_ttf.h header was not found])])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([memset sqrt strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
