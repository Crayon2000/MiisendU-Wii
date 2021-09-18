#!/bin/sh
set -ex

# Install / update required packages
if [ -z ${GITHUB_WORKSPACE} ]; then
    dkp-pacman --sync --sysupgrade --refresh --needed --noconfirm wii-dev
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
