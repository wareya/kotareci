#!/bin/bash


source=(
 "src/bengine.cpp"
 "src/components.cpp"
 "src/entity.cpp"
 "src/input.cpp"
 "src/maps.cpp"
 "src/benetnasch.cpp"
 "src/physics.cpp"
 "src/quadtree.cpp"
 "src/btime.cpp"
 "src/blib/sdlhelpers.cpp"
 "src/blib/bmath.cpp"
 "src/components/backgrounddrawable.cpp"
 "src/components/boxdrawable.cpp"
 "src/components/bullet.cpp"
 "src/components/character.cpp"
 "src/components/primitives.cpp"
 "src/components/componentlists.cpp"
 "src/components/player.cpp"
 "src/physics/characters.cpp"
 "src/physics/bullets.cpp"
 "src/physics/subroutines.cpp"
 "src/bootclient.cpp"
 "src/client/clientdata.cpp"
 "src/client/scripting.cpp"
 "src/client/textinput.cpp"
 "src/client/think.cpp"
 "src/rendering/drawbackground.cpp"
 "src/rendering/drawboxes.cpp"
 "src/rendering/drawbullets.cpp"
 "src/rendering/drawcharacterdebug.cpp"
 "src/rendering/drawrotatetextured.cpp"
 "src/rendering/drawscreentext.cpp"
 "src/rendering/drawtextured.cpp"
 "src/rendering/drawanimatedtextured.cpp"
 "src/rendering/drawspeedometer.cpp"
 "src/rendering/drawtextwindows.cpp"
 "src/rendering.cpp"
 "src/components/textwindow.cpp"
 "src/components/textureddrawable.cpp"
 "src/components/animatedtextureddrawable.cpp"
 "src/components/rotatingtextureddrawable.cpp"
 "src/samples.cpp")

codeset=" -DCLIENT"
mkdir -p obj
mkdir -p obj/blib
mkdir -p obj/physics
mkdir -p obj/rendering
mkdir -p obj/components
mkdir -p obj/client

if [ "$OSTYPE" == "msys" ]; then
    executable="kotareci.exe"
else
    executable="kotareci.out"
fi

if [ "$OSTYPE" == "msys" ]; then
    echo "Platform seems to be windows. Report this bug if this is not the case."
    
    forceinclude="`sdl2-config --prefix`"
    sdliflags="`sdl2-config --cflags`"
    sdllflags="`sdl2-config --static-libs` -lSDL2_image"
    cflags="-std=c++14 -Wall -pedantic -Iinclude $sdliflags -I${forceinclude}/include"
    linker="-L /usr/lib -static -static-libstdc++ -static-libgcc $sdllflags -mconsole -mwindows"

    if hash sdl2-config; then
        cat /dev/null;
    else
        echo "Could not find sdl2-config. Is SDL2 installed correctly? Aborting."
        exit 1
    fi

    echo ""
    echo "Checking sdl2-config --prefix: ${forceinclude}"
    if [ ! -f "${forceinclude}/lib/libSDL2.a" ]; then
        echo "sdl2-config prefix does not seem to be valid: edit sdl2-config."
        echo "Aborting."
        exit 1
    fi
    echo "Looks okay."
    echo "Also, if you get an 'XCClinker' error, remove that flag from sdl2_config."
    echo ""
else
    echo "Platform seems to be linux. If not, $OSTYPE is wrong."
    
    if hash pkg-config 2>/dev/null; then # prefer pkg-config to sdl2-config
        echo "Using pkg-config for SDL2 compiler flags."
        sdliflags="`pkg-config --cflags sdl2`"
        sdllflags="`pkg-config --libs sdl2` -lSDL2_image"
        cflags="-std=c++14 -Wall -pedantic -Iinclude $sdliflags"
        linker="-L /usr/lib $sdllflags"
    else
        forceinclude="`sdl2-config --prefix`" # avoid unfortunate packing mistake
        sdliflags="`sdl2-config --cflags`"
        sdllflags="`sdl2-config --libs` -lSDL2_image"
        cflags="-fPIC -std=c++14 -Wall -pedantic -Iinclude $sdliflags -I${forceinclude}/include"
        linker="-L /usr/lib $sdllflags"
        if hash sdl2-config; then
            cat /dev/null;
        else
            echo "Could not find sdl2-config. Is SDL2 installed correctly? Aborting."
            exit 1
        fi
        
        echo ""
        echo "Checking sdl2-config --prefix: ${forceinclude}"
        if [ ! -f "${forceinclude}/lib/libSDL2.so" ]; then
            echo "sdl2-config prefix does not seem to be valid: edit sdl2-config."
            echo "Aborting."
            exit 1
        fi
        echo "Looks okay."
    fi
