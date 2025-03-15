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
        ~ApplicationGc() override;
        ApplicationGc& operator=(ApplicationGc const&) = delete;

    protected:
        std::span<const std::string_view> getLogo() override;
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

    private:
        static void getPadData(PADData& pad_data);
};
//---------------------------------------------------------------------------
