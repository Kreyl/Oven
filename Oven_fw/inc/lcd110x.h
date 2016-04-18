/*
 * File:   lcd110x.h 
 * Author: Kreyl Laurelindo
 *
 * Created on 24 �������� 2010 �., 14:25
 */

#ifndef LCD110X_H
#define	LCD110X_H

#include "stm32f10x.h"
#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_dma.h"

#include <string.h>

// ================================ Defines ====================================
#define LCD_GPIO        GPIOB
#define LCD_USART       USART3
#define LCD_DMA_CHNL    DMA1_Channel2
#define LCD_DMA_IRQ     DMA1_Channel2_IRQn

#define LCD_SCLK        GPIO_Pin_12
#define LCD_SDA         GPIO_Pin_10
#define LCD_XRES        GPIO_Pin_5
#define LCD_XCS         GPIO_Pin_2

#define LCD_CMD         0x0000
#define LCD_DATA        0x0001

#define LCD_STR_HEIGHT  8
#define LCD_STR_WIDTH   16

#define LCD_PAGE_DELAY  999

// Uncomment this line, if image need to be turned upside down
#define LCD_UPSIDEDOWN

// Data sizes
#define LCD_VIDEOBUF_SIZE       864     // = 96 * 9
#define LCD_TEXT_SIZE_MAX       512     // chars count
// Number of initialization commands
#ifdef LCD_UPSIDEDOWN
#define LCD_INITCMD_COUNT       (6+3)
#else
#define LCD_INITCMD_COUNT       (4+3)   // +3 instructions to set x=0 y=0
#endif
#define LCD_BUF_SIZE    (LCD_VIDEOBUF_SIZE + LCD_INITCMD_COUNT)


enum Invert_t {NotInverted, Inverted};

struct LcdBuf_t {
    uint16_t Cmd[LCD_INITCMD_COUNT];
    uint16_t Data[LCD_VIDEOBUF_SIZE];
};

class Lcd_t {
private:
    // Variables
    union {
        LcdBuf_t Ram;
        uint16_t CommonBuf[LCD_BUF_SIZE];
    };
    uint16_t CurrentPosition;   // x, y to place data to
    // Text
    uint32_t Timer;
    uint8_t GlobY;
    char *IStartPageP;
    bool EndOfScroll, DisplayingText;
    void FillScreen(void);
    // Bit distortion to fulfill LSB-only USART capability
    uint16_t Distort(uint16_t CmdDta, uint8_t AByte);
    // Pin driving functions
    void XRES_Hi(void) { LCD_GPIO->BSRR = LCD_XRES; }
    void XRES_Lo(void) { LCD_GPIO->BRR  = LCD_XRES; }
public:
    char TextToShow[LCD_TEXT_SIZE_MAX];
    // General use
    void Init(void);
    void Task(void);
    void Shutdown(void);
    // Interrupt uses this
    void XCS_Hi (void) { LCD_GPIO->BSRR = LCD_XCS;  }
    void XCS_Lo (void) { LCD_GPIO->BRR  = LCD_XCS;  }
    void LCD_DMA_Init(void);
    // Grafics handling
    void Cls(void);
    void GotoXY(uint8_t x, uint8_t y);
    void GotoCharXY(uint8_t x, uint8_t y);
    void DrawChar(uint8_t AChar, Invert_t AInvert);
    void PrintString (const uint8_t x, const uint8_t y, const char *S, Invert_t AInvert);
    void PrintStringLen(const char *S, uint16_t ALen, Invert_t AInvert);
    void PrintText(void);

    void PrintBigUint(const uint8_t y, uint16_t ANumber);

    void DrawImage(const uint8_t x, const uint8_t y, const uint8_t *Img, Invert_t AInvert);
};

extern Lcd_t Lcd;

// DMA transfer complete interrupt handler
//#ifdef __cplusplus
//extern "C" {
//void DMA1_Channel2_IRQHandler(void);
//}
//#endif


#endif	/* LCD110X_H */