fi

cflags+="$codeset"

#TODO: DETECT
linker+=' -llua -Wl,-R. -L. '
if [ "$OSTYPE" == "msys" ]; then
    linker+='fauxmix.dll'
else
    linker+='fauxmix.so'
fi
#end todo

cmd="g++ $cflags"

if [ $OSTYPE == "msys" ]; then
    console="-mconsole"
fi

#options
dflags="-O0 -g -ggdb $console -D B_NET_DEBUG_CONNECTION"
ddflags="-O0 -g -ggdb $console -D B_NET_DEBUG_CONNECTION -D B_NET_DEBUG_MISTAKES"
dddflags="-O0 -g -ggdb $console -D B_NET_DEBUG_CONNECTION -D B_NET_DEBUG_PRINTPACK"

fflags='-O3'
mflags='-O3 -msse -msse2' # modern x86 optimizations
iflags='-O3 -msse -msse2 -mssse3 -msse4.1' # aggressive intel x86 optimizations
aflags='-O3 -msse -msse2 -msse2avx' # aggressive amd x86 optimizations

tflags='-D TESTS=1'
pflags='-O3 -D B_FRAMELIMIT_DISABLE -D B_DEBUG_FRAMESONLY -D B_DEBUG_COREFRAMES '
zflags='-O3 -D B_FRAMELIMIT_DISABLE -D B_DEBUG_NORENDER '
wflags='-O3 -D B_FRAMELIMIT_DISABLE'

if [ "$1" == "-d" ]; then
    cmd="$cmd $dflags"
elif [ "$1" == "-dd" ]; then
    cmd="$cmd $ddflags"
elif [ "$1" == "-ddd" ]; then
    cmd="$cmd $dddflags"
elif [ "$1" == "-f" ]; then
    cmd="$cmd $fflags"
elif [ "$1" == "-m" ]; then
    cmd="$cmd $mflags"
elif [ "$1" == "-i" ]; then
    cmd="$cmd $iflags"
elif [ "$1" == "-a" ]; then
    cmd="$cmd $aflags"
elif [ "$1" == "-p" ]; then
    cmd="$cmd $pflags"
elif [ "$1" == "-t" ]; then
    cmd="$cmd $tflags"
elif [ "$1" == "-z" ]; then
    cmd="$cmd $zflags"
elif [ "$1" == "-w" ]; then
    cmd="$cmd $wflags"
elif [ "$1" == "-s" ]; then
    cmd="$cmd ${@:2}";
elif [ "$1" == "-c" ]; then
    cmd="$cmd ${@:2}";
fi

objects=""

for i in "${source[@]}"
do
    if [ "$codeset" == " -DSERVER" ]; then
        obj="`echo $i | sed 's-src/-srvobj/-g' | sed 's-.cpp-.o-g'`"
    else
        obj="`echo $i | sed 's-src/-obj/-g' | sed 's-.cpp-.o-g'`"
    fi
    deps=($(gcc -std=c++14 -MM $i | sed -e 's/^\w*.o://' | tr '\n' ' ' | sed -e 's/\\//g' | sed 's/ \+//' | sed 's/ \+/\n/g'))
    for j in "${deps[@]}"
    do
        if test $j -nt $obj; then
            echo "$cmd -c $i -o $obj"
            $cmd -c $i -o $obj
			break
        fi
    done
    objects="$objects $obj"
done
echo "g++ -o $executable $objects $linker"
g++ -o $executable $objects $linker

echo "done"
