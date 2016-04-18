#include "lcd110x.h"
#include "lcd_font.h"
#include "images.h" 
#include "string.h"

#include "delay_util.h"
#include "uart.h"

Lcd_t Lcd;

void Lcd_t::Task(void) {
    // Restart DMA if needed
    if (DMA_GetFlagStatus(DMA1_FLAG_TC2) == SET) {
        if (USART_GetFlagStatus(LCD_USART, USART_FLAG_TC) == SET) {  // Wait until last byte transmitted
            // Clear DMA Half Transfer, Transfer Complete and Global interrupt pending bits
            DMA_ClearITPendingBit(DMA1_IT_GL2);
            USART_ClearFlag(LCD_USART, USART_FLAG_TC);
            XCS_Hi();   // End transmission
            USART_Cmd(LCD_USART, DISABLE);
            LCD_DMA_Init();
            XCS_Lo();   // Start transmission
            DMA_Cmd(LCD_DMA_CHNL, ENABLE);          // Enable USARTy DMA TX Channel
            USART_Cmd(LCD_USART, ENABLE);
        }
    }
    // Handle text displaying
    if (!DisplayingText) return;
    if (!Delay.Elapsed(&Timer, LCD_PAGE_DELAY)) return;
    if (EndOfScroll) PrintText();
    else FillScreen();
}

void Lcd_t::Init(void) {
    // ====================== Hardware setup ===================================
    // ==== GPIO init ====
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    // Configure LCD_XRES & LCD_XCS as Push-Pull output
    GPIO_InitStructure.GPIO_Pin = LCD_XRES | LCD_XCS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(LCD_GPIO, &GPIO_InitStructure);
    // Configure LCD_SCLK & LCD_SDA as AF Push-pull
    GPIO_InitStructure.GPIO_Pin = LCD_SCLK | LCD_SDA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(LCD_GPIO, &GPIO_InitStructure);

    // Workaround hardware bug with disabled CK3 when SPI2 is enabled
    uint16_t tmp = SPI2->CR2;
    tmp |= SPI_CR2_SSOE;
    SPI2->CR2 = tmp;

    // ==== USART init ====
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    // Usart clock: enabled, idle low, first edge, enable last bit pulse
    USART_ClockInitTypeDef USART_ClockInitStructure;
    USART_ClockInitStructure.USART_Clock = USART_Clock_Enable;
    USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
    USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;
    USART_ClockInitStructure.USART_LastBit = USART_LastBit_Enable;
    USART_ClockInit(LCD_USART, &USART_ClockInitStructure);
    // Usart itself
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 500000;
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;
    USART_Init(LCD_USART, &USART_InitStructure);

    // ==== DMA ====
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    LCD_DMA_Init();

    // ==== DMA IRQ ====
//    NVIC_InitTypeDef NVIC_InitStructure;
//    // Enable DMA1 channel3 IRQ Channel
//    NVIC_InitStructure.NVIC_IRQChannel = LCD_DMA_IRQ;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);

    // ========================= Init LCD ======================================
    XCS_Hi();
    // Reset display
    XRES_Lo();
    Delay.ms(54);
    XRES_Hi();
    // Initial commands
    Cls(); // clear LCD
    Ram.Cmd[0] = Distort(LCD_CMD, 0xA4);    // Set normal display mode
    Ram.Cmd[1] = Distort(LCD_CMD, 0x2F);    // Charge pump on
    Ram.Cmd[2] = Distort(LCD_CMD, 0x40);    // Set start row address = 0
    Ram.Cmd[3] = Distort(LCD_CMD, 0xAF);    // display ON
    // Set x=0, y=0
    Ram.Cmd[4] = Distort(LCD_CMD, 0xB0);    // Y axis initialization
    Ram.Cmd[5] = Distort(LCD_CMD, 0x00);    // X axis initialisation1
    Ram.Cmd[6] = Distort(LCD_CMD, 0x10);    // X axis initialisation2
#ifdef LCD_UPSIDEDOWN
    Ram.Cmd[7] = Distort(LCD_CMD, 0xC8);    // mirror Y axis
    Ram.Cmd[8] = Distort(LCD_CMD, 0xA1);    // Mirror X axis
#endif
    // Start transmission
    XCS_Lo();
    DMA_Cmd(LCD_DMA_CHNL, ENABLE);          // Enable USARTy DMA TX Channel
    // Enable USART
    USART_Cmd(LCD_USART, ENABLE);
}

