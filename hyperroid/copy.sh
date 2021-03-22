#!/bin/sh
mkdir -p app/src/main/assets/sounds
cp ../sounds/* app/src/main/assets/sounds/
mkdir -p app/src/main/res/raw/

#for x in caves crossroads desert graveyard hell icyland jungle laboratory mirror rlyeh
#do cp ../music/hr3-$x.ogg app/src/main/res/raw/$x.ogg
#done

# make sure autohdr.h is generated
# language-data.cpp is a make dependency of autohdr.h, so it's generated as well
if [ ! -f ../autohdr.h ]; then
    echo "generating autohdr.h..."
    LAST_PWD=$(pwd); cd ..
    make autohdr.h
    cd $LAST_PWD
fi

