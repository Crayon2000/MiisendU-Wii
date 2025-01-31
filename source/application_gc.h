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

        bool Run() {return true;}

    protected:
        appscreen screenInit() override {return appscreen::exitapp;}
        appscreen screenIpSelection() override {return appscreen::exitapp;} 
        appscreen screenSendInput() override {return appscreen::exitapp;}
};
//---------------------------------------------------------------------------
