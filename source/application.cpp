#include "application.h"
#include "textures.h"
#include "textures_tpl.h"
#include "udp.h"
#include "pad_to_json.h"
#include <atomic>
#include <cstdio>
#include <fstream>
#include <thread>
#include <format>
#include <grrlib.h>
#include <inipp.h>
#include <cstdlib>
#include <wiiuse/wpad.h>
#include <ogc/pad.h>
#include <network.h>

/**
 * Size of the send data stack.
 */
constexpr u32 STACKSIZE = 1024 * 4;

/**
 * Send data stack.
 */
static u8 send_data_stack[STACKSIZE] ATTRIBUTE_ALIGN(8);

/**
 * Whether pad data are being sent.
 */
static std::atomic<bool> running{true};

/**
 * Callbacks will set this to true if called.
 */
static bool exitApp = false;

/**
 * Wait time in frames.
 */
static constexpr std::uint8_t wait_time = 14;

/**
 * Callback for the reset button on the Wii.
 */
static void WiiResetPressed([[maybe_unused]] std::uint32_t irq, [[maybe_unused]] void* ctx)
{
    exitApp = true;
}

/**
 * Callback for the power button on the Wii.
 */
static void WiiPowerPressed()
{
    exitApp = true;
}

/**
 * Constructor for the Application class.
 */
Application::Application() {
    // Initialise the Graphics & Video subsystem
    GRRLIB_Init();

    // Initialise the Wii Remotes and GC Controllers
    WPAD_Init();
    PAD_Init();

    // Register callbacks
    SYS_SetResetCallback(WiiResetPressed);
    SYS_SetPowerCallback(WiiPowerPressed);

    img_font = GRRLIB_LoadTextureTPL(textures_tpl, TPL_ID_FONT);
    GRRLIB_InitTileSet(img_font, 8, 20, 32);
}

/**
 * Destructor for the Application class.
 */
Application::~Application()
{
    free(img_font);
    WPAD_Shutdown();
    net_deinit();
    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB
}

/**
 * Run.
 * @return Returns true if application should keep running.
 */
bool Application::Run()
{
    bool return_value = true;

    // Check if the Wii buttons were pressed
    if(exitApp == true)
    {   // Exit the application
        screenId = appscreen::exitapp;
    }

    switch(screenId)
    {
        case appscreen::initapp:
            screenId = screenInit();
            return true;
        case appscreen::ipselection:
            WPAD_ReadPending(WPAD_CHAN_ALL, nullptr); // Scan the Wii remotes
            PAD_ScanPads(); // Scan the GC Controllers
            screenId = screenIpSelection();
            break;
        case appscreen::sendinput:
            screenId = screenSendInput();
            break;
        case appscreen::exitapp:
            [[fallthrough]];
        default:
            GRRLIB_FillScreen(0x000000FF);
            return_value = false;
            break;
    }

    GRRLIB_Render(); // Render the frame buffer to the TV

    return return_value;
}

/**
 * Set application path.
 * @param path The path to set.
 */
void Application::SetPath(std::string_view path) {
    const auto pos = path.find_last_of('/');
    std::string_view tmp = path.substr(0, pos + 1);
    if(tmp.empty() == false) {
        pathini = std::format("{}settings.ini", tmp);
    }
}

/**
 * Print Header.
 */
