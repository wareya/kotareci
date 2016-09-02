g++ -std=c++11 `echo src/*.cpp|sed s-src/newmain.cpp--` -Isrc \
`pkg-config opusfile sdl2 --libs --cflags|sed s/-XCClinker//` \
-s -g -ggdb \
-Wall -Wextra -pedantic -Wno-unused -Wno-sign-compare -Wno-return-type -Wfatal-errors \
-static -static-libgcc -static-libstdc++ \
-shared -o fauxmix.dll
