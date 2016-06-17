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
#include "kl_pid.h"

#define MEASURE_PERIOD_MS   999

App_t App;
TmrKL_t TmrMeasurement {MS2ST(MEASURE_PERIOD_MS), EVTMSK_MEASURE_TIME, tktPeriodic};
EE_t ee {&i2c3};

PID_t PidHtr {6, 0, 50};

const PinOutputPWM_t Heater(HEATER_SETUP);
const PinOutputPWM_t Fan(FAN_SETUP);

extern MCP3551_t AdcHeater;
extern MCP3551_t AdcPCB;

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

    Heater.Init();
    Heater.SetFrequencyHz(1);
    PidHtr.SetTarget(250);

    Fan.Init();
    Fan.SetFrequencyHz(30000);

    TmrMeasurement.InitAndStart();

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
//            Uart.Printf("Code: %X\r", AdcCode);
            tHeater = CalcTemperature(AdcHeater.LastData);
//            Uart.Printf("t=%.1f\r", tHeater);
//            int32_t T = (int32_t)(tHeater * 10);
            float HtrPwrPercent = PidHtr.Calculate(tHeater);
            if(HtrPwrPercent >= 0) {
                uint32_t HtrPwr = (uint32_t)(HtrPwrPercent / 2);
                HtrPwr *= 200;
                if(IsOn) {
//                    Uart.Printf("Pwr=%u\r", HtrPwr);
                    Heater.Set(HtrPwr);
                }
            }
            else Heater.Set(0);

//            Uart.Printf("%u;%d\r\n", chVTGetSystemTime(), T);
        }
        if(EvtMsk & EVTMSK_ADC_PCB_DONE) {
            tPCB = CalcTemperature(AdcPCB.LastData);
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
//    Uart.Printf("ProfCnt = %u\r", Profiles.Cnt);
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
//    Uart.Printf("\r%S\r", PCmd->Name);
    // Handle command
    if(PCmd->NameIs("Ping")) {
        PShell->Ack(OK);
    }

    else if(PCmd->NameIs("Heater")) {
        if(PCmd->GetNextInt32(&dw32) == OK) {
            Heater.Set(dw32);
            PShell->Ack(OK);
        }
        else PShell->Ack(CMD_ERROR);
    }

    else if(PCmd->NameIs("Fan")) {
        if(PCmd->GetNextInt32(&dw32) == OK) {
            Fan.Set(dw32);
            PShell->Ack(OK);
        }
        else PShell->Ack(CMD_ERROR);
    }

    else if(PCmd->NameIs("On")) IsOn = true;
    else if(PCmd->NameIs("Off")) {
        IsOn = false;
        Heater.Set(0);
    }

    else PShell->Ack(CMD_UNKNOWN);
}
#endif
