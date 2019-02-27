// Hyperbolic Rogue -- set compiler flags, and include all the required system headers
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

#ifdef MAC
#define ISMAC 1
#endif

#ifdef LINUX
#define ISLINUX 1
#endif

#ifdef WINDOWS
#define ISWINDOWS 1
#endif

// OS settings

#ifndef ISMAC
#define ISMAC 0
#endif

#ifndef ISLINUX
#define ISLINUX 0
#endif

#ifndef ISWINDOWS
#define ISWINDOWS 0
#endif

#ifndef ISPANDORA
#define ISPANDORA 0
#endif

#ifndef ISIOS
#define ISIOS 0
#endif

#ifndef ISANDROID
#define ISANDROID 0
#endif

#ifndef ISSTEAM
#define ISSTEAM 0
#endif

#ifndef ISWEB
#define ISWEB 0
#endif

#ifndef ISFAKEMOBILE
#define ISFAKEMOBILE 0
#endif

#define ISMOBILE (ISIOS || ISANDROID || ISFAKEMOBILE)
#define ISMOBWEB (ISMOBILE || ISWEB)

#ifndef ISMINI
#define ISMINI 0
#endif

#ifndef CAP_XGD
#define CAP_XGD (ISANDROID || ISFAKEMOBILE)
#endif

#define CAP_FRAMELIMIT (!ISMOBWEB)

#if ISMOBILE
#define EXTERNALFONT
#endif

#ifndef NOLICENSE
#define NOLICENSE ISSTEAM
#endif

#ifndef CAP_SHADER
#define CAP_SHADER CAP_GL
#endif

#ifndef CAP_ANIMATIONS
#define CAP_ANIMATIONS (!ISMINI)
#endif

#ifndef CAP_FILES
#define CAP_FILES (!ISMINI)
#endif

#ifndef CAP_INV
#define CAP_INV (!ISMINI)
#endif

#ifndef CAP_ANDROIDSHARE
#define CAP_ANDROIDSHARE (ISANDROID)
#endif

#ifndef CAP_SDL
#define CAP_SDL (!ISMOBILE)
#endif

#ifndef CAP_COMPASS
#define CAP_COMPASS ISMOBILE
#endif

#ifndef CAP_SDLGFX
#define CAP_SDLGFX (CAP_SDL && !ISWEB)
#endif

#ifndef CAP_GL
#define CAP_GL (ISMOBILE || CAP_SDL)
#endif

#ifndef CAP_AUDIO
#define CAP_AUDIO ((ISMOBILE || CAP_SDL) && !ISWEB && !ISMINI)
#endif

#define CAP_GLORNOT (CAP_GL && !ISWEB && !ISIOS) 

#if ISSTEAM
#define CAP_DAILY 1
#endif

#ifndef CAP_DAILY
#define CAP_DAILY 0
#endif

#ifndef CAP_CERTIFY
#define CAP_CERTIFY 0
#endif

#ifndef CAP_RUG
#define CAP_RUG (!ISMINI && CAP_GL)
#endif

#ifndef CAP_SURFACE
#define CAP_SURFACE CAP_RUG
#endif

#ifndef CAP_EDIT
#define CAP_EDIT (CAP_FILES && !ISWEB && !ISMINI)
#endif

#ifndef CAP_SHOT
#define CAP_SHOT (CAP_FILES && (CAP_SVG || CAP_PNG))
#endif

#ifndef CAP_ODS
#define CAP_ODS 0
#endif

#ifndef CAP_TEXTURE
#define CAP_TEXTURE (CAP_GL && (CAP_PNG || CAP_SDL_IMG) && !ISMINI)
#endif

#ifndef CAP_MODEL
#define CAP_MODEL (!ISMOBWEB && !ISMINI && CAP_SDLGFX)
#endif

#ifndef CAP_SAVE
#define CAP_SAVE (CAP_FILES && !ISWEB && !ISMINI)
#endif

#ifndef CAP_CONFIG
#define CAP_CONFIG (CAP_FILES && !ISWEB && !ISMINI)
#endif

#ifndef CAP_TRANS
#define CAP_TRANS (!ISWEB && !ISMINI)
#endif

#ifndef CAP_TOUR
#define CAP_TOUR (!ISWEB && !ISMINI)
#endif

