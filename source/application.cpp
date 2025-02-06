#include "application.h"
#include "textures.h"
#include "textures_tpl.h"
#include <fstream>
#include <format>
#include <grrlib.h>
#include <inipp.h>
#include <network.h>

/**
 * Callbacks will set this to true if called.
 */
bool Application::exitApp = false;

/**
 * Constructor for the Application class.
 */
Application::Application() {
    // Initialise the Graphics & Video subsystem
    GRRLIB_Init();

    img_font = GRRLIB_LoadTextureTPL(textures_tpl, TPL_ID_FONT);
    GRRLIB_InitTileSet(img_font, 8, 20, 32);
}

/**
 * Destructor for the Application class.
 */
Application::~Application()
{
    free(img_font);
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
            scanPads();
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
 * Quit application.
 */
void Application::Quit()
{
    exitApp = true;
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
#if defined(HW_RVL)
        net_deinit();
#endif
        do {
            net_result = net_init();
        } while (net_result == -EAGAIN);
        if (net_result < 0) {
            scanPads();
            if (isHOMEDown() == true) {
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
