/*
 * ControlClasses.h
 *
 *  Created on: 15 ??? 2016 ?.
 *      Author: Kreyl
 */

#pragma once

#include "color.h"
#include "font.h"

// ==== Theme ====
struct Theme_t {
    // Button
    PFont_t BtnFont;
    Color_t BtnClrText;
    Color_t BtnClrReleasedTop, BtnClrReleasedBottom, BtnClrPressedTop, BtnClrPressedBottom;
};
extern const Theme_t Theme;

enum Justify_t { jstLeft, jstCenter, jstRight };

#if 1 // ==== Classes ====
enum ControlType_t { ctrlBtn, ctrlTextbox, ctrlChart };

// Parent class for all controls
class Control_t {
protected:
    void FillRect(Color_t ClrTop, Color_t ClrBottom) const;
public:
    ControlType_t Type;
    uint16_t Left, Top, Width, Height;
    const char* Text;
    virtual void Draw() const;
    bool IsInside(int32_t x, int32_t y) const {
        return (x >= Left) and (x <= Left+Width) and (y >= Top) and (y <= Top+Height);
    }
    Control_t(ControlType_t AType,
            uint16_t ALeft, uint16_t ATop, uint16_t AWidth, uint16_t AHeight, const char* AText) :
                Type(AType),
                Left(ALeft), Top(ATop), Width(AWidth), Height(AHeight), Text(AText) {}
};

typedef void (*ftEvtCb)(const Control_t *p);

// ==== Button ====
enum BtnState_t {btnPressed, btnReleased};

class Button_t : public Control_t {
public:
    void Draw() const { Draw(btnReleased); }
    void Draw(BtnState_t State) const;
    ftEvtCb OnRelease;
    void CheckTouchAndAct(int32_t x, int32_t y) const {
        if(IsInside(x, y)) {
            Draw(btnPressed);
        }
    }
    void CheckDetouchAndAct(int32_t x, int32_t y) const {
        if(IsInside(x, y)) {
            Draw(btnReleased);
            if(OnRelease != nullptr) OnRelease((const Control_t*)this);
        }
    }

    Button_t(uint16_t ALeft, uint16_t ATop, uint16_t AWidth, uint16_t AHeight,
            const char* AText,
            ftEvtCb AOnRelease) :
                Control_t(ctrlBtn, ALeft, ATop, AWidth, AHeight, AText),
                OnRelease(AOnRelease) {}
};

// ==== Textbox ====
class Textbox_t : public Control_t {
public:
    void Draw() const;
    PFont_t Font;
    Color_t ClrText, ClrBack;
    Textbox_t(uint16_t ALeft, uint16_t ATop, uint16_t AWidth, uint16_t AHeight,
            const char* AText,
            PFont_t AFont,
            Color_t AClrText, Color_t AClrBack) :
                Control_t(ctrlTextbox, ALeft, ATop, AWidth, AHeight, AText),
                Font(AFont),
                ClrText(AClrText), ClrBack(AClrBack) {}

};

// ==== Chart ====
#define CHART_W_PX      230
#define CHART_W_MS      600000
#define CHART_H_PX      200
#define CHART_TOP       39
#define CHART_LEFT      0
#define CHART_BACK_CLR  clBlack

#define SERIES_CNT      2
#define SERIES_Y_MIN    20
#define SERIES_Y_MAX    220

#define X_SCALE         ((float)(CHART_W_MS / CHART_W_PX))
#define Y_SCALE         ((float)CHART_H_PX / (float)(SERIES_Y_MAX - SERIES_Y_MIN))

struct Point_t {
    float x, y;
};

class Series_t {
private:
    uint32_t Cnt;
    float PrevX;
    uint32_t CurrX;
public:
    Color_t Color;
    void AddPoint(float x, float y);
    void Reset();
    void Draw();
    Series_t() :
        Cnt(0),
        PrevX(0), CurrX(0),
        Color(clWhite) {}
};

class Chart_t {
private:
    uint16_t Left, Top, Width, Height;
    Series_t Series[SERIES_CNT];
public:
    void Draw();
    void Reset();
    void AddPoint(uint32_t SerIndx, float x, float y);
    void AddLineHoriz(float y, Color_t AColor);
    Chart_t(uint16_t ALeft, uint16_t ATop, uint16_t AWidth, uint16_t AHeight,
            Color_t AColor1, Color_t AColor2) :
        Left(ALeft), Top(ATop), Width(AWidth), Height(AHeight)
        {
            Series[0].Color = AColor1;
            Series[1].Color = AColor2;
        }
};

class Page_t {
public:
    const Control_t **Controls;
    uint32_t CtrlCnt;
    void Draw() const {
        for(uint32_t i=0; i<CtrlCnt; i++) {
            const Control_t *PCtrl = Controls[i];
            PCtrl->Draw();
        } // for
    }

    void ProcessTouch(int32_t x, int32_t y) const {
        for(uint32_t i=0; i<CtrlCnt; i++) {
            if(Controls[i]->Type == ctrlBtn) {
                ((Button_t*)Controls[i])->CheckTouchAndAct(x, y);
            }
        }
    }

    void ProcessDetouch(int32_t x, int32_t y) const {
        for(uint32_t i=0; i<CtrlCnt; i++) {
            if(Controls[i]->Type == ctrlBtn) {
                ((Button_t*)Controls[i])->CheckDetouchAndAct(x, y);
            }
        }
    }

    Page_t(const Control_t **AControls, uint32_t ACtrlCnt) :
        Controls(AControls), CtrlCnt(ACtrlCnt) {}
};
#endif
