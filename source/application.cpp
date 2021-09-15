#include "application.h"
#include "udp.h"
#include "vpad_to_json.h"
#include <cstdio>
#include <fmt/format.h>
#include <grrlib.h>
#include <stdlib.h>
#include <wiiuse/wpad.h>
#include <ogc/pad.h>
#include "OxygenMono-Regular_ttf.h"

/**
 * Constructor for the Application class.
 */
Application::Application() :
    screenId(appscreen::ipselection),
    Port(4242),
    holdTime(0)
{
    // Initialise the Graphics & Video subsystem
    GRRLIB_Init();

    // Initialise the Wii Remotes and GC Controllers
    WPAD_Init();
    PAD_Init();

    ttf_font = GRRLIB_LoadTTF(OxygenMono_Regular_ttf, OxygenMono_Regular_ttf_size);

    IP = {192, 168, 50, 213};
    selected_digit = 0;
}

/**
 * Destructor for the Application class.
 */
Application::~Application()
{
    GRRLIB_FreeTTF(ttf_font);
    WPAD_Shutdown();
    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB
}

/**
 * Run.
 */
bool Application::Run()
{
    bool returnvalue = true;
    WPAD_ScanPads(); // Scan the Wii remotes
    //PAD_ScanPads(); // Scan the GC Controller

    switch(screenId)
    {
        case appscreen::ipselection:
            screenId = screenIpSelection();
            break;
        case appscreen::sendinput:
            screenId = screenSendInput();
            break;
        default:
            GRRLIB_FillScreen(0x000000FF);
            returnvalue = false;
            break;
    }

    GRRLIB_Render(); // Render the frame buffer to the TV

    return returnvalue;
}

/**
 * Print Header.
 */
void Application::printHeader() {
    constexpr char logo1[] = R"( __  __ _ _                 _ _   _  __      ___ _ )";
    constexpr char logo2[] = R"(|  \/  (_|_)___ ___ _ _  __| | | | | \ \    / (_|_))";
    constexpr char logo3[] = R"(| |\/| | | (_-</ -_) ' \/ _` | |_| |  \ \/\/ /| | |)";
    constexpr char logo4[] = R"(|_|  |_|_|_/__/\___|_||_\__,_|\___/    \_/\_/ |_|_| v0.0.1)";

    GRRLIB_PrintfTTF(10, 10 + (15 * 1), ttf_font, logo1, 13, 0xFFFFFFFF);
    GRRLIB_PrintfTTF(10, 10 + (15 * 2), ttf_font, logo2, 13, 0xFFFFFFFF);
    GRRLIB_PrintfTTF(10, 10 + (15 * 3), ttf_font, logo3, 13, 0xFFFFFFFF);
    GRRLIB_PrintfTTF(10, 10 + (15 * 4), ttf_font, logo4, 13, 0xFFFFFFFF);
}

/**
 * IP selection screen.
 */
