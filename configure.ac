AC_PREREQ([2.68])
AC_INIT([hyperrogue], [10.4i])
AC_LANG([C++])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([hyperpoint.cpp])
AC_PROG_CXX

AC_CANONICAL_HOST
AM_CONDITIONAL(LINUX, test ["${host_os#*linux}" != "$host_os"])
AM_CONDITIONAL(MAC, test ["${host_os#*darwin}" != "$host_os"])
AM_CONDITIONAL(MINGW, test ["${host_os#*mingw}" != "$host_os"])
AM_COND_IF(LINUX, AC_DEFINE([LINUX], [1]))
AM_COND_IF(MAC, AC_DEFINE([MAC], [1]))
AM_COND_IF(MINGW, AC_DEFINE([WINDOWS], [1]))

AC_CHECK_HEADERS([GL/gl.h OpenGL/gl.h], break, AC_MSG_RESULT([OpenGL header was not found]))
AC_CHECK_HEADERS([GL/glew.h], [], AC_MSG_RESULT([GLEW header was not found]))
AC_CHECK_HEADERS([png.h], [], AC_MSG_RESULT([png.h header was not found]))
AC_CHECK_HEADERS([SDL/SDL.h], [], AC_MSG_ERROR([SDL/SDL.h header was not found]))
AC_CHECK_HEADERS([SDL/SDL_gfxPrimitives.h], [], AC_MSG_RESULT([SDL/SDL_gfxPrimitives.h header was not found]))
AC_CHECK_HEADERS([SDL/SDL_mixer.h], [], AC_MSG_ERROR([SDL/SDL_mixer.h header was not found]))
AC_CHECK_HEADERS([SDL/SDL_ttf.h], [], AC_MSG_RESULT([SDL/SDL_ttf.h header was not found]))

AC_SEARCH_LIBS([glBegin], [GL opengl32], [], AC_MSG_RESULT([OpenGL library was not found]))
AC_SEARCH_LIBS([glewInit], [GLEW glew32], [], AC_MSG_RESULT([GLEW library was not found]))
AC_SEARCH_LIBS([png_create_info_struct], [png], [], AC_MSG_RESULT([png library was not found]))
AC_SEARCH_LIBS([SDL_SetVideoMode], [SDL], [], AC_MSG_ERROR([SDL library was not found]))
AC_SEARCH_LIBS([aacircleColor], [SDL_gfx], [], AC_MSG_RESULT([SDL_gfx library was not found]))
AC_SEARCH_LIBS([Mix_LoadMUS], [SDL_mixer], [], AC_MSG_ERROR([SDL_mixer library was not found]))
AC_SEARCH_LIBS([TTF_OpenFont], [SDL_ttf], [], AC_MSG_RESULT([SDL_ttf library was not found]))

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
