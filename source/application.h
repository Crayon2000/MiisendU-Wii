#pragma once
//---------------------------------------------------------------------------

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <ogc/lwp.h>
#include "pad_to_json.h"

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
        appscreen screenIpSelection();
        appscreen screenSendInput();

        virtual void getPadData(PADData& pad_data) = 0;
        virtual void printHeader() = 0;

        virtual void scanPads() = 0;
        virtual bool isHOMEHeld() = 0;
        virtual bool isHOMEUp() = 0;
        virtual bool isHOMEDown() = 0;
        virtual bool isSelectionHeld() = 0;
        virtual bool isSelectionDown() = 0;
        virtual bool isUpHeld() = 0;
        virtual bool isUpDown() = 0;
        virtual bool isDownHeld() = 0;
        virtual bool isDownDown() = 0;
        virtual bool isLeftHeld() = 0;
        virtual bool isLeftDown() = 0;
        virtual bool isRightHeld() = 0;
        virtual bool isRightDown() = 0;

        GRRLIB_texImg *img_font{nullptr};
        std::string pressHOMEText;
        std::string holdHOMEText;
        std::string selectionText;

    private:
        static bool exitApp;

        appscreen screenId{appscreen::initapp};
        lwp_t pad_data_thread{LWP_THREAD_NULL};

        // Screen IP Selection
        std::array<std::uint8_t, 4> IP{192, 168, 1, 100};
        std::int8_t selected_digit{0};
        std::string ip_address{};
        std::uint16_t port{4242};
        std::string msg_connected;
        std::uint16_t holdTime{0};
        std::string pathini{};
        std::uint32_t wait_time_horizontal{0};
        std::uint32_t wait_time_vertical{0};

        static void *sendPadData(void* arg);
};
//---------------------------------------------------------------------------
