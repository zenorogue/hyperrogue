bin_PROGRAMS = hyperrogue
hyperroguedir = $(datadir)/hyperrogue
hyperrogue_SOURCES = hyper.cpp savepng.cpp
hyperrogue_CPPFLAGS = -DFONTDESTDIR=\"$(pkgdatadir)/DejaVuSans-Bold.ttf\" -DMUSICDESTDIR=\"$(pkgdatadir)/hyperrogue-music.txt\" -DSOUNDDESTDIR=\"$(pkgdatadir)/sounds/\"
hyperrogue_CXXFLAGS = -O2 -std=c++11 ${AM_CXXFLAGS}
dist_hyperrogue_DATA = hyperrogue-music.txt DejaVuSans-Bold.ttf

# docdir
dist_doc_DATA = README.md

# musicdir
musicdir=$(datadir)/hyperrogue/music
dist_music_DATA = music/hr3-caves.ogg music/hr3-desert.ogg music/hr3-hell.ogg music/hr3-jungle.ogg music/hr3-mirror.ogg music/hr3-rlyeh.ogg music/hr3-crossroads.ogg music/hr3-graveyard.ogg music/hr3-icyland.ogg music/hr3-laboratory.ogg music/hr3-motion.ogg music/hr-savino-palace.ogg music/hr-savino-caribbean.ogg music/hr-savino-ocean.ogg music/hr-savino-ivory.ogg

# soundsdir
soundsdir=$(datadir)/hyperrogue/sounds
dist_sounds_DATA = sounds/*

noinst_PROGRAMS = langen makeh
langen_SOURCES = langen.cpp 
langen_CXXFLAGS = -O0 -std=c++11 ${AM_CXXFLAGS}
makeh_SOURCES = makeh.cpp
BUILT_SOURCES = language-data.cpp autohdr.h

language-data.cpp: langen
	./langen > language-data.cpp

autohdr.h: makeh *.cpp
	./makeh locations.cpp hyperpoint.cpp geometry.cpp goldberg.cpp init.cpp floorshapes.cpp cell.cpp multi.cpp shmup.cpp pattern2.cpp mapeditor.cpp graph.cpp textures.cpp hprint.cpp language.cpp *.cpp > autohdr.h

#############################
# Platform specific setup
#############################

if MAC
hyperrogue_LDFLAGS = -lSDLmain -framework AppKit -framework OpenGL
endif MAC

if MINGW
nodist_hyperrogue_SOURCES = hyper.res
hyperrogue_CPPFLAGS += -march=native -DWINDOWS
windres_prefix = ${host_cpu}-w64-mingw32-
endif MINGW

#############################
# Arch specific setup
#############################

hyper.res: hyper.rc hr-icon.ico
	${windres_prefix}windres hyper.rc -O coff -o hyper.res