#ifndef CAP_ROGUEVIZ
#define CAP_ROGUEVIZ 0
#endif

#ifndef CAP_PROFILING
#define CAP_PROFILING 0
#endif

#define PSEUDOKEY_WHEELDOWN 2501
#define PSEUDOKEY_WHEELUP 2502
#define PSEUDOKEY_RELEASE 2503

#ifndef CAP_PNG
#define CAP_PNG (!ISMOBWEB)
#endif

#ifndef CAP_ORIENTATION
#define CAP_ORIENTATION ISMOBILE
#endif

#ifndef CAP_COMMANDLINE
#define CAP_COMMANDLINE (!ISMOBILE)
#endif

#define CAP_SDLAUDIO (CAP_SDL && CAP_AUDIO)

#ifndef CAP_SVG
#define CAP_SVG (CAP_FILES && !ISMOBILE && !ISMINI)
#endif

#ifndef CAP_POLY
#define CAP_POLY (CAP_SDLGFX || CAP_GL || CAP_SVG)
#endif

#define CAP_QUEUE CAP_POLY
#define CAP_CURVE CAP_POLY

#ifndef CAP_SDLJOY
#define CAP_SDLJOY (CAP_SDL && !ISWEB)
#endif

#ifndef CAP_SDL_IMG
#define CAP_SDL_IMG 0
#endif

#ifndef CAP_SDLTTF
#define CAP_SDLTTF (CAP_SDL && !ISMOBILE && !ISWEB)
#endif

#define CAP_GLFONT (CAP_GL && !ISMOBILE)

#ifndef CAP_TABFONT
#define CAP_TABFONT (ISWEB)
#endif

#ifndef CAP_CREATEFONT
#define CAP_CREATEFONT 0
#endif

#ifndef CAP_FIXEDSIZE
#define CAP_FIXEDSIZE (CAP_CREATEFONT || CAP_TABFONT ? 36 : 0)
#endif

#ifndef CAP_SHMUP
#define CAP_SHMUP 1
#endif

#ifndef CAP_BITFIELD
#define CAP_BITFIELD (!ISWEB)
#endif

#ifndef CAP_ACHIEVE
#define CAP_ACHIEVE ISSTEAM
#endif

#ifndef CAP_SHMUP_GOOD
#define CAP_SHMUP_GOOD (!ISMOBWEB)
#endif

#ifndef CAP_COMPLEX2
#define CAP_COMPLEX2 (!ISMINI)
#endif

#ifndef CAP_RACING
#define CAP_RACING (!ISMOBWEB && !ISMINI)
#endif

#if ISMOBILE
#define EXTRALICENSE "\n\nHyperRogue soundtrack under the Creative Commons BY-SA 3.0 license, http://creativecommons.org/licenses/by-sa/3.0/\nCrossroads, Graveyard, Land of Eternal Motion, Hall of Mirrors, Hell, R'Lyeh, Living Caves, Jungle, Desert, Icy Lands by Shawn Parrotte (http://www.shawnparrotte.com)\nCaribbean, Ivory Tower, Ocean, Palace by Will Savino (http://www.willsavino.net/)\n\n\n";
#undef XEXTRALICENSE
#endif

#ifndef HYPERPATH
#define HYPERPATH ""
#endif

#if ISWINDOWS
#define hyper fake_hyper // avoid "hyper" typedef in <_mingw.h>
#define WIN32_LEAN_AND_MEAN // avoid "rad1" macro in <windows.h>
#define NOMINMAX // avoid "min" and "max" macros in <windows.h>
#endif

#include <stdio.h>

#if CAP_SDL
#include <SDL/SDL.h>

#if !ISMAC
#undef main
#endif

#if CAP_SDLAUDIO
#include <SDL/SDL_mixer.h>
#endif

#if CAP_SDLTTF
#include <SDL/SDL_ttf.h>
#endif

#if CAP_SDLGFX
#include <SDL/SDL_gfxPrimitives.h>
#endif

