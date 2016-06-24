/*
 * main.cpp
 *
 *  Created on: 20 февр. 2014 г.
 *      Author: g.kruglov
 */

#include "gui_engine.h"
#include "interface.h"
#include "main.h"
#include "hal.h"
#include "ee.h"
#include "i2cL476.h"
#include "board.h"
#include "mcp3551.h"
#include "kl_pid.h"

#define MEASURE_PERIOD_MS   100
#define HEATER_TOP_T        315 // degrees Centigrade

App_t App;
TmrKL_t TmrMeasurement {MS2ST(MEASURE_PERIOD_MS), EVTMSK_MEASURE_TIME, tktPeriodic};
EE_t ee {&i2c3};

template <uint32_t Number>
class AdcFilter_t {
private:
    uint32_t Counter, IValue;
public:
    uint8_t PutNewAndCheck(uint32_t NewValue) {
        IValue += NewValue;
        Counter++;
        if(Counter >= Number) return OK;
        else return BUSY;
    }
    uint32_t GetFiltered() {
        uint32_t Rslt = IValue / Counter;
        IValue = 0;
        Counter = 0;
        return Rslt;
    }
};

// Kp,  Ki, MaxI, MinI,  Kd
PID_t PidHtr {4,  0.1, 300, -300,  80};
PID_t PidPcb {5,  0.1, 300, -300,  80};

const PinOutputPWM_t Heater(HEATER_SETUP);
const PinOutputPWM_t Fan(FAN_SETUP);

extern MCP3551_t AdcHeater;
extern MCP3551_t AdcPCB;
AdcFilter_t<10> HtrFilter, PcbFilter;

