#pragma once
//---------------------------------------------------------------------------

#include <array>
#include <string>
#include <cstdint>

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
        GRRLIB_texImg *img_font;
        appscreen screenId;

        // Sreen IP Selection
        std::array<uint8_t, 4> IP;
        int8_t selected_digit;
        std::string IP_ADDRESS;
        uint16_t Port;
        std::string msg_connected;
        uint16_t holdTime;
        std::string pathini;
};
//---------------------------------------------------------------------------
