#pragma once
//---------------------------------------------------------------------------

#include "application.h"

/**
 * ApplicationGc class.
 */
class ApplicationGc : public Application {
    public:
        ApplicationGc();
        ApplicationGc(ApplicationGc const&) = delete;
        ~ApplicationGc();
        ApplicationGc& operator=(ApplicationGc const&) = delete;

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
