#!/bin/sh
mkdir -p app/src/main/assets/sounds
cp ../sounds/* app/src/main/assets/sounds/
mkdir -p app/src/main/res/raw/

#for x in caves crossroads desert graveyard hell icyland jungle laboratory mirror rlyeh
#do cp ../music/hr3-$x.ogg app/src/main/res/raw/$x.ogg
#done

# make sure that language-data.cpp is generated
if [ ! -f ../language-data.cpp ]; then
    echo "File language-data.cpp not found! Trying to compile and run langen"
    cd ..
    g++ langen.cpp -o langen
    ./langen > language-data.cpp
    ls -l langen language-data.cpp
    cd hyperroid
fi
