/*
 * gui.h
 *
 *  Created on: 13 ??? 2016 ?.
 *      Author: Kreyl
 */

#pragma once

#include "kl_lib.h"
#include "kl_buf.h"

enum Justify_t { jstLeft, jstCenter, jstRight };

#define FRAMEBUFFER_LEN     10000   // 100x100

class Gui_t {
private:
public:
    void Init();
    void DrawPage(uint8_t APage);
};

extern Gui_t Gui;
