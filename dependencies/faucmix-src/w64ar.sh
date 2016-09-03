#!/bin/bash

echo $LFLAGS

cat $(echo src/*.cpp | sed s-src/newmain.cpp--) | \
g++ -std=c++11 -c -x c++ - -Isrc \
$CFLAGS $LFLAGS \
-s -g -ggdb \
-Wall -Wextra -pedantic -Wno-unused -Wno-sign-compare -Wno-return-type -Wfatal-errors \
-fvisibility=hidden \
-o fauxmix.o
#-static -static-libgcc -static-libstdc++
ar rcs libfauxmix.a fauxmix.o
