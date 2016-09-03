#!/bin/bash

# To compile:
# ./test.sh
# To clean (from repository root):
# rm -r obj/
# Re-linking is always done, so there's no need to delete the resultant .exe/.dll file.

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

cd dependencies
./build.sh
cd ../

sdl_linker="-Ldependencies/sdl2-lib $(dependencies/sdl2-bin/sdl2-config --static-libs | sed 's:-L/usr/local/lib :: ; s:-lSDL2main::')"
lua_linker="-Ldependencies/lua-lib -llua"
mix_linker="-Ldependencies/faucmix-lib -lfauxmix -Ldependencies/opusfile-lib -lopusfile -Ldependencies/opus-lib -lopus -Ldependencies/libogg-lib -logg"


if [ "$OSTYPE" == "msys" ]; then
    echo "Platform seems to be windows-like. If not, \$OSTYPE is wrong: it's reporting 'msys'."
    os_cflags=""
    os_linker="-static -static-libstdc++ -static-libgcc -mconsole"
else
    echo "Platform seems to be unix-like. If not, report this bug. (we currently only test for msys)"
    os_cflags="-fPIC"
    os_linker=""
fi

cflags="$os_cflags -std=c++14 -Wall -pedantic -Iinclude -Idependencies/sdl2-include $codeset"
linker="-static $os_linker $sdl_linker $lua_linker $mix_linker"

cmd="g++ $cflags"

#options
dflags="-O0 -g -ggdb -D B_NET_DEBUG_CONNECTION"
ddflags="-O0 -g -ggdb -D B_NET_DEBUG_CONNECTION -D B_NET_DEBUG_MISTAKES"
dddflags="-O0 -g -ggdb -D B_NET_DEBUG_CONNECTION -D B_NET_DEBUG_PRINTPACK"

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
