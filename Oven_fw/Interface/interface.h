/*
 * interface.h
 *
 *  Created on: 22 марта 2015 г.
 *      Author: Kreyl
 */

#pragma once

#include "main.h"

class Interface_t {
private:

public:
    void Init();
    void Reset() {
//        Lcd.Printf(11, 0, VERSION_STRING);
////        Lcd.Printf(9, 1, "Active");
////        Lcd.Printf(8, 2, "duration");
//        Lcd.Printf(0, 0, "Test");
//        Lcd.Symbols(0, 5, LineHorizDouble, 16, 0);
////        for(int i=0; i<5; i++) Lcd.Symbols(6, i, ((i == 2)? LineVertDoubleLeft : LineVertDouble), 1,0);
////        Lcd.Symbols(0, 2, LineHorizDouble, 6, 0);
//        ShowMSns1(0);
//        ShowMSns2(0);
//        ShowLedOff();
    }

//    void Error(const char* msg) { Lcd.PrintfInverted(0, 2, "%S", msg); }
};

extern Interface_t Interface;
