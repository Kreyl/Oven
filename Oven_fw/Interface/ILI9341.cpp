/*
 * ILI9341.cpp
 *
 *  Created on: 13 ??? 2016 ?.
 *      Author: Kreyl
 */

#include "ILI9341.h"
#include "board.h"
#include "uart.h"

#if 1 // ==== Pin driving functions ====
#define RstHi()  { PinSet(LCD_RESET);   }
#define RstLo()  { PinClear(LCD_RESET); }
#define CsHi()   { PinSet(LCD_CSX);    }
#define CsLo()   { PinClear(LCD_CSX);  }
#define DcHi()   { PinSet(LCD_DC);    }
#define DcLo()   { PinClear(LCD_DC);  }
#define WrHi()   { PinSet(LCD_WR);    }
#define WrLo()   { PinClear(LCD_WR);  }
#define RdHi()   { PinSet(LCD_RD);    }
#define RdLo()   { PinClear(LCD_RD);  }
#define Write(Value) PortSetValue(LCD_DATA_GPIO, Value)

/*
#define WriteData(Data) { \
    LCD_DATA_GPIO->ODR = Data; \
    GPIOC->BRR = (1 << 5); \
    GPIOC->BSRR = (1 << 5); \
}
*/

#endif

void ILI9341_t::Init() {
    // ==== GPIO ====
    PinSetupOut(LCD_RESET, omPushPull, pudNone);
    PinSetupOut(LCD_CSX, omPushPull, pudNone);
    PinSetupOut(LCD_DC, omPushPull, pudNone, psHigh);
    PinSetupOut(LCD_WR, omPushPull, pudNone, psHigh);
    PinSetupOut(LCD_RD, omPushPull, pudNone, psHigh);
    // Data port
    PortInit(LCD_DATA_GPIO, omPushPull, pudNone, psHigh);
    PortSetupOutput(LCD_DATA_GPIO);
    // ==== Init LCD ====
    // Initial signals
    RstHi();
    CsHi();
    RdHi();
    WrHi();
    // Reset LCD
    chThdSleepMilliseconds(7);
    RstLo();
    chThdSleepMilliseconds(11);
    RstHi();
    chThdSleepMilliseconds(126);
    CsLo(); // Stay selected forever

    // Commands
    WriteCmd(0x11); // Sleep out
    chThdSleepMilliseconds(126);

    WriteCmd(0x29); // Display ON
    // Row order etc.
    WriteCmd(0x36);
    WriteData(0xE8);    // MY, MX, Row/Column exchange, BGR
    // Pixel format
    WriteCmd(0x3A);
    WriteData(0x55);    // 16 bit both RGB & MCU

//    WriteCmd(0x09);
//    PortSetupInput(LCD_DATA_GPIO);
//    for(uint8_t i=0; i<4; i++) {
//        RdLo();
//        RdHi();
//        uint16_t r = LCD_DATA_GPIO->IDR;
//        Uart.Printf("Lcd: %X\r", r);
//    }
//    PortSetupOutput(LCD_DATA_GPIO);
}

void ILI9341_t::WriteCmd(uint8_t Cmd) {
    DcLo();
    PortSetValue(LCD_DATA_GPIO, Cmd);
    WrLo();
    WrHi();
    DcHi();
}

void ILI9341_t::WriteData(uint16_t Data) {
    PortSetValue(LCD_DATA_GPIO, Data);
    WrLo();
    WrHi();
}

uint16_t ILI9341_t::ReadData() {
    PortSetupInput(LCD_DATA_GPIO);
    RdLo();
    uint16_t Rslt = PortGetValue(LCD_DATA_GPIO);
    RdHi();
    PortSetupOutput(LCD_DATA_GPIO);
    return Rslt;
}

void ILI9341_t::SetBounds(uint16_t Left, uint16_t Width, uint16_t Top, uint16_t Height) {
    uint16_t XEndAddr = Left + Width  - 1;
    uint16_t YEndAddr = Top  + Height - 1;
    // Write bounds
    WriteCmd(0x2A); // X
    WriteData(Left>>8);
    WriteData(Left);            // MSB will be ignored anyway
    WriteData(XEndAddr >> 8);
    WriteData(XEndAddr);
    WriteCmd(0x2B); // Y
    WriteData(Top >> 8);
    WriteData(Top);             // MSB will be ignored anyway
    WriteData(YEndAddr >> 8);
    WriteData(YEndAddr);
}

void ILI9341_t::Cls(Color_t Color) {
    SetBounds(0, LCD_W, 0, LCD_H);
    // Convert to 565
    uint16_t Clr565 = Color.RGBTo565();
    // Fill LCD
    PrepareToWriteGRAM();
    for(uint32_t i=0; i<(LCD_H * LCD_W); i++) WriteData(Clr565);
}
