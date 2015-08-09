bin_PROGRAMS = hyperrogue
hyperroguedir = $(datadir)/hyperrogue
hyperrogue_DATA = music/hr3-caves.ogg music/hr3-graveyard.ogg  music/hr3-jungle.ogg music/hr3-motion.ogg music/hr3-crossroads.ogg music/hr3-hell.ogg music/hr3-laboratory.ogg  music/hr3-rlyeh.ogg music/hr3-desert.ogg music/hr3-icyland.ogg music/hr3-mirror.ogg hyperrogue hyperrogue-music.txt DejaVuSans-Bold.ttf
dist_doc_DATA = README.md

noinst_PROGRAMS = langen
BUILT_SOURCES = language-data.cpp graph.cpp

hyperrogue_SOURCES = hyper.cpp
langen_SOURCES = langen.cpp 
language-data.cpp: langen
	./langen > language-data.cpp
graph.cpp: hyper.cpp
	sed -i 's+./hyperrogue-music.txt+$(pkgdatadir)/hyperrogue-music.txt+g' graph.cpp
	sed -i 's+./TTF_OpenFont("DejaVuSans-Bold.ttf", siz);+TTF_OpenFont("$(pkgdatadir)/DejaVuSans-Bold.ttf", siz);+g' graph.cpp

