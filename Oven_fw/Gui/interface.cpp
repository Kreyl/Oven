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
// Button
#define BTN_FNT             &fntVerdana27x27
#define BTN_CLR_TXT         clWhite
#define BTN_CLR_REL_TOP     (Color_t){0, 99, 00}
#define BTN_CLR_REL_BOT     (Color_t){0, 27, 00}
#define BTN_CLR_PRS_TOP     (Color_t){0, 27, 00}
#define BTN_CLR_PRS_BOT     (Color_t){0, 99, 00}

#define BTN_MODE_CLR_REL_TOP    (Color_t){0, 99, 99}
#define BTN_MODE_CLR_REL_BOT    (Color_t){0, 27, 27}
#define BTN_MODE_CLR_PRS_TOP    (Color_t){0, 27, 27}
#define BTN_MODE_CLR_PRS_BOT    (Color_t){0, 99, 99}

#if 1 // ========================== Global =====================================
#define TXT_T_H         36
#define TXT_T_W         75
#define TXT_T_Y0        0
#define TXT_TPCB_X0     0
#define TXT_THTR_X0     (TXT_TPCB_X0 + TXT_T_W + 9)
#define TXT_TIME_X0     (TXT_THTR_X0 + TXT_T_W + 9)

static char STPcb[7] = "---";
const Textbox_t txtTPcb {
    TXT_TPCB_X0, TXT_T_Y0, TXT_T_W, TXT_T_H,
    STPcb, &fntVerdana27x27, clRed,     // Text
    clBlack                             // Back
};

static char STHtr[7] = "---";
const Textbox_t txtTHtr {
    TXT_THTR_X0, TXT_T_Y0, TXT_T_W, TXT_T_H,
    STHtr, &fntVerdana27x27, clLightBlue,   // Text
    clBlack                                 // Back
};

static char STime[7] = "---";
const Textbox_t txtTime {
    TXT_TIME_X0, TXT_T_Y0, TXT_T_W, TXT_T_H,
    STime, &fntVerdana27x27, clLightGrey,   // Text
    clBlack                                 // Back
};

void ShowTPcb(float t) {
    kl_bufprint(STPcb, 7, "%.1f", t);
    txtTPcb.Draw();
}
void ShowTHtr(float t) {
    kl_bufprint(STHtr, 7, "%.1f", t);
    txtTHtr.Draw();
}
void ShowTime(uint32_t Tms) {
    kl_bufprint(STime, 7, "%u", Tms/1000);
    txtTime.Draw();
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
    (Color_t){0, 45, 00}                // Back
};

void ShowHeaterOn()  { txtOn.Draw(); }
void ShowHeaterOff() { txtOff.Draw(); }

// Buttons
#define BTN_W           81
#define BTN_H           63
#define BTN_DIST        4

#define BTN_X0          (LCD_W - BTN_W)
#define BTN_Y0          42

extern void OnBtnMode(const Control_t *p);

const Button_t BtnMode {
    BTN_X0, (BTN_Y0 + 2*(BTN_H + BTN_DIST)), BTN_W, BTN_H,
    "Mode", BTN_FNT, BTN_CLR_TXT,
    BTN_MODE_CLR_REL_TOP, BTN_MODE_CLR_REL_BOT, BTN_MODE_CLR_PRS_TOP, BTN_MODE_CLR_PRS_BOT,
    OnBtnMode
};
#endif

#if 1 // ========================== Chart ======================================
Chart_t Chart(0, CHART_TOP, CHART_W_PX, CHART_H_PX, clRed, clLightBlue);
#endif

#if 1 // ======================== Page Profile =================================
// Event callbacks
extern void OnBtnStart(const Control_t *p);
extern void OnBtnStop(const Control_t *p);

const Button_t BtnStart {
    BTN_X0, BTN_Y0, BTN_W, BTN_H,
    "Start", BTN_FNT, BTN_CLR_TXT,
    BTN_CLR_REL_TOP, BTN_CLR_REL_BOT, BTN_CLR_PRS_TOP, BTN_CLR_PRS_BOT,
    OnBtnStart
};

const Button_t BtnStop {
    BTN_X0, (BTN_Y0 + BTN_H + BTN_DIST), BTN_W, BTN_H,
    "Stop", BTN_FNT, BTN_CLR_TXT,
    BTN_CLR_REL_TOP, BTN_CLR_REL_BOT, BTN_CLR_PRS_TOP, BTN_CLR_PRS_BOT,
    OnBtnStop
};

