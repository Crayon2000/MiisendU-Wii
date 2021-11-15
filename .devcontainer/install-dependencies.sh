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

# Install fmt library
wget https://github.com/fmtlib/fmt/archive/refs/tags/8.0.1.tar.gz -O - | tar -xz --verbose --directory=/tmp/
cd /tmp/fmt*
mkdir build
cd build
/opt/devkitpro/devkitPPC/bin/powerpc-eabi-cmake -DFMT_TEST=0 -DCMAKE_INSTALL_PREFIX=/opt/devkitpro/portlibs/ppc ..
make install
rm -rf /tmp/fmt-*

# Install IniPP library
wget https://github.com/mcmtroffaes/inipp/archive/refs/tags/1.0.12.tar.gz -O - | tar -xz --verbose --directory=/tmp/
cd /tmp/inipp-*
mkdir build
cd build
/opt/devkitpro/devkitPPC/bin/powerpc-eabi-cmake -DCMAKE_INSTALL_PREFIX=/opt/devkitpro/portlibs/ppc ..
make install
rm -rf /tmp/inipp-*

# Install RapidJSON
wget https://github.com/Tencent/rapidjson/archive/refs/heads/master.tar.gz -O - | tar -xz --verbose --directory=/tmp/
cd /tmp/rapidjson-*
mkdir build
cd build
/opt/devkitpro/devkitPPC/bin/powerpc-eabi-cmake -DCMAKE_INSTALL_PREFIX=/opt/devkitpro/portlibs/ppc -DCMAKE_BUILD_TYPE=Release \
    -D_CMAKE_INSTALL_DIR=/. -DRAPIDJSON_BUILD_DOC=OFF -DRAPIDJSON_BUILD_EXAMPLES=OFF -DRAPIDJSON_BUILD_TESTS=OFF -DRAPIDJSON_BUILD_CXX11=OFF -DRAPIDJSON_BUILD_CXX17=ON ..
make install
rm -rf /tmp/rapidjson-*
