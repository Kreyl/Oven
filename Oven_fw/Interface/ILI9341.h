/*
 * ILI9341.h
 *
 *  Created on: 13 ??? 2016 ?.
 *      Author: Kreyl
 */

#pragma once

#include "kl_lib.h"
#include "color.h"

#define LCD_W               320 // }
#define LCD_H               240 // } Pixels count

class ILI9341_t {
private:
    void WriteCmd(uint8_t Cmd);
    void WriteData(uint16_t Data);
    uint16_t ReadData();
    void SetBounds(uint16_t Left, uint16_t Width, uint16_t Top, uint16_t Height);
    void PrepareToWriteGRAM() { WriteCmd(0x2C); }
public:
    void Init();
    void Cls(Color_t Color);
};