const Control_t* PageProfileCtrls[] = {
        (Control_t*)&BtnStart,
        (Control_t*)&BtnStop,
        (Control_t*)&BtnMode,
        (Control_t*)&txtOff,    // Show OFF txt
        (Control_t*)&txtTPcb,
        (Control_t*)&txtTHtr,
        (Control_t*)&txtTime,
};

const Page_t PageProfile = { PageProfileCtrls, countof(PageProfileCtrls) };
#endif // Page 0

#if 1 // ========================== Page Manual ================================
#if 1 // ==== Fan ====
extern void OnBtnFan(const Control_t *p);

const Button_t BtnFan {
    BTN_X0, (BTN_Y0 + BTN_H + BTN_DIST), BTN_W, BTN_H,
    "Fan", BTN_FNT, BTN_CLR_TXT,
    BTN_CLR_REL_TOP, BTN_CLR_REL_BOT, BTN_CLR_PRS_TOP, BTN_CLR_PRS_BOT,
    OnBtnFan
};

Textbox_t txtFan {
    0, (BTN_Y0 + BTN_H + BTN_DIST + 11), 144, TXT_T_H,
    "Fan OFF", &fntVerdana27x27, clWhite,     // Text
    (Color_t){0, 45, 00}                     // Back
};

void ShowFanStatus(bool IsOn) {
    if(IsOn) txtFan.Text = "Fan ON";
    else txtFan.Text = "Fan OFF";
    txtFan.Draw();
}
#endif // fan

#if 1 // ==== Heater ====
#define HTR_SETUP_Y0        (BTN_Y0 + 4)
#define TXT_SETUP_H         54
#define TXT_SETUP_W         75
#define BTN_PLUSMINUS_WH    TXT_SETUP_H

extern void OnBtnHeater(const Control_t *p);
extern void OnBtnHtrMinus(const Control_t *p);
extern void OnBtnHtrPlus(const Control_t *p);

static char STHtrSetup[7] = "180";
const Textbox_t txtHtrSetup {
    0, HTR_SETUP_Y0, TXT_T_W, TXT_SETUP_H,
    STHtrSetup, &fntVerdana27x27, clWhite,  // Text
    clLightBlue                             // Back
};

void ShowTHtrManual(float t) {
    uint32_t tl = (uint32_t)t;
    kl_bufprint(STHtrSetup, 7, "%u", tl);
    txtHtrSetup.Draw();
}

const Button_t BtnHeater {
    BTN_X0, BTN_Y0, BTN_W, BTN_H,
    "Heatr", BTN_FNT, BTN_CLR_TXT,
    BTN_CLR_REL_TOP, BTN_CLR_REL_BOT, BTN_CLR_PRS_TOP, BTN_CLR_PRS_BOT,
    OnBtnHeater
};

const Button_t BtnTHtrMinus {
    (TXT_T_W + 9), HTR_SETUP_Y0, BTN_PLUSMINUS_WH, BTN_PLUSMINUS_WH,
    "-", BTN_FNT, clWhite,
    clLightBlue, clDarkBlue, clDarkBlue, clLightBlue,
    OnBtnHtrMinus
};
const Button_t BtnTHtrPlus {
    (TXT_T_W + 9 + BTN_PLUSMINUS_WH + 9), HTR_SETUP_Y0, BTN_PLUSMINUS_WH, BTN_PLUSMINUS_WH,
    "+", BTN_FNT, clWhite,
    clLightBlue, clDarkBlue, clDarkBlue, clLightBlue,
    OnBtnHtrPlus
};

#endif

const Control_t* PageManualCtrls[] = {
        (Control_t*)&txtHtrSetup,
        (Control_t*)&BtnHeater,
        (Control_t*)&BtnTHtrMinus,
        (Control_t*)&BtnTHtrPlus,
        (Control_t*)&txtFan,
        (Control_t*)&BtnFan,
        (Control_t*)&BtnMode,
};

const Page_t PageManual = { PageManualCtrls, countof(PageManualCtrls) };
#endif

const Page_t* Page[] = {
        &PageProfile,
        &PageManual,
};
