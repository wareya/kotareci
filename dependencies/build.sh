#!/bin/bash

echo "Checking and compiling dependencies."
echo ""

mkdir -p sdl2-lib/
mkdir -p sdl2-bin/
mkdir -p sdl2-include/
mkdir -p sdl2-src/build

if [ ! -f sdl2-lib/libSDL2.a ]; then
    echo "Compiling SDL2 ..."
    echo "------------------"
    
    cd sdl2-src/build
    ../configure --enable-input-tslib=no --disable-shared
    make
    
    cp build/.libs/libSDL2.a ../../sdl2-lib/
    if [ "$OSTYPE" == "msys" ]; then
        cp build/.libs/libSDL2.dll.a ../../sdl2-lib/
        cp build/.libs/SDL2.dll ../../sdl2-bin/
    else
        echo "TODO: Don't know what the filename difference is between static and shared SDL on linux."
    fi
    cp sdl2-config ../../sdl2-bin/
    cp include/* ../../sdl2-include/SDL2/
    cd ../
    mkdir -p ../sdl2-include/SDL2
    cp include/* ../sdl2-include/SDL2/
    cd ../
    
    if [ ! -f sdl2-lib/libSDL2.a ]; then
        echo "Failed to compile sdl2. Exiting."
        exit 1
    fi

    echo "----"
    echo "Done"
    echo ""
fi

mkdir -p lua-lib/
mkdir -p lua-bin/

if [ ! -f lua-lib/liblua.a ]; then
    echo "Compiling Lua ..."
    echo "-----------------"
    
    cd lua-src
    mkdir -p src
    uname=$(uname)
    if [ "$OSTYPE" == "msys" ]; then luaplatform="mingw"
    else luaplatform="${uname,,}"
    fi
    echo "Compiling for $luaplatform"
    
    cd src
    mkdir -p build
    cd ../
    mv src/build/*.o src/
    make $luaplatform
    
    cd src
    mv *.o build;   mv *.a build
    if [ "$OSTYPE" == "msys" ]; then mv *.exe build/; mv *.dll build/
    else mv luac build/; mv lua build/; mv *.so build/
    fi
    cd build
    cp liblua.a ../../../lua-lib/
    
    cd ../../../
    
    if [ ! -f lua-lib/liblua.a ]; then
        echo "Failed to compile lua. If this is a platform mis-detection issue (the current logic is just basically lowercase uname), please submit a pull request. Exiting."
        exit 1
    fi
    
    echo "----"
    echo "Done"
    echo ""
fi

mkdir -p libogg-lib/
mkdir -p libogg-bin/
mkdir -p libogg-include/
mkdir -p pkg/

if [ ! -f libogg-lib/libogg.a ]; then
    echo "Compiling libogg ..."
    echo "--------------------"
    cd libogg-src
    
    mkdir -p build
    cd build
    ../configure
    make
    
    cd src/.libs
    cp libogg.a ../../../../libogg-lib/
    cp ../../ogg.pc ../../../../pkg/
    if [ "$OSTYPE" == "msys" ]; then
        cp libogg.dll.a ../../../../libogg-lib/
        cp libogg-0.dll ../../../../libogg-bin/
    else
        cp libogg.la ../../../../libogg-lib/
        cp libogg.so ../../../../libogg-bin/
    fi
    
    cd ../../../../
    mkdir -p libogg-include/ogg/ && cp libogg-src/include/ogg/*.h "$_"
    
    if [ ! -f libogg-lib/libogg.a ]; then
        echo "Failed to compile libogg."
        exit 1
    fi
    
    echo "----"
    echo "Done"
    echo ""
fi

mkdir -p opus-lib/
mkdir -p opus-bin/
mkdir -p opus-include/

if [ ! -f opus-lib/libopus.a ]; then
    echo "Compiling opus ..."
    echo "--------------------"
    cd opus-src
    
    mkdir -p build
    cd build
    ../configure
    make
    
    cd .libs
    cp libopus.a ../../../opus-lib/
    cp ../opus.pc ../../../pkg/
    if [ "$OSTYPE" == "msys" ]; then
        cp libopus.dll.a ../../../opus-lib/
        cp libopus-0.dll ../../../opus-bin/
    else
        cp libopus.la ../../../opus-lib/
        cp libopus.so ../../../opus-bin/
    fi
    
    cd ../../../
    
    cp opus-src/include/*.h opus-include/
    
    if [ ! -f opus-lib/libopus.a ]; then
        echo "Failed to compile opus."
        exit 1
    fi
    
    echo "----"
    echo "Done"
    echo ""
fi

if [ "$OSTYPE" == "msys" ]; then
    pkgconfigexecutable="pkg-config-bin/pkg-config.exe"
else
    pkgconfigexecutable="pkg-config-bin/pkg-config"
fi

mkdir -p pkg-config-bin/

if [ ! -f $pkgconfigexecutable ]; then
    echo "Compiling pkg-config ..."
    echo "------------------------"
    cd pkg-config-src
    
    mkdir -p build
    cd build
    ../configure --with-internal-glib
    make
    
    if [ "$OSTYPE" == "msys" ]; then
        cd .libs
        cp pkg-config.exe ../../../pkg-config-bin/
        cd ../
    else
        cp pkg-config ../../pkg-config-bin/
    fi
    
    cd ../../
    
    if [ ! -f $pkgconfigexecutable ]; then
        echo "Failed to compile pkg-config. $pkgconfigexecutable is missing."
        exit 1
    fi
    
    echo "----"
    echo "Done"
    echo ""
fi

mkdir -p opusfile-lib/
mkdir -p opusfile-include/


if [ ! -f opusfile-lib/libopusfile.a ]; then
    echo "Compiling opusfile ..."
    echo "----------------------"
    
    export PKG_CONFIG="$PWD/$pkgconfigexecutable"
    export PKG_CONFIG_PATH="$PWD/pkg/"
    export C_INCLUDE_PATH="$PWD/libogg-include/:$PWD/opus-include/"
    export LIBRARY_PATH="$PWD/libogg-lib/:$PWD/opus-lib/"
    
    cd opusfile-src
    
    mkdir -p build
    cd build
    ../configure --disable-http
    make
    
    cd .libs
    cp libopusfile.a ../../../opusfile-lib/
    cp ../opusfile.pc ../../../pkg/
    
    cd ../../../
    
    cp opusfile-src/include/*.h opusfile-include/
    
    if [ ! -f opusfile-lib/libopusfile.a ]; then
        echo "Failed to compile opusfile."
        exit 1
    fi
    
    echo "----"
    echo "Done"
    echo ""
fi

mkdir -p faucmix-lib/

if [ ! -f faucmix-lib/libfauxmix.a ]; then
    echo "Compiling mixer faucet ..."
    echo "--------------------------"
    
    export PKG_CONFIG="$PWD/$pkgconfigexecutable"
    export PKG_CONFIG_PATH="$PWD/pkg/"
    export C_INCLUDE_PATH="$PWD/opusfile-include/"
    export LIBRARY_PATH="$PWD/opusfile-lib/"
    
    export CFLAGS=" $IFLAGS $(sdl2-bin/sdl2-config --cflags | sed 's:-Dmain=SDL_main::') $($pkgconfigexecutable --cflags opusfile) -I../sdl2-include -I../opusfile-include -I../libogg-include -I../opus-include "
    
    cd faucmix-src/
    
    if [ "$OSTYPE" == "msys" ]; then
        ./w64ar.sh
        cp libfauxmix.a ../faucmix-lib/
    else
        ./64ar.sh
        cp libfauxmix.a ../faucmix-lib/
    fi
    
    cd ../
    
    if [ ! -f faucmix-lib/libfauxmix.a ]; then
        echo "Failed to compile mixer faucet."
        exit 1
    fi
    
    echo "----"
    echo "Done"
    echo ""
fi