void Application::printHeader() {
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
 * Initialization screen.
 * @return Returns the appscreen to use next.
 */
appscreen Application::screenInit() {
    static std::uint8_t print_count = 1;

    // Print loading screen
    GRRLIB_FillScreen(0x000000FF);
    printHeader();
    GRRLIB_Printf(10, 100 + (15 * 5), img_font, 0xFFFFFFFF, 1, "Initializing...");
    GRRLIB_Render();

    if(print_count++ < 2) {
        // Make sure both frame buffers are filled
        return appscreen::initapp;
    }

    // Init network
    s32 net_result = -1;
    while (net_result < 0) {
        net_deinit();
        do {
            net_result = net_init();
        } while (net_result == -EAGAIN);
        if (net_result < 0) {
            WPAD_ReadPending(WPAD_CHAN_ALL, nullptr);
            if (WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_HOME) {
                return appscreen::exitapp;
            }

            printHeader();
            GRRLIB_Printf(10, 100 + (15 * 5), img_font, 0xFFFFFFFF, 1, "Network initialization failed, retrying...");
            GRRLIB_Render();
        }
    }

    // Load default IP address
    bool ip_loaded = false;
    if (pathini.empty() == false) {
        port = 4242;
        if (std::ifstream is(pathini); is.good() == true) {
            std::string ipaddress;
            inipp::Ini<char> ini;
            ini.parse(is);
            inipp::extract(ini.sections["server"]["port"], port);
            inipp::extract(ini.sections["server"]["ipaddress"], ipaddress);
            is.close();
            if(struct in_addr addr; inet_aton(ipaddress.c_str(), &addr) > 0) {
                IP = std::bit_cast<std::array<uint8_t, 4>>(addr.s_addr);
                ip_loaded = true;
            }
        }
    }
    if (ip_loaded == false) {
        const std::uint32_t ip = net_gethostip();
        IP[0] = static_cast<std::uint8_t>((ip >> 24) & 0xFF);
        IP[1] = static_cast<std::uint8_t>((ip >> 16) & 0xFF);
        IP[2] = static_cast<std::uint8_t>((ip >>  8) & 0xFF);
        IP[3] = static_cast<std::uint8_t>((ip >>  0) & 0xFF);
    }

    return appscreen::ipselection;
}

/**
 * Send pad data to UDP.
 * @param arg Unused.
 * @return Returns the appscreen to use next.
 */
static void *sendPadData([[maybe_unused]] void *arg) {
    while(running == true) {
        for(s32 i = WPAD_CHAN_0; i < WPAD_MAX_WIIMOTES; ++i) {
            WPAD_ReadPending(i, nullptr);
        }
        PADStatus padstatus[PAD_CHANMAX];
        PAD_Read(padstatus);

        PADData pad_data;
        memset(&pad_data, 0, sizeof(PADData));

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

        // Transform to JSON
        auto msg_data = pad_to_json(pad_data);

        // Send the message
        udp_print(msg_data.c_str());

        // Wait for while
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }

    udp_deinit();

    return nullptr;
}

/**
 * IP selection screen.
 * @return Returns the appscreen to use next.
 */
appscreen Application::screenIpSelection() {
    WPADData *wpad_data0 = WPAD_Data(WPAD_CHAN_0);

    // If [HOME] was pressed on the first Wii Remote, break out of the loop
    if (wpad_data0->btns_d & WPAD_BUTTON_HOME) {
        return appscreen::exitapp;
    }
    if (wpad_data0->btns_d & WPAD_BUTTON_A) {
        // Get IP Address (without spaces)
        ip_address = std::format("{}.{}.{}.{}", IP[0], IP[1], IP[2], IP[3]);

        // Output the IP address
        msg_connected = std::format("Connected to {}:{}", ip_address, port);

        // Initialize the UDP connection
        udp_init(ip_address, port);

        if(LWP_CreateThread(&pad_data_thread, sendPadData, this, send_data_stack, STACKSIZE, 80) < 0) {
            return appscreen::ipselection;
        }

        return appscreen::sendinput;
    }

    if (wpad_data0->btns_h & WPAD_BUTTON_LEFT  && selected_digit > 0) {
        if (wpad_data0->btns_d & WPAD_BUTTON_LEFT || wait_time_horizontal++ > wait_time) {
            selected_digit--;
            wait_time_horizontal = 0;
        }
    }
    if (wpad_data0->btns_h & WPAD_BUTTON_RIGHT && selected_digit < 3) {
        if (wpad_data0->btns_d & WPAD_BUTTON_RIGHT || wait_time_horizontal++ > wait_time) {
            selected_digit++;
            wait_time_horizontal = 0;
        }
    }
    if (wpad_data0->btns_h & WPAD_BUTTON_UP) {
        if (wpad_data0->btns_d & WPAD_BUTTON_UP || wait_time_vertical++ > wait_time) {
            IP[selected_digit] = (IP[selected_digit] < 255) ? (IP[selected_digit] + 1) : 0;
            wait_time_vertical = 0;
        }
    }
    if (wpad_data0->btns_h & WPAD_BUTTON_DOWN) {
        if (wpad_data0->btns_d & WPAD_BUTTON_DOWN || wait_time_vertical++ > wait_time) {
            IP[selected_digit] = (IP[selected_digit] >   0) ? (IP[selected_digit] - 1) : 255;
            wait_time_vertical = 0;
        }
    }

    printHeader();

    GRRLIB_Printf(10, 100 + (15 * 5), img_font, 0xFFFFFFFF, 1,
        "Please insert your computer's IP address below");
    GRRLIB_Printf(10, 100 + (15 * 6), img_font, 0xFFFFFFFF, 1,
        "(use the DPAD to edit the IP address)");

    GRRLIB_Printf(10 + (4.0f * 8.0f * selected_digit), 100 + (15 * 8), img_font, 0xFFFFFFFF, 1,
        "vvv");

    const std::string ip_str = std::format("{:3d}.{:3d}.{:3d}.{:3d}", IP[0], IP[1], IP[2], IP[3]);
    GRRLIB_Printf(10, 100 + (15 * 9), img_font, 0xFFFFFFFF, 1,
        ip_str.c_str());

    GRRLIB_Printf(10, 100 + (15 * 15), img_font, 0xFFFFFFFF, 1,
        "Press 'A' to confirm");
    GRRLIB_Printf(10, 100 + (15 * 16), img_font, 0xFFFFFFFF, 1,
        "Press the HOME button to exit");

    // Stay on this screen
    return appscreen::ipselection;
}

/**
 * Send input screen.
 * @return Returns the appscreen to use next.
 */
appscreen Application::screenSendInput() {
    WPADData *wpad_data0 = WPAD_Data(WPAD_CHAN_0);

    // Check for exit signal
    if (wpad_data0->btns_h & WPAD_BUTTON_HOME && ++holdTime > 240) {
        running = false;
        LWP_JoinThread(pad_data_thread, nullptr);

        // Save settings to file
        if (pathini.empty() == false) {
            if (std::ofstream os(pathini); os.good() == true) {
                inipp::Ini<char> ini;
                const inipp::Ini<char>::Section server_section = {
                    {"port", std::to_string(port)},
                    {"ipaddress", ip_address},
                };
                ini.sections.emplace("server", server_section);
                ini.generate(os);
                os.close();
            }
        }

        return appscreen::exitapp;
    }
    if (wpad_data0->btns_u & WPAD_BUTTON_HOME) {
        holdTime = 0;
    }

    printHeader();

    GRRLIB_Printf(10, 100 + (15 * 5), img_font, 0xFFFFFFFF, 1,
        msg_connected.c_str());
    GRRLIB_Printf(10, 100 + (15 * 7), img_font, 0xFFFFFFFF, 1,
        "Remember the program will not work without");
    GRRLIB_Printf(10, 100 + (15 * 8), img_font, 0xFFFFFFFF, 1,
        "UsendMii running on your computer.");
    GRRLIB_Printf(10, 100 + (15 * 9), img_font, 0xFFFFFFFF, 1,
        "You can get UsendMii from http://wiiubrew.org/wiki/UsendMii");
    GRRLIB_Printf(10, 100 + (15 * 16), img_font, 0xFFFFFFFF, 1,
        "Hold the HOME button to exit.");

    // Stay on this screen
    return appscreen::sendinput;
}
