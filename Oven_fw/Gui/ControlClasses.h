/*
 * ControlClasses.h
 *
 *  Created on: 15 ??? 2016 ?.
 *      Author: Kreyl
 */

#pragma once

enum Justify_t { jstLeft, jstCenter, jstRight };

#if 1 // ==== Classes ====
enum ControlType_t { ctrlBtn };

// Parent class for all controls
class Control_t {
public:
    ControlType_t Type;
    uint16_t Left, Top, Width, Height;
    const char* Text;
    virtual void Draw() const;
    Control_t(ControlType_t AType,
            uint16_t ALeft, uint16_t ATop, uint16_t AWidth, uint16_t AHeight, const char* AText) :
                Type(AType),
                Left(ALeft), Top(ATop), Width(AWidth), Height(AHeight), Text(AText) {}
};

class Button_t : public Control_t {
public:
    PFont_t Font;
    Color_t ClrText, ClrTop, ClrBot;
    void Draw() const;
    Button_t(uint16_t ALeft, uint16_t ATop, uint16_t AWidth, uint16_t AHeight,
            const char* AText, const Font_t &AFont, Color_t AClrText,
            Color_t AClrTop, Color_t AClrBot) :
                Control_t(ctrlBtn, ALeft, ATop, AWidth, AHeight, AText),
                Font(&AFont), ClrText(AClrText),
                ClrTop(AClrTop), ClrBot(AClrBot) {}
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
    Page_t(const Control_t **AControls, uint32_t ACtrlCnt) :
        Controls(AControls), CtrlCnt(ACtrlCnt) {}
};
#endif