appscreen Application::screenIpSelection() {
    // If [HOME] was pressed on the first Wiimote, break out of the loop
    if (WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_HOME) {
        return appscreen::exitapp;
    }
    if (WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_A) {
        // Get IP Address (without spaces)
        IP_ADDRESS = fmt::format("{}.{}.{}.{}", IP[0], IP[1], IP[2], IP[3]);

        // Output the IP address
        msg_connected = fmt::format("Connected to {}:{}", IP_ADDRESS, Port);

        // Initialize the UDP connection
        udp_init(IP_ADDRESS.c_str(), Port);

        return appscreen::sendinput;
    }

    if (WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_LEFT  && selected_digit > 0) {
        selected_digit--;
    }
    if (WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_RIGHT && selected_digit < 3) {
        selected_digit++;
    }
    if (WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_UP) {
        IP[selected_digit] = (IP[selected_digit] < 255) ? (IP[selected_digit] + 1) : 0;
    }
    if (WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_DOWN) {
        IP[selected_digit] = (IP[selected_digit] >   0) ? (IP[selected_digit] - 1) : 255;
    }

    printHeader();

    GRRLIB_PrintfTTF(10, 100 + (15 * 5), ttf_font,
        "Please insert your computer's IP address below", 13, 0xFFFFFFFF);
    GRRLIB_PrintfTTF(10, 100 + (15 * 6), ttf_font,
        "(use the DPAD to edit the IP address)", 13, 0xFFFFFFFF);

    GRRLIB_PrintfTTF(10 + (4 * 8 * selected_digit), 100 + (15 * 8), ttf_font,
        "vvv", 13, 0xFFFFFFFF);

    const std::string IP_str = fmt::format("{:3d}.{:3d}.{:3d}.{:3d}", IP[0], IP[1], IP[2], IP[3]);
    GRRLIB_PrintfTTF(10, 100 + (15 * 9), ttf_font,
        IP_str.c_str(), 13, 0xFFFFFFFF);

    GRRLIB_PrintfTTF(10, 100 + (15 * 15), ttf_font,
        "Press 'A' to confirm", 13, 0xFFFFFFFF);
    GRRLIB_PrintfTTF(10, 100 + (15 * 16), ttf_font,
        "Press the HOME button to exit", 13, 0xFFFFFFFF);

    // Stay on this screen
    return appscreen::ipselection;
}

/**
 * Send input screen.
 */
appscreen Application::screenSendInput() {
    PADStatus padstatus[PAD_CHANMAX];
    PAD_Read(padstatus);

    WPADData *wpad_data0 = WPAD_Data(WPAD_CHAN_0);
    WPADData *wpad_data1 = WPAD_Data(WPAD_CHAN_1);
    WPADData *wpad_data2 = WPAD_Data(WPAD_CHAN_2);
    WPADData *wpad_data3 = WPAD_Data(WPAD_CHAN_3);

    PADData pad_data;
    memset(&pad_data, 0, sizeof(PADData));
    if(wpad_data0->err == WPAD_ERR_NONE) {
        pad_data.wpad[WPAD_CHAN_0] = wpad_data0;
    }
    if(wpad_data1->err == WPAD_ERR_NONE) {
        pad_data.wpad[WPAD_CHAN_1] = wpad_data1;
    }
    if(wpad_data2->err == WPAD_ERR_NONE) {
        pad_data.wpad[WPAD_CHAN_2] = wpad_data2;
    }
    if(wpad_data3->err == WPAD_ERR_NONE) {
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

    // Check for exit signal
    if (wpad_data0->btns_h & WPAD_BUTTON_HOME && ++holdTime > 240) {
        udp_deinit();
        return appscreen::exitapp;
    }
    if (wpad_data0->btns_u & WPAD_BUTTON_HOME) {
        holdTime = 0;
    }

    // The buffer sent to the computer
    char msg_data[1024];

    // Transform to JSON
    pad_to_json(pad_data, msg_data, sizeof(msg_data));

    // Send the message
    udp_print(msg_data);

    printHeader();

    GRRLIB_PrintfTTF(10, 100 + (15 * 5), ttf_font,
        msg_connected.c_str(), 13, 0xFFFFFFFF);
    GRRLIB_PrintfTTF(10, 100 + (15 * 7), ttf_font,
        "Remember the program will not work without", 13, 0xFFFFFFFF);
    GRRLIB_PrintfTTF(10, 100 + (15 * 8), ttf_font,
        "UsendMii running on your computer.", 13, 0xFFFFFFFF);
    GRRLIB_PrintfTTF(10, 100 + (15 * 9), ttf_font,
        "You can get UsendMii from http://wiiubrew.org/wiki/UsendMii", 13, 0xFFFFFFFF);
    GRRLIB_PrintfTTF(10, 100 + (15 * 16), ttf_font,
        "Hold the HOME button to exit.", 13, 0xFFFFFFFF);

    // Stay on this screen
    return appscreen::sendinput;
}
