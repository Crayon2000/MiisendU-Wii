#include "application.h"
#include "textures.h"
#include "textures_tpl.h"
#include <format>
#include <grrlib.h>

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
