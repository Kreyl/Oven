/*
 * interface.cpp
 *
 *  Created on: 13 ??? 2016 ?.
 *      Author: Kreyl
 */

#include "interface.h"
#include "ILI9341.h"

Interface_t Interface;

ILI9341_t Lcd;

void Interface_t::Init() {
    Lcd.Init();
    Lcd.Cls(clGreen);
}
