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

#include "mf_font.h"
#include "mf_justify.h"

Gui_t Gui;
ILI9341_t Lcd;

typedef const struct mf_font_s* font_t;


extern const struct mf_rlefont_s mf_rlefont_DejaVuSans16;
font_t FontDefault = (font_t)&mf_rlefont_DejaVuSans16;

// Parent class for all
class Widget_t {
public:
    uint16_t Left, Width, Top, Height;
    const char* Text;
    virtual void Draw() const;
    Widget_t(uint16_t ALeft, uint16_t AWidth, uint16_t ATop, uint16_t AHeight, const char* AText) :
        Left(ALeft), Width(AWidth), Top(ATop), Height(AHeight), Text(AText) {}
};

class Button_t : public Widget_t {
public:
    Color_t ClrTop, ClrBot;
    void Draw() const;
    Button_t(uint16_t ALeft, uint16_t AWidth, uint16_t ATop, uint16_t AHeight,
            const char* AText, Color_t AClrTop, Color_t AClrBot) :
        Widget_t(ALeft, AWidth, ATop, AHeight, AText),
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

static void DrawStringBox(uint16_t Left, uint16_t Width, uint16_t Top, uint16_t Height,
        const char* S, font_t font, Color_t color, mf_align_t justify);

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

const Button_t BtnOk     {
    BTN_X0, BTN_W, BTN_Y0, BTN_H,
    "OK",
    BTN_COLOR_TOP, BTN_COLOR_BOT
};

const Button_t BtnCancel {
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
void Button_t::Draw() const {
//    Uart.Printf("%u %u %u %u %S\r", Left, Width, Top, Height, Text);
    // Shape
    if(ClrTop == ClrBot) Lcd.DrawRect(Left, Width, Top, Height, ClrTop);
    else {
        uint32_t dalpha = 255 / Height;
        uint32_t alpha = 0;
        for(uint32_t i=0; i<Height; i++, alpha += dalpha) {
            uint16_t Clr565 = ColorBlend(ClrBot, ClrTop, alpha);
            Lcd.DrawRect(Left, Width, Top+i, 1, Clr565);
        }
    }
    // Text
    DrawStringBox(Left, Top, Width-1, Height-1, Text, FontDefault, clWhite, MF_ALIGN_CENTER);
}

static uint8_t drawcharglyph(int16_t x, int16_t y, mf_char ch, void *state) {
    return 0;//mf_render_character(GD->t.font, x, y, ch, drawcharline, state);
}


void DrawStringBox(uint16_t Left, uint16_t Width, uint16_t Top, uint16_t Height,
        const char* S, font_t font, Color_t color, mf_align_t justify) {
    // Select the anchor position
    switch(justify) {
    case MF_ALIGN_CENTER:
        Left += (Width + 1) / 2;
        break;
    case MF_ALIGN_RIGHT:
        Left += Width;
        break;
    default:    // justifyLeft
        Left += font->baseline_x;
        break;
    }

    Top += (Height+1 - font->height)/2;
    mf_render_aligned(font, Left, Top, justify, S, 0, drawcharglyph, nullptr);
}

#endif