#elif !ISFAKEMOBILE
#define SDLK_F1  (123001)
#define SDLK_F2  (123002)
#define SDLK_F3  (123003)
#define SDLK_F4  (123004)
#define SDLK_F5  (123005)
#define SDLK_F6  (123006)
#define SDLK_F7  (123007)
#define SDLK_F8  (123008)
#define SDLK_F9  (123009)
#define SDLK_F10 (123010)
#define SDLK_ESCAPE (123099)
#define SDLK_F12 (123012)
#define SDLK_HOME (123013)
#define SDLK_LEFT (123014)
#define SDLK_RIGHT (123015)
#define SDLK_END (123016)
#define MIX_MAX_VOLUME 128
#define SDLK_UP (123021)
#define SDLK_DOWN (123022)
#define SDLK_PAGEUP (123023)
#define SDLK_PAGEDOWN (123024)
#define SDLK_RETURN (123025)
#define SDLK_KP1 (123031)
#define SDLK_KP2 (123032)
#define SDLK_KP3 (123033)
#define SDLK_KP4 (123034)
#define SDLK_KP5 (123035)
#define SDLK_KP6 (123036)
#define SDLK_KP7 (123037)
#define SDLK_KP8 (123038)
#define SDLK_KP9 (123039)
#define SDLK_KP_PERIOD (123051)
#define SDLK_KP_MINUS  (123053)
#define SDLK_DELETE (123052)
#define SDLK_DELETE (123052)
#define SDLK_KP_ENTER (123054)
#define SDLK_BACKSPACE (123055)
#define FAKE_SDL
typedef int SDL_Event;
typedef unsigned int Uint32;
#endif

#if ISWEB
extern "C" {
  Uint8 *SDL_GetKeyState(void*);
}
// inline Uint8 *SDL_GetKeyState(void *v) { static Uint8 tab[1024]; return tab; }
#endif


#ifndef CAP_GLEW
#define CAP_GLEW (CAP_GL && !ISMOBILE && !ISMAC && !ISLINUX && !ISWEB)
#endif

#if CAP_GL
#if CAP_GLEW
  #include <GL/glew.h>
#else
  #define GL_GLEXT_PROTOTYPES 1
  #if ISMAC
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
    #include <OpenGL/glext.h>
  #elif ISIOS
    // already included
  #elif ISANDROID
    #include <GLES/gl.h>
    #include <GLES/glext.h>
    #include <GLES2/gl2.h>
    #include <GLES3/gl3.h>
    #define GLES_ONLY
  #else
    #include <GL/gl.h>
    #include <GL/glu.h>
    #include <GL/glext.h>
    #endif
  #endif
#endif

#include <functional>
#include <memory>
#include <cmath>
#include <time.h>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <map>
#include <queue>
#include <stdexcept>
#include <array>
#include <set>
#include <random>
#include <complex>

#ifdef USE_UNORDERED_MAP
#include <unordered_map>
#else
#define unordered_map map
#endif

#include <stdint.h>

#if ISWINDOWS
#include "direntx.h"
#else
#include <dirent.h>
#endif

#if CAP_TEXTURE
#if CAP_SDL_IMG
#include <SDL/SDL_image.h>
#elif CAP_PNG
#include <png.h>
#endif
#endif

#if CAP_SAVE
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#if CAP_TIMEOFDAY
#include <sys/time.h>
#endif

#ifdef BACKTRACE
#include <execinfo.h>
#endif

#if CAP_SDL
union SDL_Event;
#endif

#if ISWINDOWS
#undef hyper  // avoid "hyper" typedef in <_mingw.h>
#endif

#ifndef MINIMIZE_GL_CALLS
#ifdef EMSCRIPTEN
#define MINIMIZE_GL_CALLS 1
#else  
#define MINIMIZE_GL_CALLS 0
#endif
#endif

#define MAXMDIM 4

#ifndef CAP_GEOMETRY
#define CAP_GEOMETRY (!(ISMINI))
#endif

#ifndef CAP_IRR
#define CAP_IRR CAP_GEOMETRY
#endif

#ifndef CAP_GP
#define CAP_GP CAP_GEOMETRY
#endif

#ifndef CAP_ARCM
#define CAP_ARCM CAP_GEOMETRY
#endif

#ifndef CAP_CRYSTAL
#define CAP_CRYSTAL CAP_GEOMETRY
#endif

#ifndef CAP_BT
#define CAP_BT CAP_GEOMETRY
#endif

#ifndef CAP_STARTANIM
#define CAP_STARTANIM (!(ISMINI))
#endif

#ifndef CAP_FIELD
#define CAP_FIELD (!(ISMINI))
#endif
