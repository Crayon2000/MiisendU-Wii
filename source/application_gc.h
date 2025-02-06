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
        void printHeader() override;
        appscreen screenIpSelection() override {return appscreen::exitapp;} 
        appscreen screenSendInput() override {return appscreen::exitapp;}
        void scanPads() override;
        bool isHOMEHeld() override;
        bool isHOMEUp() override;
        bool isHOMEDown() override;

};
//---------------------------------------------------------------------------
