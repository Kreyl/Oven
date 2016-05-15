/*
 * ControlClasses.h
 *
 *  Created on: 15 ??? 2016 ?.
 *      Author: Kreyl
 */

#pragma once

#include "color.h"
#include "font.h"

enum Justify_t { jstLeft, jstCenter, jstRight };

#if 1 // ==== Classes ====
enum ControlType_t { ctrlBtn };

// Styles
struct ButtonStyle_t {
    PFont_t Font;
    Color_t ClrText;
    Color_t ClrTop, ClrBot;
};

// Parent class for all controls
class Control_t {
public:
    ControlType_t Type;
    uint16_t Left, Top, Width, Height;
    const char* Text;
    virtual void Draw() const;
    virtual void CheckTouchAndAct(int32_t x, int32_t y) const;
    virtual void CheckDetouchAndAct(int32_t x, int32_t y) const;
    bool IsInside(int32_t x, int32_t y) const {
        return (x >= Left) and (x <= Left+Width) and (y >= Top) and (y <= Top+Height);
    }
    Control_t(ControlType_t AType,
            uint16_t ALeft, uint16_t ATop, uint16_t AWidth, uint16_t AHeight, const char* AText) :
                Type(AType),
                Left(ALeft), Top(ATop), Width(AWidth), Height(AHeight), Text(AText) {}
};

typedef void (*ftEvtCb)(const Control_t *p);

enum BtnState_t {btnPressed, btnReleased};

class Button_t : public Control_t {
public:
    const ButtonStyle_t *StyleReleased, *StylePressed;
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
            const ButtonStyle_t &AStyleReleased, const ButtonStyle_t &AStylePressed,
            ftEvtCb AOnRelease) :
                Control_t(ctrlBtn, ALeft, ATop, AWidth, AHeight, AText),
                StyleReleased(&AStyleReleased), StylePressed(&AStylePressed),
                OnRelease(AOnRelease) {}
};

//class StringBox_t : public Widget_t {
//public:
//    Color_t ClrText;
//    void Draw() const;
//    StringBox_t(uint16_t ALeft, uint16_t AWidth, uint16_t ATop, uint16_t AHeight,
//            const char* AText, Color_t AClrText) :
//                Widget_t(ALeft, AWidth, ATop, AHeight, AText),
//                ClrText(AClrText) {}
//};

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
            const Control_t *PCtrl = Controls[i];
            PCtrl->CheckTouchAndAct(x, y);
//            if(PCtrl->Type == ctrlBtn) {
//                if(PCtrl->IsClickInside(x, y))
//            }

        }
    }

    void ProcessDetouch(int32_t x, int32_t y) const {
        for(uint32_t i=0; i<CtrlCnt; i++) {
            const Control_t *PCtrl = Controls[i];
            PCtrl->CheckDetouchAndAct(x, y);
        }
    }

    Page_t(const Control_t **AControls, uint32_t ACtrlCnt) :
        Controls(AControls), CtrlCnt(ACtrlCnt) {}
};
#endif
