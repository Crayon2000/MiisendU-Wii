#ifndef ApplicationH
#define ApplicationH
//---------------------------------------------------------------------------

#include <array>
#include <cstdint>

enum class appscreen : uint8_t {
    exitapp,
    ipselection,
    sendinput
};

typedef struct GRRLIB_Font GRRLIB_ttfFont;

class Application {
    public:
        Application();
        Application(Application const&) = delete;
        ~Application();
        Application& operator=(Application const&) = delete;

        bool Run();

    protected:
        void printHeader();
        appscreen screenIpSelection();
        appscreen screenSendInput();

    private:
        GRRLIB_ttfFont *ttf_font;
        appscreen screenId;

        // Sreen IP Selection
        std::array<uint8_t, 4> IP;
        int8_t selected_digit;
        char * IP_ADDRESS;
        uint16_t Port;
        char * msg_connected;
        uint16_t holdTime;
};
//---------------------------------------------------------------------------
#endif
