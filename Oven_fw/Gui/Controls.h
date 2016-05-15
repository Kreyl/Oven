/*
 * Controls.h
 *
 *  Created on: 15 ??? 2016 ?.
 *      Author: Kreyl
 */

#pragma once

// ==== Style ====
#define BTN_COLOR1  clWhite
#define BTN_COLOR2  (Color_t){0, 99, 0}

const ButtonStyle_t BtnStyleReleased = {
        &fntVerdana27x27, clWhite,   // Text
        BTN_COLOR1, BTN_COLOR2
};
const ButtonStyle_t BtnStylePressed = {
        &fntVerdana27x27, clWhite,   // Text
        BTN_COLOR2, BTN_COLOR1
};

// Page 0
#define BTN_W           99
#define BTN_H           63
#define BTN_DIST        18

#define BTN_X0          (LCD_W - BTN_W)
#define BTN_Y0          0

#if 1 // ========================== Page 0 =====================================
const Button_t BtnOk {
    BTN_X0, BTN_Y0, BTN_W, BTN_H,
    "OK",
    BtnStyleReleased, BtnStylePressed
};

const Button_t BtnCancel {
    BTN_X0, (BTN_Y0 + BTN_H + BTN_DIST), BTN_W, BTN_H,
    "Cancel",
    BtnStyleReleased, BtnStylePressed
};

const Control_t* __Page0Ctrls[2] = {
        (Control_t*)&BtnOk,
        (Control_t*)&BtnCancel,
};

const Page_t Page0 {
    &__Page0Ctrls[0], 2
};
#endif // Page 0
