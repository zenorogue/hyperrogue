#!/bin/sh
/usr/lib/emscripten/em++ hyperweb.cpp -O3 -s USE_ZLIB=1 -s LEGACY_GL_EMULATION=1 -s TOTAL_MEMORY=128*1024*1024 -std=c++11 -o hyper.html

