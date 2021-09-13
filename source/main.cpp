/*===========================================
        GRRLIB (GX Version)
        - Template Code -

        Minimum Code To Use GRRLIB
============================================*/
#include <memory>
#include "application.h"

/**
 * Entry point.
 */
int main(int argc, char **argv) {
    auto app = std::make_unique<Application>();
    while(app->Run()) {}
    exit(0); // Use exit() to exit a program
}
