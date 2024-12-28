#!/bin/sh
set -ex

# Install / update required packages
if [ -z ${GITHUB_WORKSPACE} ]; then
    dkp-pacman --sync --sysupgrade --refresh --needed --noconfirm wii-dev ppc-portlibs
fi
