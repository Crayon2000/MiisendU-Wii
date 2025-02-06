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

        bool Run();
        static void Quit();
        void SetPath(std::string_view path);

    protected:
        appscreen screenInit() ;
        virtual appscreen screenIpSelection() = 0;
        virtual appscreen screenSendInput() = 0;
        virtual void printHeader() = 0;
        virtual void scanPads() = 0;
        virtual bool isHOMEHeld() = 0;
        virtual bool isHOMEUp() = 0;
        virtual bool isHOMEDown() = 0;

        static bool exitApp;

        GRRLIB_texImg *img_font{nullptr};
        appscreen screenId{appscreen::initapp};

        // Screen IP Selection
        std::array<std::uint8_t, 4> IP{192, 168, 1, 100};
        std::uint16_t port{4242};
        std::string pathini{};

    private:

};
//---------------------------------------------------------------------------
