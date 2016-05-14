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
FrameBuffer_t<uint16_t, FRAMEBUFFER_LEN> FBuf;

typedef const struct mf_font_s* font_t;


extern const struct mf_rlefont_s mf_rlefont_DejaVuSans16;
font_t FontDefault = (font_t)&mf_rlefont_DejaVuSans16;

#if 1 // ==== Classes ====
// Parent class for all
class Widget_t {
public:
    uint16_t Left, Top, Width, Height;
    const char* Text;
    virtual void Draw() const;
    Widget_t(uint16_t ALeft, uint16_t ATop, uint16_t AWidth, uint16_t AHeight, const char* AText) :
        Left(ALeft), Top(ATop), Width(AWidth), Height(AHeight), Text(AText) {}
};

class Button_t : public Widget_t {
public:
    Color_t ClrTop, ClrBot;
    void Draw() const;
    Button_t(uint16_t ALeft, uint16_t ATop, uint16_t AWidth, uint16_t AHeight,
            const char* AText, Color_t AClrTop, Color_t AClrBot) :
        Widget_t(ALeft, ATop, AWidth, AHeight, AText),
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
#endif

//static void DrawStringBox(uint16_t Left, uint16_t Top, uint16_t Width, uint16_t Height,
//        const char* S, font_t font, Color_t color, mf_align_t justify);

// Page 0
#define BTN_W       99
#define BTN_H       63
#define BTN_DIST    18

#define BTN_X0      (LCD_W - BTN_W)
#define BTN_Y0      0

#define BTN_COLOR_TOP   clWhite
//#define BTN_COLOR_BOT   clWhite
#define BTN_COLOR_BOT   (Color_t){0, 207, 0}

const Button_t BtnOk     {
    BTN_X0, BTN_Y0, BTN_W, BTN_H,
    "OK",
    BTN_COLOR_TOP, BTN_COLOR_BOT
};

const Button_t BtnCancel {
    BTN_X0, (BTN_Y0 + BTN_H + BTN_DIST), BTN_W, BTN_H,
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
    if(FBuf.Setup(Width, Height) != OK) {
        Uart.Printf("Too Large Btn\r");
        return;
    }
    // Shape
    if(ClrTop == ClrBot) {
        uint16_t Color565 = ClrTop.RGBTo565();
        for(uint32_t y=0; y<Height; y++) {
            for(uint32_t x=0; x<Width; x++) FBuf.Put(x, y, Color565);
        }
    }
    else {
        uint32_t dalpha = 255 / Height;
        uint32_t alpha = 0;
        for(uint32_t y=0; y<Height; y++, alpha += dalpha) {
            uint16_t Color565 = ColorBlend(ClrBot, ClrTop, alpha);
            for(uint32_t x=0; x<Width; x++) FBuf.Put(x, y, Color565);
        }
    }
    // Text
//    DrawStringBox(Left, Top, Width-1, Height-1, Text, FontDefault, clWhite, MF_ALIGN_CENTER);
    Lcd.FillWindow(Left, Top, Width, Height, FBuf.Buf);
}

void DrawString(uint16_t Left, uint16_t Top, uint16_t Width, uint16_t Height,
        const char* S, font_t font, Justify_t Justify, Color_t ClrFront) {
    // Select the anchor position
    switch(Justify) {
        case jstLeft:   Left += font->baseline_x; break;
        case jstCenter: Left += (Width + 1) / 2;  break;
        case jstRight:  Left += Width;            break;
    }
    Top += (Height+1 - font->height)/2;
//    mf_render_aligned(font, Left, Top, justify, S, 0, drawcharglyph, nullptr);
}

#endif

