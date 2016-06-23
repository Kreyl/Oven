/*
 * ControlClasses.cpp
 *
 *  Created on: 23 θώνÿ 2016 γ.
 *      Author: Kreyl
 */

#include <gui_engine.h>
#include "kl_lib.h"
#include "uart.h"
#include "ControlClasses.h"

extern FrameBuffer_t<uint16_t, FRAMEBUFFER_LEN> FBuf;

// ==== Common ====
void DrawText(uint16_t Left, uint16_t Top, uint16_t Width, uint16_t Height,
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

void Control_t::FillRect(Color_t ClrTop, Color_t ClrBottom) const {
    if(ClrTop == ClrBottom) {
        uint16_t Color565 = ClrTop.RGBTo565();
        for(uint32_t y=0; y<Height; y++) {
            for(uint32_t x=0; x<Width; x++) FBuf.Put(x, y, Color565);
        }
    }
    else {
        uint32_t dalpha = 255 / Height;
        uint32_t alpha = 0;
        for(uint32_t y=0; y<Height; y++, alpha += dalpha) {
            uint16_t Color565 = ColorBlend(ClrBottom, ClrTop, alpha);
            for(uint32_t x=0; x<Width; x++) FBuf.Put(x, y, Color565);
        }
    }
}

// Button
void Button_t::Draw(BtnState_t State) const {
//    Uart.Printf("%u %u %u %u %S\r", Left, Width, Top, Height, Text);
    if(FBuf.Setup(Width, Height) != OK) {
        Uart.Printf("Too Large Btn\r");
        return;
    }
    // Draw shape depending on state
    if(State == btnReleased) {
        FillRect(Theme.BtnClrReleasedTop, Theme.BtnClrReleasedBottom);
    }
    else {
        FillRect(Theme.BtnClrPressedTop, Theme.BtnClrPressedBottom);
    }
    DrawText(Left, Top, Width-1, Height-1, Text, Theme.BtnFont, jstCenter, Theme.BtnClrText);
    Lcd.FillWindow(Left, Top, Width, Height, FBuf.Buf);
}

// Textbox
void Textbox_t::Draw() const {
//    Uart.Printf("%u %u %u %u %S\r", Left, Width, Top, Height, Text);
    if(FBuf.Setup(Width, Height) != OK) {
        Uart.Printf("Too Large Textbox\r");
        return;
    }
    // Draw shape
    FillRect(ClrBack, ClrBack);
    // Text
    DrawText(Left, Top, Width-1, Height-1, Text, Theme.TxtboxFont, jstCenter, ClrText);
    Lcd.FillWindow(Left, Top, Width, Height, FBuf.Buf);
}
