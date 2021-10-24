# MiisendU Wii

[![Continuous Integration](https://github.com/Crayon2000/MiisendU-Wii/actions/workflows/ci.yml/badge.svg)](https://github.com/Crayon2000/MiisendU-Wii/actions/workflows/ci.yml)

This homebrew for the Wii is a UsendMii client application.
The button states from the Wii Remotes and the GameCube Controllers will be sent to the server.

## Build

Prerequisites:

* Packages in the wii-dev and ppc-portlibs group from devkitpro (or get the *devkitpro/devkitppc:latest* Docker image)

To compile:

```bash
sh .devcontainer/install-dependencies.sh
make
```
