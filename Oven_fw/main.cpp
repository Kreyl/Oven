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
#define HEATER_TOP_T        300 // degrees Centigrade

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
PID_t PidPcb {5,  0.05, 300, -300,  80};

const PinOutputPWM_t Heater(HEATER_SETUP);
const PinOutputPWM_t Fan(FAN_SETUP);

extern MCP3551_t AdcHeater;
extern MCP3551_t AdcPCB;
AdcFilter_t<10> HtrFilter, PcbFilter;

ThermoProfile_t TProfile = {
        { 130, 1  },    // Preheat zone: t end = 120...150 dg
        { 130, 99 },    // Activation zone: 120...150 dg
        { 170, 36 },    // Reflow zone: 200...230 dg
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
    App.SetMode(modeProfile);

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
            AdcHeater.StartMeasurement();
            AdcPCB.StartMeasurement();
        }

        if(EvtMsk & EVTMSK_ADC_HEATER_DONE) {
            // Filter ADC value
            if(HtrFilter.PutNewAndCheck(AdcHeater.LastData) == OK) {
                tHtr = CalcTemperature(HtrFilter.GetFiltered());
    //            Uart.Printf("t=%.1f\r", tHeater);
                float PwrPercent = PidHtr.Calculate(tHtr);
                if(PwrPercent >= 0 and IsOn) {
                    PwrByHtrCtrl = (uint32_t)(PwrPercent / 2);   // Granulation 2%
                    PwrByHtrCtrl *= 200;  // 0%...100% -> 0...10000
                    if(Mode == modeProfile) {
                        // Select lower pwr setting
                        uint32_t Pwr = (PwrByPcbCtrl < PwrByHtrCtrl)? PwrByPcbCtrl : PwrByHtrCtrl;
                        Heater.Set(Pwr);
                    }
                    else Heater.Set(PwrByHtrCtrl);
                }
                else {
                    PwrByHtrCtrl = 0;
                    Heater.Set(0);
                }
                ShowTHtr(tHtr);
            } // if filter done
        }

        if(EvtMsk & EVTMSK_ADC_PCB_DONE) {
            if(PcbFilter.PutNewAndCheck(AdcPCB.LastData) == OK) {
                tPCB = CalcTemperature(PcbFilter.GetFiltered());
                uint32_t TimemS = (chVTGetSystemTimeX() - TimeStart) / 10;
                if(Mode == modeProfile) {
                    float PwrPercent = 0;
                    if(IsOn) {
            //            Uart.Printf("Pcb: %.1f\r", tPCB);
                        ProfileRslt_t Rslt = TProfile.CalcTargetT(TimemS, tPCB, &PidPcb.TargetValue);
                        if(Rslt == tprStop) App.Stop();
                        else {  // New or proceeding
                            // Draw lines if new
                            if(Rslt == tprNew) {
                                Chart.AddLineHoriz(PidPcb.TargetValue, clGrey);
                                if(TProfile.State != pstPreheat) Chart.AddLineVert(TimemS, clGrey);
                            }
                            // Heat if not cooling
                            if(TProfile.State == pstCooling) PwrPercent = 0;
                            else PwrPercent = PidPcb.Calculate(tPCB);
                            // Process pwr control
                            if(PwrPercent >= 0) {
                                PwrByPcbCtrl = (uint32_t)(PwrPercent / 2);   // Granulation 2%
                                PwrByPcbCtrl *= 200;  // 0%...100% -> 0...10000
                                // Select lowest pwr setting
                                uint32_t Pwr = (PwrByPcbCtrl < PwrByHtrCtrl)? PwrByPcbCtrl : PwrByHtrCtrl;
                                Heater.Set(Pwr);
                            }
                            else {
                                PwrByPcbCtrl = 0;
                                Heater.Set(0);
                            }
                            CheckIfFanRequired(tPCB, PidPcb.TargetValue);   // Fan
                        } // ret == OK
                    } // if on
                    else {
                        PwrByPcbCtrl = 0;
                        Heater.Set(0);
                    }
                    Uart.Printf("%u; %.1f; %.1f;  %.1f; %.1f\r\n", TimemS, tPCB, tHtr, PidPcb.TargetValue, PwrPercent);
                } // if modeProfile
                else {
                    Uart.Printf("%u; %.1f; %.1f\r\n", TimemS, tPCB, tHtr);
                }

                // Show everything
                ShowTPcb(tPCB);

                if(Mode == modeProfile) {
                    if(IsOn) ShowTime(TimemS);
                    SeriesTPcb.AddPoint(TimemS, tPCB);
                    SeriesTHtr.AddPoint(TimemS, tHtr);
                }
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

void App_t::CheckIfFanRequired(float tPCB, float tRequired) {
    if     (tPCB > tRequired + 11) Fan.Set(100);
    else if(tPCB > tRequired + 9 ) Fan.Set(70);
    else if(tPCB > tRequired + 4 ) Fan.Set(50);
    else Fan.Set(0);
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
    ShowHeaterOn();
    IsOn = true;
    TimeStart = chVTGetSystemTimeX();      // reset time counter
    if(Mode == modeProfile) {
        TProfile.Reset();
        Chart.Clear();
        PidHtr.TargetValue = HEATER_TOP_T;
    }
    else {
        PidHtr.TargetValue = tHtrManual;
    }
}
void App_t::Stop() {
    ShowHeaterOff();
    Fan.Set(0);
    IsOn = false;
    if(Mode == modeProfile) {
        PidPcb.TargetValue = 0;
    }
    else {
        PidHtr.TargetValue = 0;
    }
}

void App_t::SetMode(Mode_t NewMode) {
    Stop();
    Mode = NewMode;
    if(Mode == modeManual) Gui.DrawPage(1);
    else Gui.DrawPage(0);
}

// ============================== Thermoprofile ================================
ProfileRslt_t ThermoProfile_t::CalcTargetT(uint32_t TimemS, float CurrT, float *TargetT) {
    ProfileRslt_t Rslt = tprProceeding;
    if(TargetReached) { // If required temperature was reached
        uint32_t DurationForNow = TimemS / 1000 - TimeStart;
        // Check if time to go to next chunk
        if(DurationForNow >= Chunk[State].DurationS) {
            if(State == pstCooling) Rslt = tprStop;  // cooling completed
            else {
                // Switch to next chunk
                State = (ProfileState_t)((int)State + 1);
                TargetReached = false;
                Rslt = tprNew;
            } // if not cooling
        } // if duration
    } // if reached
    else {
        float tEnd = Chunk[State].tEnd;
        if( (State == pstCooling and CurrT <= tEnd) or  // When cooling, precision is not required
            (State != pstCooling and CurrT >= (tEnd - T_PRECISION) and CurrT <= (tEnd + T_PRECISION))) {
            // Target temperature reached
            TargetReached = true;
            TimeStart = TimemS / 1000;  // Start time count
        }
    }
    *TargetT = Chunk[State].tEnd;
    // Send new after beginning
    if(JustResetted) {
        JustResetted = false;
        Rslt = tprNew;
    }
    return Rslt;
}

#if 1 // =========================== Interface =================================
void OnBtnStart(const Control_t *p) {
    App.Start();
}
void OnBtnStop(const Control_t *p) {
    App.Stop();
}
void OnBtnMode(const Control_t *p) {
    if(App.Mode == modeProfile) App.SetMode(modeManual);
    else App.SetMode(modeProfile);
}

void OnBtnFan(const Control_t *p) {
    static bool FanIsOn = false;
    if(FanIsOn) {
        FanIsOn = false;
        Fan.Set(0);
    }
    else {
        FanIsOn = true;
        Fan.Set(100);
    }
    ShowFanStatus(FanIsOn);
}

// Manual heater control
void OnBtnHeater(const Control_t *p) {
    if(App.IsOn) App.Stop();
    else App.Start();
}

void OnBtnHtrMinus(const Control_t *p) {
    if(App.tHtrManual > 30) App.tHtrManual -= 5;
    ShowTHtrManual(App.tHtrManual);
    PidHtr.TargetValue = App.tHtrManual;
}
void OnBtnHtrPlus(const Control_t *p) {
    if(App.tHtrManual < HEATER_TOP_T) App.tHtrManual += 5;
    ShowTHtrManual(App.tHtrManual);
    PidHtr.TargetValue = App.tHtrManual;
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
