#!/usr/bin/env bash
#  just for me test
cd "$(dirname "$0")"

sudo rm -rf build

cmake -B build \
      -S . \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/usr

cmake --build build

cd build
sudo cmake --install .
sudo chown $USER:$USER ~/.local/bin/UltralightWebCursor-GUI
sudo chown $USER:$USER ~/.local/share/applications/UltralightWebCursor-GUI.desktop
