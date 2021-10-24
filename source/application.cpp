#include "application.h"
#include "udp.h"
#include "vpad_to_json.h"
#include <cstdio>
#include <fstream>
#include <fmt/format.h>
#include <grrlib.h>
#include <inipp.h>
#include <cstdlib>
#include <wiiuse/wpad.h>
#include <ogc/pad.h>
#include <network.h>
#include "OxygenMono-Regular_ttf.h"

/**
 * Converts an IPv4 Internet network address in its standard text presentation form into its numeric binary form.
 * @param addrString A string that contains the text representation of the IP address to convert to numeric binary form.
 * @param addrBuf A pointer to a buffer in which to store the numeric binary representation of the IP address.
 * @return If no error occurs, the function returns a value of 1 and the buffer pointed to by the addrBuf parameter contains the binary numeric IP address in network byte order.
 */
static int8_t inet_pton(std::string_view addrString, void *addrBuf) {
    auto a = static_cast<uint8_t *>(addrBuf);
    for (int8_t i = 0; i < 4; ++i) {
        int16_t v;
        int8_t j;
        for (v = j = 0; j < 3 && std::isdigit(addrString[j]); ++j) {
            v = 10 * v + addrString[j] - '0';
        }
        if (j == 0 || (j > 1 && addrString[0] == '0') || v > 255) {
            return 0;
        }
        a[i] = v;
        if (addrString[j] == 0 && i == 3) {
            return 1;
        }
        if (addrString[j] != '.') {
            return 0;
        }
        addrString.remove_prefix(j + 1);
    }
    return 0;
}

/**
 * Constructor for the Application class.
 */
Application::Application() :
    screenId(appscreen::initapp),
    Port(4242),
    holdTime(0)
{
    // Initialise the Graphics & Video subsystem
    GRRLIB_Init();

    // Initialise the Wii Remotes and GC Controllers
    WPAD_Init();
    PAD_Init();

    ttf_font = GRRLIB_LoadTTF(OxygenMono_Regular_ttf, OxygenMono_Regular_ttf_size);

    IP = {192, 168, 1, 100};
    selected_digit = 0;
}

/**
 * Destructor for the Application class.
 */
Application::~Application()
{
    GRRLIB_FreeTTF(ttf_font);
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
    bool returnvalue = true;
    WPAD_ScanPads(); // Scan the Wii remotes
    //PAD_ScanPads(); // Scan the GC Controller

    switch(screenId)
    {
        case appscreen::initapp:
            screenId = screenInit();
            return true;
        case appscreen::ipselection:
            screenId = screenIpSelection();
            break;
        case appscreen::sendinput:
            screenId = screenSendInput();
            break;
        case appscreen::exitapp:
            [[fallthrough]];
        default:
            GRRLIB_FillScreen(0x000000FF);
            returnvalue = false;
            break;
    }

    GRRLIB_Render(); // Render the frame buffer to the TV

    return returnvalue;
}

/**
 * Set application path.
 * @param path The path to set.
 */
void Application::SetPath(std::string_view path) {
    auto const pos = path.find_last_of('/');
    std::string_view tmp = path.substr(0, pos + 1);
    if(tmp.empty() == false) {
        pathini = fmt::format("{}settings.ini", tmp);
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

    GRRLIB_PrintfTTF(10, 10 + (15 * 1), ttf_font, logo1, 13, 0xFFFFFFFF);
    GRRLIB_PrintfTTF(10, 10 + (15 * 2), ttf_font, logo2, 13, 0xFFFFFFFF);
    GRRLIB_PrintfTTF(10, 10 + (15 * 3), ttf_font, logo3, 13, 0xFFFFFFFF);
    GRRLIB_PrintfTTF(10, 10 + (15 * 4), ttf_font, logo4, 13, 0xFFFFFFFF);
}

/**
 * Initialization screen.
 * @return Returns the appscreen to use next.
 */
appscreen Application::screenInit() {
    // Print loading screen
    GRRLIB_FillScreen(0x000000FF);
    printHeader();
    GRRLIB_PrintfTTF(10, 100 + (15 * 5), ttf_font, "Initializing...", 13, 0xFFFFFFFF);
    GRRLIB_Render();

    // Init network
    s32 net_result = -1;
    while (net_result < 0) {
        net_deinit();
        do {
            net_result = net_init();
        } while (net_result == -EAGAIN);
        if (net_result < 0) {
            WPAD_ScanPads();
            if (WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_HOME) {
                return appscreen::exitapp;
            }

            printHeader();
            GRRLIB_PrintfTTF(10, 100 + (15 * 5), ttf_font, "Network initialization failed, retrying...", 13, 0xFFFFFFFF);
            GRRLIB_Render();
        }
    }

    // Load default IP address
    bool ip_loaded = false;
    if (pathini.empty() == false) {
        Port = 4242;
        std::ifstream is(pathini);
        if (is.good() == true) {
            std::string ipaddress;
            inipp::Ini<char> ini;
            ini.parse(is);
            inipp::extract(ini.sections["server"]["port"], Port);
            inipp::extract(ini.sections["server"]["ipaddress"], ipaddress);
            is.close();
            if(inet_pton(ipaddress, &IP) > 0) {
                ip_loaded = true;
            }
        }
    }
    if (ip_loaded == false) {
        uint32_t ip = net_gethostip();
        IP[0] = (ip >> 24) & 0xFF;
        IP[1] = (ip >> 16) & 0xFF;
        IP[2] = (ip >>  8) & 0xFF;
        IP[3] = (ip >>  0) & 0xFF;
    }

    return appscreen::ipselection;
}

/**
 * IP selection screen.
 * @return Returns the appscreen to use next.
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
 * @return Returns the appscreen to use next.
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

        // Save settings to file
        if (pathini.empty() == false) {
            std::ofstream os(pathini);
            if (os.good() == true) {
                inipp::Ini<char> ini;
                ini.sections.emplace("server", (inipp::Ini<char>::Section) {
                    {"port", std::to_string(Port)},
                    {"ipaddress", IP_ADDRESS},
                });
                ini.generate(os);
                os.close();
            }
        }

        return appscreen::exitapp;
    }
    if (wpad_data0->btns_u & WPAD_BUTTON_HOME) {
        holdTime = 0;
    }

    // The string sent to the computer
    std::string msg_data;

    // Transform to JSON
    pad_to_json(pad_data, msg_data);

    // Send the message
    udp_print(msg_data.c_str());

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
