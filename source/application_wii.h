#pragma once
//---------------------------------------------------------------------------

#include "application.h"

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
        void getPadData(PADData& pad_data) override;
        void printHeader() override;
        void scanPads() override;
        bool isHOMEHeld() override;
        bool isHOMEUp() override;
        bool isHOMEDown() override;
        bool isSelectionHeld() override;
        bool isSelectionDown() override;
        bool isUpHeld() override;
        bool isUpDown() override;
        bool isDownHeld() override;
        bool isDownDown() override;
        bool isLeftHeld() override;
        bool isLeftDown() override;
        bool isRightHeld() override;
        bool isRightDown() override;
};
//---------------------------------------------------------------------------
