#!/usr/bin/env bash

# pkill wineserver64
# killall hmh.exe
pushd /home/trueneu/.wine/drive_c/mingw64/bin

/usr/bin/wine gdbserver.exe localhost:12345 W:\\build\\handmade_win32.exe

popd