void Lcd_t::LCD_DMA_Init() {
    DMA_InitTypeDef DMA_InitStructure;
    DMA_DeInit(LCD_DMA_CHNL);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&LCD_USART->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) CommonBuf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = LCD_BUF_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(LCD_DMA_CHNL, &DMA_InitStructure);
    // Enable USARTy DMA TX request
    USART_DMACmd(LCD_USART, USART_DMAReq_Tx, ENABLE);
    // Enable DMA Transfer Complete interrupt
//    DMA_ITConfig(LCD_DMA_CHNL, DMA_IT_TC, ENABLE);
}

void Lcd_t::Shutdown(void) {
    DMA_Cmd(LCD_DMA_CHNL, DISABLE);
    XRES_Lo();
    XCS_Lo();
}
// ================================ Graphics ===================================
void Lcd_t::Cls() {
    DisplayingText = false;
    for (uint16_t i = 0; i < LCD_VIDEOBUF_SIZE; i++)
        Ram.Data[i] = 0x0001;   // Empty character
}

void Lcd_t::GotoCharXY(uint8_t x, uint8_t y) {
    CurrentPosition =  (x<<2)+(x<<1);   // = x * 6; move to x
    CurrentPosition += (y<<6)+(y<<5);   // = (*64)+(*32) = *96; move to y
}
void Lcd_t::GotoXY(uint8_t x, uint8_t y) {
    CurrentPosition =  x;               // move to x
    CurrentPosition += (y<<6)+(y<<5);   // = (*64)+(*32) = *96; move to y
}

void Lcd_t::DrawChar(uint8_t AChar, Invert_t AInvert) {
    uint8_t b;
    for(uint8_t i=0; i<6; i++) {
        b = Font_6x8_Data[AChar][i];
        if(AInvert == Inverted) b = ~b;
        Ram.Data[CurrentPosition++] = Distort(LCD_DATA, b);
        if (CurrentPosition >= LCD_VIDEOBUF_SIZE) CurrentPosition = 0;
    }
}

void Lcd_t::PrintString(const uint8_t x, const uint8_t y, const char* S, Invert_t AInvert) {
    GotoCharXY(x, y);
    while (*S != '\0')
        DrawChar(*S++, AInvert);
}
void Lcd_t::PrintStringLen(const char *S, uint16_t ALen, Invert_t AInvert) {
    for (uint16_t i=0; ((i<ALen) && (*S != '\0')); i++)
        DrawChar(*S++, AInvert);
}

void Lcd_t::PrintBigUint(const uint8_t y, uint16_t ANumber) {
    if(ANumber > 999) return;
    uint8_t digit = 0, x = 0;
    bool ShouldPrint = false;
    const uint16_t m[2] = {100, 10};

    for(uint8_t i=0; i<2; i++) {
        while (ANumber >= m[i]) {
            digit++;
            ANumber -= m[i];
        }
        if (digit != 0 || ShouldPrint) {
            DrawImage(x, y, &Font_32x48_Digits[digit][0], NotInverted);
            ShouldPrint = true;
            x += 32;
        }
        digit = 0;
    } // for
    DrawImage(x, y, &Font_32x48_Digits[ANumber][0], NotInverted);
}

