#include "application_gc.h"
#include <ogc/pad.h>
#include <grrlib.h>

/**
 * Constructor for the ApplicationGc class.
 */
ApplicationGc::ApplicationGc() : Application() {
    // Initialise the GC Controllers
    PAD_Init();

    pressHOMEText = "Press the MENU button to exit";
    holdHOMEText = "Hold the MENU button to exit.";
    selectionText = "Press 'A' to confirm";
}

/**
 * Destructor for the ApplicationGc class.
 */
ApplicationGc::~ApplicationGc()
{
}

/**
 * Scan the controllers.
 */
void ApplicationGc::scanPads() {
    PAD_ScanPads(); // Scan the GC Controllers
}

/**
 * Check if the HOME button is held.
 */
bool ApplicationGc::isHOMEHeld() {
    return (PAD_ButtonsHeld(PAD_CHAN0) & PAD_BUTTON_MENU);
}

/**
 * Check if the HOME button is up.
 */
bool ApplicationGc::isHOMEUp() {
    return (PAD_ButtonsUp(PAD_CHAN0) & PAD_BUTTON_MENU);
}

/**
 * Check if the HOME button is down.
 */
bool ApplicationGc::isHOMEDown() {
   return (PAD_ButtonsDown(PAD_CHAN0) & PAD_BUTTON_MENU);
}

/**
 * Check if the selection button is held.
 */
bool ApplicationGc::isSelectionHeld() {
    return (PAD_ButtonsHeld(PAD_CHAN0) & PAD_BUTTON_A);
}

/**
 * Check if the selection button is down.
 */
bool ApplicationGc::isSelectionDown() {
   return (PAD_ButtonsDown(PAD_CHAN0) & PAD_BUTTON_A);
}

/**
 * Check if the up button is held.
 */
bool ApplicationGc::isUpHeld() {
    return (PAD_ButtonsHeld(PAD_CHAN0) & PAD_BUTTON_UP);
}

/**
 * Check if the up button is down.
 */
bool ApplicationGc::isUpDown() {
   return (PAD_ButtonsDown(PAD_CHAN0) & PAD_BUTTON_UP);
}

/**
 * Check if the down button is held.
 */
bool ApplicationGc::isDownHeld() {
    return (PAD_ButtonsHeld(PAD_CHAN0) & PAD_BUTTON_DOWN);
}

/**
 * Check if the down button is down.
 */
bool ApplicationGc::isDownDown() {
   return (PAD_ButtonsDown(PAD_CHAN0) & PAD_BUTTON_DOWN);
}

/**
 * Check if the left button is held.
 */
bool ApplicationGc::isLeftHeld() {
    return (PAD_ButtonsHeld(PAD_CHAN0) & PAD_BUTTON_LEFT);
}

/**
 * Check if the left button is down.
 */
bool ApplicationGc::isLeftDown() {
   return (PAD_ButtonsDown(PAD_CHAN0) & PAD_BUTTON_LEFT);
}

/**
 * Check if the right button is held.
 */
bool ApplicationGc::isRightHeld() {
    return (PAD_ButtonsHeld(PAD_CHAN0) & PAD_BUTTON_RIGHT);
}

/**
 * Check if the right button is down.
 */
bool ApplicationGc::isRightDown() {
   return (PAD_ButtonsDown(PAD_CHAN0) & PAD_BUTTON_RIGHT);
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

/**
 * Get the PAD data.
 * @param pad_data The PAD data.
 */
void ApplicationGc::getPadData(PADData& pad_data) {
    PADStatus padstatus[PAD_CHANMAX];
    PAD_Read(padstatus);

    if(padstatus[PAD_CHAN0].err == PAD_ERR_NONE) {
        pad_data.pad[PAD_CHAN0] = &padstatus[PAD_CHAN0];
    }
    if(padstatus[PAD_CHAN1].err == PAD_ERR_NONE) {
        pad_data.pad[PAD_CHAN1] = &padstatus[PAD_CHAN1];
    }
    if(padstatus[PAD_CHAN2].err == PAD_ERR_NONE) {
        pad_data.pad[PAD_CHAN2] = &padstatus[PAD_CHAN2];
    }
    if(padstatus[PAD_CHAN3].err == PAD_ERR_NONE) {
        pad_data.pad[PAD_CHAN3] = &padstatus[PAD_CHAN3];
    }
}