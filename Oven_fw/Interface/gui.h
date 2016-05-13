/*
 * gui.h
 *
 *  Created on: 13 ??? 2016 ?.
 *      Author: Kreyl
 */

#pragma once

#include <inttypes.h>

// Parent class for all
class Widget_t {
public:
    uint16_t Left, Width, Top, Height;
    virtual void Draw();
    Widget_t(uint16_t ALeft, uint16_t AWidth, uint16_t ATop, uint16_t AHeight) :
        Left(ALeft), Width(AWidth), Top(ATop), Height(AHeight) {}
};

class Button_t : public Widget_t {
public:
    const char* Text;
    Button_t(uint16_t ALeft, uint16_t AWidth, uint16_t ATop, uint16_t AHeight, const char* AText) :
        Widget_t(ALeft, AWidth, ATop, AHeight), Text(AText) {}
};

class Page_t {
public:
    uint8_t WidgetCnt;
    const Widget_t *PWidgets;
    Page_t(uint32_t AWidgetCnt, const Widget_t *AWidgets):
        WidgetCnt(AWidgetCnt), PWidgets(AWidgets) {}
    void Draw() const {
//        for(uint8_t i=0; i<WidgetCnt; i++) PWidgets[i].Draw();
    }
};

extern Page_t Page0;