void Lcd_t::PrintText() {
    Cls();
    IStartPageP = &TextToShow[0];
    // Fill first screen
    DisplayingText = true;
    EndOfScroll = false;
    GlobY = 7;      // Start show not from upper row
    FillScreen();
}
void Lcd_t::FillScreen(void) {
    char *StartP, *EndP;
    uint8_t x=0, y=GlobY, len=0;
    bool EndOfTextReached = false;

    StartP = IStartPageP;
    GotoXY(x, y);
    while(1) {
        EndP = strchr(StartP, ' ');
        if (EndP == NULL) {
            EndOfTextReached = true;
            EndP = strchr(StartP, '\0');
        }
        len = EndP - StartP;
        // Check if fit in current row
        if((x+len) > 16) {                      // Goto next row if not fit
            for (uint8_t i=x; i<16; i++) DrawChar(' ', NotInverted);    // Fill remainder of row with spaces
            y++;
            // Leading spaces
            if ((y == 1) && (GlobY == 0)) IStartPageP = StartP;   // Next time, start with this row
            if (y >= LCD_STR_HEIGHT) {          // End of screen
                if (GlobY != 0) GlobY--;
                return;
            }
            x = 0;
            GotoXY(0, y);
        }
        else if (EndOfTextReached && (y==0)) EndOfScroll = true;  // End of scroll in case of last word is in first row
        PrintStringLen(StartP, len, NotInverted);
        x += len;
        // Space after word
        if (x < 16) {
            DrawChar(' ', NotInverted);
            x++;
        }
        // Go further through the text
        if (!EndOfTextReached) StartP = EndP+1;
        else {
            for (uint8_t i=x; i<16; i++) DrawChar(' ', NotInverted);    // Fill remainder of row with spaces
            if (++y < LCD_STR_HEIGHT) { // Fill next row with spaces
                GotoXY(0, y);
                for (uint8_t i=0; i<16; i++) DrawChar(' ', NotInverted);
            }
            return;
        }
    } // while
}

void Lcd_t::DrawImage(const uint8_t x, const uint8_t y, const uint8_t* Img, Invert_t AInvert) {
    uint16_t i=0;
    uint8_t Width = Img[i++], Height = Img[i++];
    uint8_t b;
    for(uint8_t fy=y; fy<y+Height; fy++) {
        GotoXY(x, fy);
        for(uint8_t fx=x; fx<x+Width; fx++) {
            b = Img[i++];
            if(AInvert) b = ~b;
            Ram.Data[CurrentPosition++] = Distort(LCD_DATA, b);
            if (CurrentPosition >= LCD_VIDEOBUF_SIZE) CurrentPosition = 0;
        } // fx
    } // fy
}


// ============================ Inner use ======================================
uint16_t Lcd_t::Distort(uint16_t CmdDta, uint8_t AByte) {
    uint32_t Itmp = (uint32_t) AByte;
    __ASM volatile ("rbit %0, %1" : "=r" (Itmp) : "r" (Itmp)); // Reverse bit order
    __ASM volatile ("rev %0, %1" : "=r" (Itmp) : "r" (Itmp)); // Reverse byte order
    Itmp <<= 1;
    Itmp |= CmdDta;
    return (uint16_t)Itmp;
}

// ================================== Interrupts ===============================
//void DMA1_Channel2_IRQHandler(void) {
//    // Test on DMA1 Channel3 Transfer Complete interrupt
//    if(DMA_GetITStatus(DMA1_IT_TC2)) {
//        // Clear DMA Half Transfer, Transfer Complete and Global interrupt pending bits
//        DMA_ClearITPendingBit(DMA1_IT_GL2);
//        //DMA_Cmd(LCD_DMA_CHNL, DISABLE);
//        // Wait for current byte to finish transfer
//        while(USART_GetFlagStatus(LCD_USART, USART_FLAG_TC) == RESET);
//        Lcd.XCS_Hi();   // End transmission
//        USART_Cmd(LCD_USART, DISABLE);
//        Delay.Loop(18);
//        Lcd.LCD_DMA_Init();
//        DMA_Cmd(LCD_DMA_CHNL, ENABLE);          // Enable USARTy DMA TX Channel
//        Lcd.XCS_Lo();   // Start transmission
//        //DMA_Cmd(LCD_DMA_CHNL, ENABLE);
//        USART_Cmd(LCD_USART, ENABLE);
//    } // if
//}
