#!/bin/sh
set -ex

# Install / update required packages
if [ -z ${GITHUB_WORKSPACE} ]; then
    dkp-pacman --sync --sysupgrade --refresh --needed --noconfirm wii-dev ppc-portlibs
fi

# Install GRRLIB
wget https://github.com/GRRLIB/GRRLIB/archive/refs/heads/master.tar.gz -O - | tar -xz --verbose --directory=/tmp/
(cd /tmp/GRRLIB-*/GRRLIB && make && make install)
rm -rf /tmp/GRRLIB-*

# Install RapidJSON
wget https://github.com/Tencent/rapidjson/archive/refs/heads/master.tar.gz -O - | tar -xz --verbose --directory=/tmp/
cd /tmp/rapidjson-*
/opt/devkitpro/devkitPPC/bin/powerpc-eabi-cmake -DCMAKE_INSTALL_PREFIX=/opt/devkitpro/portlibs/ppc -DCMAKE_BUILD_TYPE=Release \
    -DRAPIDJSON_BUILD_DOC=OFF -DRAPIDJSON_BUILD_EXAMPLES=OFF -DRAPIDJSON_BUILD_TESTS=OFF -DRAPIDJSON_BUILD_CXX11=OFF -DRAPIDJSON_BUILD_CXX17=ON -B build
cmake --build build --target install
rm -rf /tmp/rapidjson-*
