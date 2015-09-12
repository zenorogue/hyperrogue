# Hyperrogue binairy rules
bin_PROGRAMS = hyperrogue
hyperroguedir = $(datadir)/hyperrogue
dist_hyperrogue_DATA = music/hyperrogue-music.txt DejaVuSans-Bold.ttf
dist_doc_DATA = README.md
hyperrogue_SOURCES = hyper.cpp

# Some definitions used in graph.cpp
hyperrogue_CPPFLAGS = -DFONTDESTDIR=\"$(pkgdatadir)/DejaVuSans-Bold.ttf\" -DMUSICDESTDIR=\"$(pkgdatadir)/hyperrogue-music.txt\"

# Musicdir
musicdir=$(datadir)/hyperrogue/music
dist_music_DATA = music/hr3-caves.ogg music/hr3-desert.ogg music/hr3-hell.ogg music/hr3-jungle.ogg music/hr3-mirror.ogg music/hr3-rlyeh.ogg music/hr3-crossroads.ogg music/hr3-graveyard.ogg music/hr3-icyland.ogg music/hr3-laboratory.ogg music/hr3-motion.ogg

music/hyperrogue-music.txt: hyperrogue
	cp hyperrogue-music.txt music/hyperrogue-music.txt
	sed -i 's+music+$(pkgdatadir)/music+g' music/hyperrogue-music.txt

# Langen binary rules
noinst_PROGRAMS = langen
langen_SOURCES = langen.cpp 

# Generation of language-data.cpp
language-data.cpp: langen
	./langen > language-data.cpp

# Local clean rules
clean-local:
	rm -rf language-data.cpp
	rm -rf music/hyperrogue-music.txt

# First target made
BUILT_SOURCES = language-data.cpp graph.cpp

#############################
# Platform specific setup
#############################
windres_prefix=""
if MINGW 
hyperrogue_SOURCES += hyper.res
hyperrogue_CPPFLAGS += ${wincppflags}
#hyperrogue_LDFLAGS = "-static-libgcc -static-libstdc++"
windres_prefix += "${host_cpu}-w64-mingw32-"

endif MINGW

#############################
# Arch specific setup
#############################
hyper.res: hyper.rc hr-icon.ico
	${windres_prefix}windres hyper.rc -O coff -o hyper.res
