#pragma once
//---------------------------------------------------------------------------

#include "application.h"
#include <array>
#include <cstdint>
#include <string>
#include <ogc/lwp.h>

/**
 * ApplicationWii class.
 */
class ApplicationWii : public Application {
    public:
        ApplicationWii();
        ApplicationWii(ApplicationWii const&) = delete;
        ~ApplicationWii();
        ApplicationWii& operator=(ApplicationWii const&) = delete;

    protected:
        void printHeader();
        appscreen screenInit() override;
        appscreen screenIpSelection() override;
        appscreen screenSendInput() override;
        void scanPads() override;

    private:
        lwp_t pad_data_thread{LWP_THREAD_NULL};

        // Screen IP Selection
        std::array<std::uint8_t, 4> IP{192, 168, 1, 100};
        std::int8_t selected_digit{0};
        std::string ip_address{};
        std::uint16_t port{4242};
        std::string msg_connected;
        std::uint16_t holdTime{0};
        std::uint32_t wait_time_horizontal{0};
        std::uint32_t wait_time_vertical{0};
};
//---------------------------------------------------------------------------
