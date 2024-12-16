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
