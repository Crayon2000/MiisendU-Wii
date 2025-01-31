#pragma once
//---------------------------------------------------------------------------

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <ogc/lwp.h>

/**
 * Application screens.
 */
enum class appscreen : std::uint8_t {
    initapp,    /**< Initialization screen. */
    exitapp,    /**< Exit the application. */
    ipselection,/**< IP selection screen. */
    sendinput   /**< Send input screen. */
};

struct GRRLIB_texImg;

/**
 * Application class.
 */
class Application {
    public:
        Application();
        Application(Application const&) = delete;
        virtual ~Application();
        Application& operator=(Application const&) = delete;

        static void Quit();
        void SetPath(std::string_view path);

    protected:
        virtual appscreen screenInit() = 0;
        virtual appscreen screenIpSelection() = 0;
        virtual appscreen screenSendInput() = 0;

        static bool exitApp;

        GRRLIB_texImg *img_font{nullptr};
        appscreen screenId{appscreen::initapp};
        std::string pathini{};

    private:

};
//---------------------------------------------------------------------------
