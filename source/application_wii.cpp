#include "application_wii.h"
#include <wiiuse/wpad.h>
#include <ogc/pad.h>
#include <grrlib.h>

/**
 * Callback for the reset button on the Wii.
 */
static void WiiResetPressed([[maybe_unused]] std::uint32_t irq, [[maybe_unused]] void* ctx)
{
    Application::Quit();
}

/**
 * Callback for the power button on the Wii.
 */
static void WiiPowerPressed()
{
    Application::Quit();
}

/**
 * Constructor for the ApplicationWii class.
 */
ApplicationWii::ApplicationWii() : Application() {
    // Initialise the Wii Remotes and GC Controllers
    WPAD_Init();
    PAD_Init();

    // Register callbacks
    SYS_SetResetCallback(WiiResetPressed);
    SYS_SetPowerCallback(WiiPowerPressed);

    pressHOMEText = "Press the HOME button to exit";
    holdHOMEText = "Hold the HOME button to exit.";
    selectionText = "Press 'A' to confirm";
}

/**
 * Destructor for the ApplicationWii class.
 */
ApplicationWii::~ApplicationWii()
{
    WPAD_Shutdown();
}

/**
 * Scan the controllers.
 */
void ApplicationWii::scanPads() {
    WPAD_ReadPending(WPAD_CHAN_ALL, nullptr); // Scan the Wii remotes
    PAD_ScanPads(); // Scan the GC Controllers
}

/**
 * Check if the HOME button is held.
 */
bool ApplicationWii::isHOMEHeld() {
    return (WPAD_ButtonsHeld(WPAD_CHAN_0) & WPAD_BUTTON_HOME);
}

/**
 * Check if the HOME button is up.
 */
bool ApplicationWii::isHOMEUp() {
    return (WPAD_ButtonsUp(WPAD_CHAN_0) & WPAD_BUTTON_HOME);
}

/**
 * Check if the HOME button is down.
 */
bool ApplicationWii::isHOMEDown() {
   return (WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_HOME);
}

/**
 * Check if the selection button is held.
 */
bool ApplicationWii::isSelectionHeld() {
   return (WPAD_ButtonsHeld(WPAD_CHAN_0) & WPAD_BUTTON_A);
}

/**
 * Check if the selection button is down.
 */
bool ApplicationWii::isSelectionDown() {
   return (WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_A);
}

/**
 * Check if the up button is held.
 */
bool ApplicationWii::isUpHeld() {
    return (WPAD_ButtonsHeld(WPAD_CHAN_0) & WPAD_BUTTON_UP);
}

/**
 * Check if the up button is down.
 */
bool ApplicationWii::isUpDown() {
   return (WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_UP);
}

/**
 * Check if the down button is held.
 */
bool ApplicationWii::isDownHeld() {
    return (WPAD_ButtonsHeld(WPAD_CHAN_0) & WPAD_BUTTON_DOWN);
}

/**
 * Check if the down button is down.
 */
bool ApplicationWii::isDownDown() {
   return (WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_DOWN);
}

/**
 * Check if the left button is held.
 */
bool ApplicationWii::isLeftHeld() {
    return (WPAD_ButtonsHeld(WPAD_CHAN_0) & WPAD_BUTTON_LEFT);
}

/**
 * Check if the left button is down.
 */
bool ApplicationWii::isLeftDown() {
   return (WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_LEFT);
}

/**
 * Check if the right button is held.
 */
bool ApplicationWii::isRightHeld() {
    return (WPAD_ButtonsHeld(WPAD_CHAN_0) & WPAD_BUTTON_RIGHT);
}

/**
 * Check if the right button is down.
 */
bool ApplicationWii::isRightDown() {
   return (WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_RIGHT);
}

/**
 * Print Header.
 */
void ApplicationWii::printHeader() {
    constexpr char logo1[] = R"( __  __ _ _                 _ _   _  __      ___ _ )";
    constexpr char logo2[] = R"(|  \/  (_|_)___ ___ _ _  __| | | | | \ \    / (_|_))";
    constexpr char logo3[] = R"(| |\/| | | (_-</ -_) ' \/ _` | |_| |  \ \/\/ /| | |)";
    constexpr char logo4[] = R"(|_|  |_|_|_/__/\___|_||_\__,_|\___/    \_/\_/ |_|_| v0.0.1)";

    GRRLIB_Printf(10, 10 + (15 * 1), img_font, 0xFFFFFFFF, 1, logo1);
    GRRLIB_Printf(10, 10 + (15 * 2), img_font, 0xFFFFFFFF, 1, logo2);
    GRRLIB_Printf(10, 10 + (15 * 3), img_font, 0xFFFFFFFF, 1, logo3);
    GRRLIB_Printf(10, 10 + (15 * 4), img_font, 0xFFFFFFFF, 1, logo4);
}

/**
 * Get the PAD data.
 * @param pad_data The PAD data.
 */
void ApplicationWii::getPadData(PADData& pad_data) {
    for(s32 i = WPAD_CHAN_0; i < WPAD_MAX_WIIMOTES; ++i) {
        WPAD_ReadPending(i, nullptr);
    }
    PADStatus padstatus[PAD_CHANMAX];
    PAD_Read(padstatus);

    if(WPADData *wpad_data0 = WPAD_Data(WPAD_CHAN_0);
        wpad_data0->err == WPAD_ERR_NONE && wpad_data0->data_present > 0) {
        pad_data.wpad[WPAD_CHAN_0] = wpad_data0;
    }
    if(WPADData *wpad_data1 = WPAD_Data(WPAD_CHAN_1);
        wpad_data1->err == WPAD_ERR_NONE && wpad_data1->data_present > 0) {
        pad_data.wpad[WPAD_CHAN_1] = wpad_data1;
    }
    if(WPADData *wpad_data2 = WPAD_Data(WPAD_CHAN_2);
        wpad_data2->err == WPAD_ERR_NONE && wpad_data2->data_present > 0) {
        pad_data.wpad[WPAD_CHAN_2] = wpad_data2;
    }
    if(WPADData *wpad_data3 = WPAD_Data(WPAD_CHAN_3);
        wpad_data3->err == WPAD_ERR_NONE && wpad_data3->data_present > 0) {
        pad_data.wpad[WPAD_CHAN_3] = wpad_data3;
    }
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
