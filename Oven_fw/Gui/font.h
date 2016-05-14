/*
 * font.h
 *
 *  Created on: 14 ??? 2016 ?.
 *      Author: Kreyl
 */

#pragma once

#define FONT_CHAR_W(WidthPx, HeightPx)  (WidthPx * ((HeightPx + 7) / 8))

template <uint32_t CharSz>
struct FontChar_t {
    uint8_t Width;
    uint8_t data[CharSz];
};

template <uint32_t WidthPx, uint32_t HeightPx>
class Font_t {
public:
    uint8_t FirstCharCode;
    uint8_t MaxWidth;
    uint8_t RowsCnt;
    uint8_t YCenterLine;
    FontChar_t<FONT_CHAR_W(WidthPx, HeightPx)> *CharData;

    Font_t(uint8_t AFirstCharCode, uint8_t AMaxWidth, uint8_t ARowsCnt, uint8_t AYCenterLine, const uint8_t *FontData) :
        FirstCharCode(AFirstCharCode), MaxWidth(AMaxWidth), RowsCnt(ARowsCnt), YCenterLine(AYCenterLine) {
        CharData = (FontChar_t<FONT_CHAR_W(WidthPx, HeightPx)> *)FontData;
    }
//    FontChar_t* GetPChar(char c) const {
//        return (FontChar_t*)&CharData[c - FirstCharCode];
//    }
};
