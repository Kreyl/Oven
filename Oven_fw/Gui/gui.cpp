/*
 * gui.cpp
 *
 *  Created on: 13 ??? 2016 ?.
 *      Author: Kreyl
 */

#include "fnt_Tahoma25x27.h"
#include "gui.h"
#include "ILI9341.h"
#include "color.h"
#include "uart.h"
#include "font.h"

#include "mf_font.h"
#include "mf_justify.h"

// Fonts

Gui_t Gui;
ILI9341_t Lcd;
FrameBuffer_t<uint16_t, FRAMEBUFFER_LEN> FBuf;

//typedef const struct mf_font_s* font_t;

//extern const struct mf_rlefont_s mf_rlefont_DejaVuSans16;
//font_t FontDefault = (font_t)&mf_rlefont_DejaVuSans16;

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

void DrawStringBox(uint16_t Left, uint16_t Top, uint16_t Width, uint16_t Height,
        const char* S, font_t font, Justify_t Justify, Color_t ClrFront);

// Page 0
#define BTN_W       99
#define BTN_H       63
#define BTN_DIST    18

#define BTN_X0      (LCD_W - BTN_W)
#define BTN_Y0      0

#define BTN_COLOR_TOP   clWhite
#define BTN_COLOR_BOT   clWhite
//#define BTN_COLOR_BOT   (Color_t){0, 207, 0}

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
//    BtnCancel.Draw();
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
//    DrawStringBox(Left, Top, Width-1, Height-1, Text, Tahoma25x27, jstCenter, clBlack);
    Lcd.FillWindow(Left, Top, Width, Height, FBuf.Buf);
}

// ==== Text ====
uint32_t GetStringWidth(const char* S, font_t font) {
    uint32_t W = 0;
    uint32_t CHeight = font[2], CMaxWidth = font[1];
    uint8_t FirstSymbolCode = font[0];
//    Uart.Printf("ch=%u; cmw=%u; fsc=%u\r", CHeight, CMaxWidth, FirstSymbolCode);
    char c;
    while((c = *S++) != 0) {
        uint32_t Offset = 4 + (c - FirstSymbolCode) * (CMaxWidth*CHeight + 1);
        uint32_t CW = font[Offset];
//        Uart.Printf("%c %u   %u\r", c, CW, Offset);
        W += CW;
    }
    return W;
}

void DrawStringBox(uint16_t Left, uint16_t Top, uint16_t Width, uint16_t Height,
        const char* S, font_t font, Justify_t Justify, Color_t ClrFront) {

    // String width
    uint32_t StrW = GetStringWidth(S, font);
    Uart.Printf("W=%u\r", StrW);
    if(StrW > Width) StrW = Width;
    uint32_t XC = 0;    // x in buffer
    // Select the anchor position
    switch(Justify) {
        case jstLeft:   XC = 0;                  break;
        case jstCenter: XC = (Width - StrW) / 2; break;
        case jstRight:  XC = Width - StrW - 1;   break;
    }
    uint32_t CHeight = font[2], CMaxWidth = font[1];
    uint32_t YC = (Height - CHeight*8) / 2;   // y in buffer

    uint8_t FirstSymbolCode = font[0];
    uint16_t Clr565 = ClrFront.RGBTo565();
    char c;
    while((c = *S++) != 0) {
        // Pointer to char
        uint8_t *P = (uint8_t*)font + 4 + (c - FirstSymbolCode) * (CMaxWidth*CHeight + 1);
        uint32_t width = 1 + *P++;
        // Iterate pixels
        for(uint8_t x=0; x<width; x++) {
            for(uint8_t y=0; y<CHeight; y++) {
                uint8_t b = *P++;   // bit msk
                // Put pixels to buffer
                for(uint8_t i=0; i<8; i++) {
                    if(b & 0x01) FBuf.Put(x+XC, i+y*8+YC, Clr565);
                    else FBuf.Put(x+XC, i+y*8+YC, 0xFFFF);
                    b >>= 1;
                }
            } // y
        } // x
        XC += width;
    } // while

//    mf_render_aligned(font, Left, Top, justify, S, 0, drawcharglyph, nullptr);
}

#endif
