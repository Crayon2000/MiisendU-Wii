#!/bin/sh
set -ex

# Install required packages
dkp-pacman --sync --sysupgrade --refresh --needed --noconfirm wii-dev

# Install GRRLIB
wget https://github.com/GRRLIB/GRRLIB/archive/refs/heads/master.tar.gz -O - | tar -xz --verbose --directory=/tmp/
(cd /tmp/GRRLIB-*/GRRLIB && make && make install)
