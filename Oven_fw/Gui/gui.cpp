/*
 * gui.cpp
 *
 *  Created on: 13 ??? 2016 ?.
 *      Author: Kreyl
 */

#include "fnt_Verdana27x27.h"

#include "gui.h"
#include "ILI9341.h"
#include "color.h"
#include "uart.h"
#include "font.h"

Gui_t Gui;
ILI9341_t Lcd;
FrameBuffer_t<uint16_t, FRAMEBUFFER_LEN> FBuf;

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
    PFont_t Font;
    Color_t ClrText, ClrTop, ClrBot;
    void Draw() const;
    Button_t(uint16_t ALeft, uint16_t ATop, uint16_t AWidth, uint16_t AHeight,
            const char* AText, const Font_t &AFont, Color_t AClrText,
            Color_t AClrTop, Color_t AClrBot) :
        Widget_t(ALeft, ATop, AWidth, AHeight, AText),
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
#endif

void DrawStringBox(uint16_t Left, uint16_t Top, uint16_t Width, uint16_t Height,
        const char* S, PFont_t Font, Justify_t Justify, Color_t ClrFront);

// Page 0
#define BTN_W       99
#define BTN_H       63
#define BTN_DIST    18

#define BTN_X0      (LCD_W - BTN_W)
#define BTN_Y0      0

#define BTN_FONT        fntVerdana27x27
#define BTN_COLOR_TEXT  clWhite
#define BTN_COLOR_TOP   clWhite
//#define BTN_COLOR_BOT   clWhite
//#define BTN_COLOR_BOT   clBlue
#define BTN_COLOR_BOT   (Color_t){0, 144, 0}

const Button_t BtnOk     {
    BTN_X0, BTN_Y0, BTN_W, BTN_H,
    "OK", BTN_FONT, BTN_COLOR_TEXT,
    BTN_COLOR_TOP, BTN_COLOR_BOT
};

const Button_t BtnCancel {
    BTN_X0, (BTN_Y0 + BTN_H + BTN_DIST), BTN_W, BTN_H,
    "Cancel", BTN_FONT, BTN_COLOR_TEXT,
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
    DrawStringBox(Left, Top, Width-1, Height-1, Text, Font, jstCenter, ClrText);
    Lcd.FillWindow(Left, Top, Width, Height, FBuf.Buf);
}

// ==== Text ====
void DrawStringBox(uint16_t Left, uint16_t Top, uint16_t Width, uint16_t Height,
        const char* S, PFont_t Font, Justify_t Justify, Color_t ClrFront) {
    // String width
    uint32_t StrW = Font->GetStringWidth(S);
    if(StrW > Width) StrW = Width;
    uint32_t XC = 0;    // x in buffer
    // Select the anchor position
    switch(Justify) {
        case jstLeft:   XC = 0;                  break;
        case jstCenter: XC = (Width - StrW) / 2; break;
        case jstRight:  XC = Width - StrW - 1;   break;
    }
    int32_t YC = Height / 2 - Font->YCenterLine;   // y in buffer
    if(YC < 0) YC = 0;

    uint16_t Clr565 = ClrFront.RGBTo565();
    char c;
    while((c = *S++) != 0) {
        uint8_t width;
        uint8_t *ptr;
        Font->GetChar(c, &width, &ptr);
        // Iterate pixels
        for(uint8_t x=0; x<width; x++) {
            uint32_t YCounter = 0;
            for(uint8_t y=0; y<Font->RowsCnt; y++) {
                uint8_t b = *ptr++;   // bit msk
                // Put pixels to buffer
                for(uint8_t i=0; i<8; i++) {
                    if(b & 0x01) FBuf.Put(x+XC, i+y*8+YC, Clr565);  // Modify only "black" pixels
                    b >>= 1;
                    if(++YCounter >= Font->Height) break;
                }
            } // y
        } // x
        XC += width+1;
    } // while
}

#endif

