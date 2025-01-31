#include "application_gc.h"
#include <grrlib.h>

/**
 * Constructor for the ApplicationGc class.
 */
ApplicationGc::ApplicationGc() : Application() {
}

/**
 * Destructor for the ApplicationGc class.
 */
ApplicationGc::~ApplicationGc()
{
}

/**
 * Print Header.
 */
void ApplicationGc::printHeader() {
    constexpr char logo1[] = R"( __  __ _ _                 _ _   _    ___  ___ )";
    constexpr char logo2[] = R"(|  \/  (_|_)___ ___ _ _  __| | | | |  / __|/ __|)";
    constexpr char logo3[] = R"(| |\/| | | (_-</ -_) ' \/ _` | |_| | | (_ | (__ )";
    constexpr char logo4[] = R"(|_|  |_|_|_/__/\___|_||_\__,_|\___/   \___|\___| v0.0.1)";

    GRRLIB_Printf(10, 10 + (15 * 1), img_font, 0xFFFFFFFF, 1, logo1);
    GRRLIB_Printf(10, 10 + (15 * 2), img_font, 0xFFFFFFFF, 1, logo2);
    GRRLIB_Printf(10, 10 + (15 * 3), img_font, 0xFFFFFFFF, 1, logo3);
    GRRLIB_Printf(10, 10 + (15 * 4), img_font, 0xFFFFFFFF, 1, logo4);
}
