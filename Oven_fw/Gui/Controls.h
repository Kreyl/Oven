/*
 * Controls.h
 *
 *  Created on: 15 ??? 2016 ?.
 *      Author: Kreyl
 */

#pragma once

// ==== Style ====
#define BTN_COLOR1  (Color_t){0, 99, 00}
#define BTN_COLOR2  (Color_t){0, 27, 00}

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
// Event callbacks
extern void OnBtnStart(const Control_t *p);
extern void OnBtnStop(const Control_t *p);

const Button_t BtnStart {
    BTN_X0, BTN_Y0, BTN_W, BTN_H,
    "Start",
    BtnStyleReleased, BtnStylePressed,
    OnBtnStart
};

const Button_t BtnStop {
    BTN_X0, (BTN_Y0 + BTN_H + BTN_DIST), BTN_W, BTN_H,
    "Stop",
    BtnStyleReleased, BtnStylePressed,
    OnBtnStop
};

const Control_t* __Page0Ctrls[2] = {
        (Control_t*)&BtnStart,
        (Control_t*)&BtnStop,
};

const Page_t Page0 {
    &__Page0Ctrls[0], 2
};
#endif // Page 0
