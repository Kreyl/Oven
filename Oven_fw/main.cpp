/*
 * main.cpp
 *
 *  Created on: 20 ����. 2014 �.
 *      Author: g.kruglov
 */

#include "main.h"
#include "hal.h"
#include "ee.h"
#include "i2cL476.h"
#include "board.h"
#include "gui.h"
#include "mcp3551.h"

#define MEASURE_PERIOD_MS   45

App_t App;
TmrKL_t TmrMeasurement;

const MCP3551_t AdcHeater(SPI1, ADC_H_CS, ADC_H_CLK, ADC_H_SDO);

int main(void) {
    // ==== Setup clock frequency ====
    __unused uint8_t ClkResult = 1;
//    Clk.SetupFlashLatency(64);  // Setup Flash Latency for clock in MHz
//    Clk.EnablePrefetch();
//    // 12 MHz/6 = 2; 2*192 = 384; 384/6 = 64 (preAHB divider); 384/8 = 48 (USB clock)
//    Clk.SetupPLLDividers(6, 192, pllSysDiv6, 8);
//    // 64/1 = 64 MHz core clock. APB1 & APB2 clock derive on AHB clock; APB1max = 42MHz, APB2max = 84MHz
//    // Keep APB freq at 32 MHz to left peripheral settings untouched
//    Clk.SetupBusDividers(ahbDiv1, apbDiv2, apbDiv2);
//    if((ClkResult = Clk.SwitchToPLL()) == 0) Clk.HSIDisable();
    Clk.UpdateFreqValues();

    // Init OS
    halInit();
    chSysInit();

    // ==== Init hardware ====
    Uart.Init(115200, UART_GPIO, UART_TX_PIN, UART_GPIO, UART_RX_PIN);
    Uart.Printf("\r%S %S\r", APP_NAME, BUILD_TIME);
    Clk.PrintFreqs();
//    if(ClkResult != 0) Uart.Printf("\rXTAL failure");

    App.InitThread();

    i2c3.Init();    // Touch controller and EEPROM
//    i2c3.ScanBus();

    //    ee.Init();
    //    ee.On();

    Gui.Init();
    Gui.DrawPage(0);

    AdcHeater.Init();

    TmrMeasurement.InitAndStart(chThdGetSelfX(), MS2ST(MEASURE_PERIOD_MS), EVTMSK_MEASURE_TIME, tktPeriodic);

    // Main cycle
    App.ITask();
}

__attribute__ ((__noreturn__))
void App_t::ITask() {
    while(true) {
        __unused uint32_t EvtMsk = chEvtWaitAny(ALL_EVENTS);
#if 0 // ==== USB ====
        if(EvtMsk & EVTMSK_USB_READY) {
            Uart.Printf("\rUsbReady");
        }
        if(EvtMsk & EVTMSK_USB_SUSPEND) {
            Uart.Printf("\rUsbSuspend");
        }
#endif
        if(EvtMsk & EVTMSK_MEASURE_TIME) {
            uint32_t ADCValue = AdcHeater.GetData();
            ADCValue &= 0x3FFFFF;   // Clear MS bits
            float Code = ADCValue;
//            float Te
            float R = 10000 * (Code / ((1<<20) - Code));
            float fT = 10*(R - 100)/0.385;  // == *10 dg C
            int32_t T = (int32_t)fT;

            Uart.Printf("ADC: %X;  T=%d\r", ADCValue, T);

        }

        if(EvtMsk & EVTMSK_UART_NEW_CMD) {
            OnCmd((Shell_t*)&Uart);
            Uart.SignalCmdProcessed();
        }

    } // while true
}

#if 1 // ======================= Command processing ============================
void App_t::OnCmd(Shell_t *PShell) {
	Cmd_t *PCmd = &PShell->Cmd;
    __attribute__((unused)) int32_t dw32 = 0;  // May be unused in some configurations
    Uart.Printf("\r%S\r", PCmd->Name);
    // Handle command
    if(PCmd->NameIs("Ping")) PShell->Ack(OK);

    else PShell->Ack(CMD_UNKNOWN);
}
#endif
