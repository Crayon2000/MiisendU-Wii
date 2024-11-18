#pragma once
//---------------------------------------------------------------------------

#include <array>
#include <string>
#include <cstdint>
#include <ogc/lwp.h>

enum class appscreen : uint8_t {
    initapp,
    exitapp,
    ipselection,
    sendinput
};

struct GRRLIB_texImg;

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
        int8_t selected_digit{0};
        std::string IP_ADDRESS;
        uint16_t Port{4242};
        std::string msg_connected;
        uint16_t holdTime{0};
        std::string pathini;
        uint32_t wait_time_horizontal{0};
        uint32_t wait_time_vertical{0};
};
//---------------------------------------------------------------------------
