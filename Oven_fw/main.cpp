/*
 * main.cpp
 *
 *  Created on: 20 февр. 2014 г.
 *      Author: g.kruglov
 */

#include "main.h"
#include "hal.h"
#include "ee.h"
#include "i2cL476.h"
#include "board.h"
#include "gui.h"
#include "mcp3551.h"

#define MEASURE_PERIOD_MS   99

App_t App;
TmrKL_t TmrMeasurement;
EE_t ee {&i2c3};

const MCP3551_t AdcHeater(ADC_H_SPI, ADC_H_CS, ADC_H_CLK, ADC_H_SDO, ADC_H_SPI_AF);
const MCP3551_t AdcPCB   (ADC_P_SPI, ADC_P_CS, ADC_P_CLK, ADC_P_SDO, ADC_P_SPI_AF);

const ThermoProfile_t TPDefault = {
        { 1,   3, 160, 0,  90 },
        { 0.5, 1, 180, 0,  160 },
        { 1,   4, 240, 30, 40 },
        { -2, -6, 80,  0,  300 }
};


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

    // Read profiles from EE
    App.LoadProfiles();

    Gui.Init();
    Gui.DrawPage(0);

    AdcHeater.Init();
    AdcPCB.Init();

    TmrMeasurement.InitAndStart(chThdGetSelfX(), MS2ST(MEASURE_PERIOD_MS), EVTMSK_MEASURE_TIME, tktPeriodic);

    // Main cycle
    App.ITask();
}

__noreturn
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
            AdcHeater.StartMeasurement();
            AdcPCB.StartMeasurement();
        }

        if(EvtMsk & EVTMSK_ADC_HEATER_DONE) {
            uint32_t AdcCode = AdcHeater.GetData();
            tHeater = CalcTemperature(AdcCode);
//            int32_t T = (int32_t)(tHeater * 10);
//            Uart.Printf("Htr: %X;  T=%d\r", AdcCode, T);
        }
        if(EvtMsk & EVTMSK_ADC_PCB_DONE) {
            uint32_t AdcCode = AdcPCB.GetData();
            tHeater = CalcTemperature(AdcCode);
//            int32_t T = (int32_t)(tHeater * 10);
//            Uart.Printf("Pcb: %X;  T=%d\r", AdcCode, T);
        }

        if(EvtMsk & EVTMSK_UART_NEW_CMD) {
            OnCmd((Shell_t*)&Uart);
            Uart.SignalCmdProcessed();
        }

    } // while true
}

float App_t::CalcTemperature(uint32_t AdcCode) {
    float Code = AdcCode & 0x3FFFFF;   // Clear MS bits;
    float R = 10000 * (Code / ((1<<20) - Code));
    return (R - 100)/0.385; // 100R at 0C, 0.385 ohm/degree
}

void App_t::LoadProfiles() {
    ee.Read(0, &Profiles.Cnt, 4);
    Uart.Printf("ProfCnt = %u\r", Profiles.Cnt);
    if(Profiles.Cnt == 0 or Profiles.Cnt > PROFILES_CNT_MAX) {
        Profiles.Cnt = 1;
        Profiles.Prof[0] = TPDefault;
        SaveProfiles();
    }
}

void App_t::SaveProfiles() {
    ee.Write(0, &Profiles, sizeof(Profiles));
}

#if 1 // ======================= Command processing ============================
void App_t::OnCmd(Shell_t *PShell) {
	Cmd_t *PCmd = &PShell->Cmd;
    __attribute__((unused)) int32_t dw32 = 0;  // May be unused in some configurations
    Uart.Printf("\r%S\r", PCmd->Name);
    // Handle command
    if(PCmd->NameIs("Ping")) {
        PShell->Ack(OK);
    }

    else PShell->Ack(CMD_UNKNOWN);
}
#endif

#if 1 // ============================= IRQs ====================================
extern "C" {
// ==== ADC Heater ====
CH_IRQ_HANDLER(Vector9C) {  // EXTI Line[9:5] interrupts
    CH_IRQ_PROLOGUE();
    AdcHeater.DisableIRQ();
    chSysLockFromISR();
    App.SignalEvtI(EVTMSK_ADC_HEATER_DONE);
    chSysUnlockFromISR();
    CH_IRQ_EPILOGUE();
}
// ==== ADC PCB ====
CH_IRQ_HANDLER(VectorE0) {  // EXTI Line[15:10] interrupts
    CH_IRQ_PROLOGUE();
    AdcPCB.DisableIRQ();
    chSysLockFromISR();
    App.SignalEvtI(EVTMSK_ADC_PCB_DONE);
    chSysUnlockFromISR();
    CH_IRQ_EPILOGUE();
}
} // extern c

#endif
