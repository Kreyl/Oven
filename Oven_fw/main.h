/*
 * main.h
 *
 *  Created on: 15 сент. 2014 г.
 *      Author: g.kruglov
 */

#pragma once

#include "ch.h"
#include "kl_lib.h"
#include "uart.h"
#include "evt_mask.h"
#include "board.h"

#define APP_NAME            "Oven"

#if 1 // ======== Thermoprofiles ========
#define T_PRECISION     4.0   // degrees
struct ThermoChunk_t {
    float tEnd;
    uint32_t DurationS;
};

class ThermoProfile_t {
private:
    ThermoChunk_t Preheat, Soak, Reflow, Cooling;
    bool TargetReached;
    uint32_t TimeStart;
    ThermoChunk_t *Curr;
public:
    uint8_t CalcTargetT(uint32_t TimemS, float CurrT, float *TargetT);
    void Reset() {
        Curr = &Preheat;
        TargetReached = false;
    }
    ThermoProfile_t(
            ThermoChunk_t APreheat,
            ThermoChunk_t ASoak,
            ThermoChunk_t AReflow,
            ThermoChunk_t ACooling) :
                Preheat(APreheat), Soak(ASoak), Reflow(AReflow), Cooling(ACooling),
                TargetReached(false), TimeStart(0),
                Curr(&Preheat)
    {}
};
#endif

enum Mode_t { modeProfile, modeManual };

class App_t {
private:
    uint32_t TimeStart;
    thread_t *PThread;
    float tHtr, tPCB;
    float CalcTemperature(uint32_t AdcCode);
    uint32_t PwrByHtrCtrl, PwrByPcbCtrl;
    bool IsOn = false;
    Mode_t Mode = modeManual;
    void CheckIfFanRequired(float tPCB, float tRequired);
public:
    float tHtrManual = 180;
    void LoadProfiles();
    void SaveProfiles();
    void Start();
    void Stop();
    void OnBtnMode();
    // Eternal methods
    void InitThread() { PThread = chThdGetSelfX(); }
    void SignalEvt(eventmask_t Evt) {
        chSysLock();
        chEvtSignalI(PThread, Evt);
        chSysUnlock();
    }
    void SignalEvtI(eventmask_t Evt) { chEvtSignalI(PThread, Evt); }
    void OnCmd(Shell_t *PShell);
    // Inner use
    void ITask();
};

extern App_t App;
