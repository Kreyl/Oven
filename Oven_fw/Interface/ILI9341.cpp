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

    WriteCmd(0x09);
    PortSetupInput(LCD_DATA_GPIO);
    for(uint8_t i=0; i<4; i++) {
        RdLo();
        RdHi();
        uint16_t r = LCD_DATA_GPIO->IDR;
        Uart.Printf("Lcd: %X\r", r);
    }
    PortSetupOutput(LCD_DATA_GPIO);

//    WriteCmd(0xCF);
//    WriteData(0x00);
//    WriteData(0xc3);
//    WriteData(0X30);
//
//    WriteCmd(0xED);
//    WriteData(0x64);
//    WriteData(0x03);
//    WriteData(0X12);
//    WriteData(0X81);
//
//    WriteCmd(0xE8);
//    WriteData(0x85);
//    WriteData(0x10);
//    WriteData(0x79);
//
//    WriteCmd(0xCB);
//    WriteData(0x39);
//    WriteData(0x2C);
//    WriteData(0x00);
//    WriteData(0x34);
//    WriteData(0x02);
//
//    WriteCmd(0xF7);
//    WriteData(0x20);
//
//    WriteCmd(0xEA);
//    WriteData(0x00);
//    WriteData(0x00);
//
//    WriteCmd(0xC0);    //Power control
//    WriteData(0x22);   //VRH[5:0]
//
//    WriteCmd(0xC1);    //Power control
//    WriteData(0x11);   //SAP[2:0];BT[3:0]
//
//    WriteCmd(0xC5);    //VCM control
//    WriteData(0x3d);
//    //LCD_DataWrite_ILI9341(0x30);
//    WriteData(0x20);
//
//    WriteCmd(0xC7);    //VCM control2
//    //LCD_DataWrite_ILI9341(0xBD);
//    WriteData(0xAA); //0xB0
//
//    WriteCmd(0x36);    // Memory Access Control
//    WriteData(0x08);
//
//    WriteCmd(0x3A);
//    WriteData(0x55);
//
//    WriteCmd(0xB1);
//    WriteData(0x00);
//    WriteData(0x13);
//
//    WriteCmd(0xB6);    // Display Function Control
//    WriteData(0x0A);
//    WriteData(0xA2);
//
//    WriteCmd(0xF6);
//    WriteData(0x01);
//    WriteData(0x30);
//
//    WriteCmd(0xF2);    // 3Gamma Function Disable
//    WriteData(0x00);
//
//    WriteCmd(0x26);    //Gamma curve selected
//    WriteData(0x01);
//
//    WriteCmd(0xE0);    //Set Gamma
//    WriteData(0x0F);
//    WriteData(0x3F);
//    WriteData(0x2F);
//    WriteData(0x0C);
//    WriteData(0x10);
//    WriteData(0x0A);
//    WriteData(0x53);
//    WriteData(0XD5);
//    WriteData(0x40);
//    WriteData(0x0A);
//    WriteData(0x13);
//    WriteData(0x03);
//    WriteData(0x08);
//    WriteData(0x03);
//    WriteData(0x00);
//
//    WriteCmd(0XE1);    //Set Gamma
//    WriteData(0x00);
//    WriteData(0x00);
//    WriteData(0x10);
//    WriteData(0x03);
//    WriteData(0x0F);
//    WriteData(0x05);
//    WriteData(0x2C);
//    WriteData(0xA2);
//    WriteData(0x3F);
//    WriteData(0x05);
//    WriteData(0x0E);
//    WriteData(0x0C);
//    WriteData(0x37);
//    WriteData(0x3C);
//    WriteData(0x0F);
//
//    WriteCmd(0x11);    //Exit Sleep
//    chThdSleepMilliseconds(120);
//    WriteCmd(0x29);    //Display on
//    chThdSleepMilliseconds(50);
}

void ILI9341_t::WriteCmd(uint8_t Cmd) {
    DcLo();
    PortSetValue(LCD_DATA_GPIO, Cmd);
    WrLo();
    __NOP();
    WrHi();
    __NOP();
    DcHi();
}

void ILI9341_t::WriteData(uint16_t Data) {
    PortSetValue(LCD_DATA_GPIO, Data);
    WrLo();
    __NOP();
    WrHi();
    __NOP();
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
//    SetBounds(50, 50, 20, 100);
    // Convert to 565
    uint16_t Clr565 = Color.RGBTo565();
    // Fill LCD
    PrepareToWriteGRAM();
    for(uint32_t i=0; i<(LCD_H * LCD_W); i++) WriteData(Clr565);
//    for(uint32_t i=0; i<(50 * 100); i++) {
//        WriteData(0xF800);
//        WriteData(0xF8);
//        WriteData(0x00);
//    }
}
