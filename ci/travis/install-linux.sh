#!/bin/bash

sudo apt-get install -y --force-yes libsdl2-dev gcc-4.9;
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 60 || true;

#wget https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz
#tar xzf emsdk-portable.tar.gz
#rm emsdk-portable.tar.gz
#pushd emsdk_portable
#./emsdk update && ./emsdk install latest && ./emsdk activate latest && source ./emsdk_env.sh
#popd
