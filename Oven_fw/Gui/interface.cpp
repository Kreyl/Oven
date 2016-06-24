/*
 * Controls.cpp
 *
 *  Created on: 23 θώνÿ 2016 γ.
 *      Author: Kreyl
 */

#include "fnt_Verdana27x27.h"
#include "ControlClasses.h"
#include "kl_lib.h"
#include "kl_sprintf.h"

// ==== Theme ====
const Theme_t Theme = {
        // Button
        &fntVerdana27x27, clWhite,   // Text
        (Color_t){0, 99, 00}, (Color_t){0, 27, 00}, // Released Top/Bottom
        (Color_t){0, 27, 00}, (Color_t){0, 99, 00}, // Pressed Top/Bottom
};

#if 1 // ========================== Global =====================================
#define TXT_T_H         36
#define TXT_T_W         72
#define TXT_T_Y0        0
#define TXT_TPCB_X0     0
#define TXT_THTR_X0     (TXT_TPCB_X0 + TXT_T_W + 9)

static char STPcb[7] = "---";
const Textbox_t txtTPcb {
    TXT_TPCB_X0, TXT_T_Y0, TXT_T_W, TXT_T_H,
    STPcb, &fntVerdana27x27, clRed,     // Text
    clBlack                             // Back
};

static char STHtr[7] = "---";
const Textbox_t txtTHtr {
    TXT_THTR_X0, TXT_T_Y0, TXT_T_W, TXT_T_H,
    STHtr, &fntVerdana27x27, (Color_t){90, 90, 255}, // Text
    clBlack                             // Back
};

void ShowTPcb(float t) {
    kl_bufprint(STPcb, 7, "%.1f", t);
    txtTPcb.Draw();
}
void ShowTHtr(float t) {
    kl_bufprint(STHtr, 7, "%.1f", t);
    txtTHtr.Draw();
}

#define TXT_ONOFF_W     63
#define TXT_ONOFF_H     36
#define TXT_ONOFF_X0    (LCD_W - TXT_ONOFF_W)

const Textbox_t txtOn {
    TXT_ONOFF_X0, 0, TXT_ONOFF_W, TXT_ONOFF_H,
    "ON", &fntVerdana27x27, clWhite,    // Text
    clRed                               // Back
};
const Textbox_t txtOff {
    TXT_ONOFF_X0, 0, TXT_ONOFF_W, TXT_ONOFF_H,
    "OFF", &fntVerdana27x27, clWhite,   // Text
    (Color_t){0, 99, 00}                // Back
};

void ShowHeaterOn()  { txtOn.Draw(); }
void ShowHeaterOff() { txtOff.Draw(); }
#endif

#if 1 // ========================== Page 0 =====================================
#define BTN_W           99
#define BTN_H           63
#define BTN_DIST        18

#define BTN_X0          (LCD_W - BTN_W)
#define BTN_Y0          (LCD_H - BTN_H * 2 - BTN_DIST)

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

const Control_t* __Page0Ctrls[] = {
        (Control_t*)&BtnStart,
        (Control_t*)&BtnStop,
        (Control_t*)&txtOff,    // Show OFF txt
        (Control_t*)&txtTPcb,
        (Control_t*)&txtTHtr,
};

const Page_t PagePreheat = { __Page0Ctrls, countof(__Page0Ctrls) };
#endif // Page 0

const Page_t* Page[] = {
        &PagePreheat
};
