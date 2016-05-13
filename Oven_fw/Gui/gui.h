/*
 * gui.h
 *
 *  Created on: 13 ??? 2016 ?.
 *      Author: Kreyl
 */

#pragma once

#include <inttypes.h>



class Gui_t {
private:

public:
    void Init();
    void DrawPage(uint8_t APage);
};

extern Gui_t Gui;
