#!/usr/bin/env bash

mkdir build 2>/dev/null
pushd build

x86_64-w64-mingw32-g++ -ggdb -static -static-libgcc -static-libstdc++ -D HANDMADE_SLOW=1 -D HANDMADE_INTERNAL=1 -I /usr/x86_64-w64-mingw32/include/ ../src/handmade_win32.cpp -lgdi32 -o handmade_win32.exe

popd
