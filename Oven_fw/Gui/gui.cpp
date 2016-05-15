/*
 * gui.cpp
 *
 *  Created on: 13 ??? 2016 ?.
 *      Author: Kreyl
 */

#include "fnt_Verdana27x27.h"

#include "gui.h"
#include "ILI9341.h"
#include "stmpe811.h"
#include "uart.h"

#include "Controls.h"

Gui_t Gui;
ILI9341_t Lcd;
FrameBuffer_t<uint16_t, FRAMEBUFFER_LEN> FBuf;

void DrawStringBox(uint16_t Left, uint16_t Top, uint16_t Width, uint16_t Height,
        const char* S, PFont_t Font, Justify_t Justify, Color_t ClrFront);

static THD_WORKING_AREA(waGuiThread, 256);
__noreturn
static THD_FUNCTION(GuiThread, arg) {
    chRegSetThreadName("Gui");
    Gui.ITask();
}

#if 1 // =========================== Implementation ============================
void Gui_t::Init() {
    CurrPage = &Page0;
    Lcd.Init();
    Touch.Init();
    chThdCreateStatic(waGuiThread, sizeof(waGuiThread), NORMALPRIO, GuiThread, NULL);
}

void Gui_t::DrawPage(uint8_t APage) {
    Lcd.Cls(clBlack);
    Page0.Draw();
}

__noreturn
void Gui_t::ITask() {
    bool TouchProcessed = false, DetouchProcessed = true;
    while(true) {
        chThdSleepMilliseconds(TOUCH_POLLING_PERIOD_MS);
        if(Touch.IsTouched()) {
            if(TouchProcessed) Touch.DiscardData();
            else {   // New touch detected
                if(Touch.ReadData() == NEW) {
                    TouchProcessed = true;
                    DetouchProcessed = false;
                    Uart.Printf("X=%d; Y=%d\r", Touch.X, Touch.Y);
                    CurrPage->ProcessTouch(Touch.X, Touch.Y);
                }
                else {
                    TouchProcessed = false;
                    DetouchProcessed = false;
                }
            } // new touch
        }
        else {
            TouchProcessed = false;
            if(!DetouchProcessed) {
                DetouchProcessed = true;
                Uart.Printf("Detouch\r");
                CurrPage->ProcessDetouch(Touch.X, Touch.Y);
            }
        }
    } // while true
}

void Gui_t::ProcessClick() {
//    CurrPage
}

// Grafics

// Button
void Button_t::Draw(BtnState_t State) const {
//    Uart.Printf("%u %u %u %u %S\r", Left, Width, Top, Height, Text);
    if(FBuf.Setup(Width, Height) != OK) {
        Uart.Printf("Too Large Btn\r");
        return;
    }
    // Select style depending on state
    const ButtonStyle_t *Style = (State == btnReleased)? StyleReleased : StylePressed;
    // Shape
    if(Style->ClrTop == Style->ClrBot) {
        uint16_t Color565 = Style->ClrTop.RGBTo565();
        for(uint32_t y=0; y<Height; y++) {
            for(uint32_t x=0; x<Width; x++) FBuf.Put(x, y, Color565);
        }
    }
    else {
        uint32_t dalpha = 255 / Height;
        uint32_t alpha = 0;
        for(uint32_t y=0; y<Height; y++, alpha += dalpha) {
            uint16_t Color565 = ColorBlend(Style->ClrBot, Style->ClrTop, alpha);
            for(uint32_t x=0; x<Width; x++) FBuf.Put(x, y, Color565);
        }
    }
    // Text
    DrawStringBox(Left, Top, Width-1, Height-1, Text, Style->Font, jstCenter, Style->ClrText);
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

