/*
 * Controls.h
 *
 *  Created on: 15 ??? 2016 ?.
 *      Author: Kreyl
 */

#pragma once

// Page 0
#define BTN_W           99
#define BTN_H           63
#define BTN_DIST        18

#define BTN_X0          (LCD_W - BTN_W)
#define BTN_Y0          0

#define BTN_FONT        fntVerdana27x27
#define BTN_COLOR_TEXT  clWhite
#define BTN_COLOR_IDLE_TOP  clWhite
#define BTN_COLOR_IDLE_BOT  (Color_t){0, 144, 0}

#if 1 // ========================== Page 0 =====================================
const Button_t BtnOk {
    BTN_X0, BTN_Y0, BTN_W, BTN_H,
    "OK", BTN_FONT, BTN_COLOR_TEXT,
    BTN_COLOR_TOP, BTN_COLOR_BOT
};

const Button_t BtnCancel {
    BTN_X0, (BTN_Y0 + BTN_H + BTN_DIST), BTN_W, BTN_H,
    "Cancel", BTN_FONT, BTN_COLOR_TEXT,
    BTN_COLOR_TOP, BTN_COLOR_BOT
};

const Control_t* __Page0Ctrls[2] = {
        (Control_t*)&BtnOk,
        (Control_t*)&BtnCancel,
};

const Page_t Page0 {
    &__Page0Ctrls[0], 2
};
#endif // Page 0
