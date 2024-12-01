#pragma once
//---------------------------------------------------------------------------

#include <array>
#include <cstdint>
#include <string>
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
        ~Application();
        Application& operator=(Application const&) = delete;

        bool Run();
        void SetPath(std::string_view path);

    protected:
        void printHeader();
        appscreen screenInit();
        appscreen screenIpSelection();
        appscreen screenSendInput();

    private:
        GRRLIB_texImg *img_font{nullptr};
        appscreen screenId{appscreen::initapp};
        lwp_t pad_data_thread{LWP_THREAD_NULL};

        // Sreen IP Selection
        std::array<uint8_t, 4> IP{192, 168, 1, 100};
        std::int8_t selected_digit{0};
        std::string IP_ADDRESS;
        std::uint16_t Port{4242};
        std::string msg_connected;
        std::uint16_t holdTime{0};
        std::string pathini;
        std::uint32_t wait_time_horizontal{0};
        std::uint32_t wait_time_vertical{0};
};
//---------------------------------------------------------------------------
