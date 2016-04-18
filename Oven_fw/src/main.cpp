/*
 * File:   main.cpp 
 * Author: Kreyl
 *
 * Created on May 27, 2011, 6:37 PM
 */

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "delay_util.h"
#include "uart.h"
#include "lcd110x.h"
#include "lcd_font.h"
#include "images.h"
#include "adc.h"

// =============================== Prototypes ==================================
void GeneralInit(void);

#define HEATER_PERIOD   504 // ms
#define HEATER_MIN_ADC  504
class Heater_t {
private:
    uint32_t Timer;
    void On(void)  { GPIOB->BSRR = GPIO_Pin_8; }
    void Off(void) { GPIOB->BRR  = GPIO_Pin_8; }
public:
    uint16_t NeededADC;
    uint8_t CurrentHeatIndx;
    bool IsCooling;
    void Task(void);
    void Init(void);
} Heater;

struct DgAdc_t {
    uint16_t Adc;
    uint16_t Degrees;
};
#define HEAT_ARR_COUNT  9
const DgAdc_t HeatArray[] = {
        {1,    0  },
        {1850, 150},
        {2207, 175},
        {2550, 200},
        {2940, 225},
        {3290, 250},
        {3590, 275},
        {3900, 300},
        {4080, 315},
};

struct Keys_t {
    uint32_t Timer;
    bool UpIsDown, DownIsDown, CoolIsDown;
    bool PinUpIsDown(void)   { return !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15); }
    bool PinDownIsDown(void) { return !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14); }
    bool PinCoolIsDown(void) { return !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13); }
    void Task(void);
    void Init(void);
} Keys;

// ============================= Implementation ================================
int main(void) {
    UART_Init();
    Delay.ms(100);
    UART_PrintString("Can offer only oven\r");

    GeneralInit();
    Lcd.DrawImage(0,0, ImageLogo, NotInverted);
    Delay.ms(999);
    Lcd.Cls();

    //Lcd.DrawImage(0, 0, Font_32x48_Digits[0], NotInverted);
    Lcd.DrawImage(0, 2,  Font_32x48_O, NotInverted);
    Lcd.DrawImage(33, 2, Font_32x48_F, NotInverted);
    Lcd.DrawImage(65, 2, Font_32x48_F, NotInverted);

    // ==== Main cycle ====
    while(1) {
        Keys.Task();
        Heater.Task();
        Lcd.Task();
    } // while(1)
    return 0;
}

void GeneralInit(void) {
    // Disable JTAG
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    // Configure two bits for preemption priority
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);

    Delay.Init();
    Lcd.Init();
    Adc.Init();
    Keys.Init();
    Heater.Init();

    // ==== Backlight ====
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIOB->BSRR = GPIO_Pin_0;   // Switch backlight on
}

// ================================ Events =====================================
void EVENT_KeyUpPressed(void) {
    Heater.IsCooling = false;
    if (Heater.CurrentHeatIndx < (HEAT_ARR_COUNT-1)) Heater.CurrentHeatIndx++;
    Heater.NeededADC = HeatArray[Heater.CurrentHeatIndx].Adc;
    Lcd.Cls();
    Lcd.PrintBigUint(2, HeatArray[Heater.CurrentHeatIndx].Degrees);
}
void EVENT_KeyDownPressed(void) {
    Heater.IsCooling = false;
    if (Heater.CurrentHeatIndx !=0) Heater.CurrentHeatIndx--;
    Heater.NeededADC = HeatArray[Heater.CurrentHeatIndx].Adc;
    Lcd.Cls();
    Lcd.PrintBigUint(2, HeatArray[Heater.CurrentHeatIndx].Degrees);
}
void EVENT_KeyCoolPressed(void) {
    Heater.IsCooling = true;
    Heater.NeededADC = 1;
    Lcd.Cls();
    Lcd.DrawImage(0, 2,  Font_32x48_O, NotInverted);
    Lcd.DrawImage(33, 2, Font_32x48_F, NotInverted);
    Lcd.DrawImage(65, 2, Font_32x48_F, NotInverted);
}

// ================================ Heater =====================================
void Heater_t::Task() {
    if (!Delay.Elapsed(&Timer, HEATER_PERIOD)) return;
    // Switch heater on or off depending on current temperature
    uint16_t AdcValue = Adc.AverageValues();
    UART_StrUint("ADC: ", AdcValue);
    if (IsCooling) {
        this->Off();
        if (AdcValue > HEATER_MIN_ADC) Lcd.PrintString(0, 1, "Cooling...", NotInverted);
        else Lcd.PrintString(0, 1, "Now it's cool", NotInverted);
    }
    else {
        if (AdcValue > NeededADC) {
            this->Off();
            Lcd.PrintString(0, 1, "          ", NotInverted);
        }
        else {
            this->On();
            Lcd.PrintString(0, 1, "Heating...", Inverted);
        }
    }
}

void Heater_t::Init() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

// ================================= Keys ======================================
void Keys_t::Init() {   // PB15 = UP, PB14=DOWN, PB13=Cool
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    UpIsDown = false;
    DownIsDown = false;
    CoolIsDown = false;
}

void Keys_t::Task() {
    if(!Delay.Elapsed(&Timer, 108)) return;
    if(PinUpIsDown() && !UpIsDown) {
        UpIsDown = true;
        EVENT_KeyUpPressed();
    }
    else if(!PinUpIsDown() && UpIsDown) UpIsDown = false;

    if(PinDownIsDown() && !DownIsDown) {
        DownIsDown = true;
        EVENT_KeyDownPressed();
    }
    else if(!PinDownIsDown() && DownIsDown) DownIsDown = false;

    if(PinCoolIsDown() && !CoolIsDown) {
        CoolIsDown = true;
        EVENT_KeyCoolPressed();
    }
    else if(!PinCoolIsDown() && CoolIsDown) CoolIsDown = false;
}
