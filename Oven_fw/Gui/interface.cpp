/*
 * Controls.cpp
 *
 *  Created on: 23 θώνÿ 2016 γ.
 *      Author: Kreyl
 */

#include "fnt_Verdana27x27.h"
#include "ControlClasses.h"
#include "kl_lib.h"

// ==== Theme ====
const Theme_t Theme = {
        // Button
        &fntVerdana27x27, clWhite,   // Text
        (Color_t){0, 99, 00}, (Color_t){0, 27, 00}, // Released Top/Bottom
        (Color_t){0, 27, 00}, (Color_t){0, 99, 00}, // Pressed Top/Bottom

        // Textbox
        &fntVerdana27x27, clWhite,   // Text
        (Color_t){0, 99, 00},
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
    OnBtnStart
};

const Button_t BtnStop {
    BTN_X0, (BTN_Y0 + BTN_H + BTN_DIST), BTN_W, BTN_H,
    "Stop",
    OnBtnStop
};

const Textbox_t txtOn {
    0, 0, 63, 36,
    "ON", clWhite,  // Text
    clRed           // Back
};
const Textbox_t txtOff {
    0, 0, 63, 36,
    "OFF"
};

const Control_t* __Page0Ctrls[] = {
        (Control_t*)&BtnStart,
        (Control_t*)&BtnStop,
        (Control_t*)&txtOff,    // Show OFF txt
};

const Page_t PagePreheat = { __Page0Ctrls, countof(__Page0Ctrls) };

void ShowHeaterOn()  { txtOn.Draw(); }
void ShowHeaterOff() { txtOff.Draw(); }
#endif // Page 0

const Page_t* Page[] = {
        &PagePreheat
};
