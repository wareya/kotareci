#!/bin/bash

# To compile:
# ./test.sh
# To clean (from repository root):
# rm -r obj/
# Re-linking is always done, so there's no need to delete the resultant executable.

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

lua_linker="dependencies/lua-lib/liblua.a"
mix_linker="dependencies/faucmix-lib/libfauxmix.a dependencies/opusfile-lib/libopusfile.a dependencies/opus-lib/libopus.a dependencies/libogg-lib/libogg.a"


if [ "$OSTYPE" == "msys" ]; then
    echo "Platform seems to be windows-like. If not, \$OSTYPE is wrong: it's reporting 'msys'."
    os_cflags=""
    os_linker="--static -static-libstdc++ -static-libgcc -mconsole"
    sdl_linker="-Ldependencies/sdl2-lib $(dependencies/sdl2-bin/sdl2-config --static-libs | sed 's:-lSDL2main::')"
else
    echo "Platform seems to be unix-like. If not, report this bug. (we currently only test for msys)"
    os_cflags="-fPIC"
    os_linker=""
    sdl_linker="-ldl -lpthread -lm -lrt dependencies/sdl2-lib/libSDL2.a"
fi

cflags="$os_cflags -std=c++14 -O3 -Wall -pedantic -Iinclude -Idependencies/sdl2-include $codeset"
linker="$os_linker $sdl_linker $lua_linker $mix_linker"

mflags='-O3 -msse -msse2' # modern amd64 optimizations
cmd="g++ $cflags $mflags"

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
