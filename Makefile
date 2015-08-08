CXXFLAGS ?=

hyper: hyper.cpp graph.cpp hyperpoint.cpp geometry.cpp cell.cpp heptagon.cpp game.cpp classes.cpp polygons.cpp language.cpp language-data.cpp achievement.cpp
	g++ hyper.cpp -o hyper -lSDL -lSDL_ttf -lSDL_mixer -DFHS -Wall -lSDL_gfx ${CXXFLAGS} ${CPPFLAGS} ${LDFLAGS} -lGL -O3

langen: langen.cpp language-cz.cpp language-pl.cpp language-tr.cpp language-ru.cpp
	g++ langen.cpp -o langen

language-data.cpp: langen
	./langen > language-data.cpp
