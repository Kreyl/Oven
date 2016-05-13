/*
 * gui.cpp
 *
 *  Created on: 13 ??? 2016 ?.
 *      Author: Kreyl
 */

#include "gui.h"
#include "ILI9341.h"
#include "color.h"
#include "uart.h"

Gui_t Gui;
ILI9341_t Lcd;

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
    void Draw();
    Color_t ClrTop, ClrBot;
    Button_t(uint16_t ALeft, uint16_t AWidth, uint16_t ATop, uint16_t AHeight,
            const char* AText, Color_t AClrTop, Color_t AClrBot) :
        Widget_t(ALeft, AWidth, ATop, AHeight), Text(AText),
        ClrTop(AClrTop), ClrBot(AClrBot) {}
};

//struct tst_t {
//    uint8_t a, b, c;
//};
//
//tst_t tst = {
//        a: 2, b: 3, c: 4};

//class Page_t {
//public:
//    uint8_t WidgetCnt;
//    const Widget_t *PWidgets;
//    Page_t(uint32_t AWidgetCnt, const Widget_t *AWidgets):
//        WidgetCnt(AWidgetCnt), PWidgets(AWidgets) {}
//    void Draw() const {
////        for(uint8_t i=0; i<WidgetCnt; i++) PWidgets[i].Draw();
//    }
//};

// Page 0
#define BTN_W       99
#define BTN_H       63
#define BTN_DIST    18

#define BTN_X0      (LCD_W - BTN_W)
#define BTN_Y0      0

#define BTN_COLOR_TOP   clWhite
#define BTN_COLOR_BOT   (Color_t){0, 207, 0}

Button_t BtnOk     {
    BTN_X0, BTN_W, BTN_Y0, BTN_H,
    "OK",
    BTN_COLOR_TOP, BTN_COLOR_BOT
};

Button_t BtnCancel {
    BTN_X0, BTN_W, (BTN_Y0 + BTN_H + BTN_DIST), BTN_H,
    "Cancel",
    BTN_COLOR_TOP, BTN_COLOR_BOT
};

#if 1 // =============================== Pages =================================
static void DrawPage0() {
    BtnOk.Draw();
    BtnCancel.Draw();
}
#endif // Pages

#if 1 // =========================== Implementation ============================
void Gui_t::Init() {
    Lcd.Init();
//    Lcd.Cls(clBlack);
}

void Gui_t::DrawPage(uint8_t APage) {
    Lcd.Cls(clBlack);
    DrawPage0();
}

// Grafics

// Button
void Button_t::Draw() {
//    Uart.Printf("%u %u %u %u %S\r", Left, Width, Top, Height, Text);
    if(ClrTop == ClrBot) Lcd.DrawRect(Left, Width, Top, Height, ClrTop);
    else {
        uint32_t dalpha = 255 / Height;
        uint32_t alpha = 0;
        for(uint32_t i=0; i<Height; i++, alpha += dalpha) {
            uint16_t Clr565 = ColorBlend(ClrBot, ClrTop, alpha);
            Lcd.DrawRect(Left, Width, Top+i, 1, Clr565);
        }
    }

}

#endif