ThermoProfile_t TProfile = {
        { 150, 60 },
        { 180, 90 },
        { 200, 30 },
        { 80,  20 }
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

    // Read profiles from EE
//    App.LoadProfiles();

    Gui.Init();
    Gui.DrawPage(0);
    Chart.Reset();

    AdcHeater.Init();
    AdcPCB.Init();

    Heater.Init();
    Heater.SetFrequencyHz(1);
    PidHtr.TargetValue = HEATER_TOP_T; // Do not heat above

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
            AdcPCB.StartMeasurement();
            AdcHeater.StartMeasurement();
        }

        if(EvtMsk & EVTMSK_ADC_HEATER_DONE) {
            // Filter ADC value
            if(HtrFilter.PutNewAndCheck(AdcHeater.LastData) == OK) {
                tHeater = CalcTemperature(HtrFilter.GetFiltered());
    //            Uart.Printf("t=%.1f\r", tHeater);
                float PwrPercent = PidHtr.Calculate(tHeater);
                if(PwrPercent >= 0) {
                    PwrByHtrCtrl = (uint32_t)(PwrPercent / 2);   // Granulation 2%
                    PwrByHtrCtrl *= 200;  // 0%...100% -> 0...10000
                    // Select lower pwr setting
                    uint32_t Pwr = (PwrByPcbCtrl < PwrByHtrCtrl)? PwrByPcbCtrl : PwrByHtrCtrl;
                    Heater.Set(Pwr);
                }
                else {
                    PwrByHtrCtrl = 0;
                    Heater.Set(0);
                }
                ShowTHtr(tHeater);
            } // if filter done
        }

        if(EvtMsk & EVTMSK_ADC_PCB_DONE) {
            if(PcbFilter.PutNewAndCheck(AdcPCB.LastData) == OK) {
                tPCB = CalcTemperature(PcbFilter.GetFiltered());
                uint32_t TimemS = (chVTGetSystemTimeX() - TimeStart) / 10;
                float PwrPercent = 0;
                if(IsOn) {
        //            Uart.Printf("Pcb: %.1f\r", tPCB);
                    float tToSet;
                    if(TProfile.CalcTargetT(TimemS, tPCB, &tToSet) == LAST) App.Stop();
                    else { // OK or NEW, i.e. proceed
                        if(tToSet != PidPcb.TargetValue) {
                            PidPcb.TargetValue = tToSet;
                            Chart.AddLineHoriz(tToSet, clGrey);
                        }
                        PwrPercent = PidPcb.Calculate(tPCB);
                        if(PwrPercent >= 0) {
                            PwrByPcbCtrl = (uint32_t)(PwrPercent / 2);   // Granulation 2%
                            PwrByPcbCtrl *= 200;  // 0%...100% -> 0...10000
                            // Select lower pwr setting
                            uint32_t Pwr = (PwrByPcbCtrl < PwrByHtrCtrl)? PwrByPcbCtrl : PwrByHtrCtrl;
                            Heater.Set(Pwr);
                        }
                        else {
                            PwrByPcbCtrl = 0;
                            Heater.Set(0);
                        }
                    } // ret == OK
                } // if on
                else {
                    PwrByPcbCtrl = 0;
                    Heater.Set(0);
                }

                ShowTPcb(tPCB);
                Uart.Printf("%u; %.1f; %.1f; %.1f\r\n", TimemS, tPCB, tHeater, PwrPercent);
                Chart.AddPoint(0, TimemS, tPCB);
                Chart.AddPoint(1, TimemS, tHeater);
            } // if filter done
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

//void App_t::LoadProfiles() {
//    ee.Read(0, &Profiles.Cnt, 4);
////    Uart.Printf("ProfCnt = %u\r", Profiles.Cnt);
//    if(Profiles.Cnt == 0 or Profiles.Cnt > PROFILES_CNT_MAX) {
//        Profiles.Cnt = 1;
//        Profiles.Prof[0] = TPDefault;
//        SaveProfiles();
//    }
//}
//
//void App_t::SaveProfiles() {
//    ee.Write(0, &Profiles, sizeof(Profiles));
//}

void App_t::Start() {
    Fan.Set(0);
    TProfile.Reset();
    ShowHeaterOn();
    Chart.Reset();
    IsOn = true;
    //Chart.AddLineHoriz(WorkTarget, clGrey);
    TimeStart = chVTGetSystemTimeX();      // reset time counter
}
void App_t::Stop() {
    PidPcb.TargetValue = 0;
    ShowHeaterOff();
    IsOn = false;
}

// Thermoprofile
uint8_t ThermoProfile_t::CalcTargetT(uint32_t TimemS, float CurrT, float *TargetT) {
    // Calculate current chunk
//    uint32_t Ts = TimemS / 1000;
//    if(Ts <= Preheat.DurationS) {
//        Curr = &Preheat;
//    }
//    else if(Ts <= (Preheat.DurationS + Soak.DurationS)) {
//        Curr = &Soak;
//        Ts -= Preheat.DurationS;    // Calculate time within chunk
//    }
//    else if(Ts <= (Preheat.DurationS + Soak.DurationS + Reflow.DurationS)) {
//        Curr = &Reflow;
//        Ts -= Preheat.DurationS + Soak.DurationS;
//    }
//    else {
//        Curr = &Cooling;
//        Ts -= Preheat.DurationS + Soak.DurationS + Reflow.DurationS;
//    }
    uint8_t Rslt = OK;
    if(TargetReached) {
        uint32_t Ts = TimemS / 1000;
        Ts -= TimeStart;
        // Check if time to go to next chunk
        if(Ts >= Curr->DurationS) {
            if(Curr == &Cooling) Rslt = LAST;  // cooling completed
            else {
                Curr++;
                TargetReached = false;
                Rslt = NEW;
            } // if not curr
        } // if enough time
    } // if reached
    else {
        if(CurrT >= (Curr->tEnd - T_PRECISION) and CurrT <= (Curr->tEnd + T_PRECISION)) {
            // Target temperature reached
            TargetReached = true;
            TimeStart = TimemS / 1000;  // Start time count
        }
    }
    *TargetT = Curr->tEnd;
    return Rslt;
}

#if 1 // =========================== Interface =================================
void OnBtnStart(const Control_t *p) {
    App.Start();
}
void OnBtnStop(const Control_t *p) {
    App.Stop();
}
#endif

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
        }
        else PShell->Ack(CMD_ERROR);
    }

    else if(PCmd->NameIs("On"))  App.Start();
    else if(PCmd->NameIs("Off")) App.Stop();

//    else if(PCmd->NameIs("Target")) {
//        if(PCmd->GetNextInt32(&dw32) == OK) {
//            App.WorkTarget = dw32;
//        }
//        else PShell->Ack(CMD_ERROR);
//    }

    else PShell->Ack(CMD_UNKNOWN);
}
#endif
