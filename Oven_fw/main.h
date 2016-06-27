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
#define T_PRECISION     3.0   // degrees
struct ThermoChunk_t {
    float tEnd;
    uint32_t DurationS;
};

enum ProfileRslt_t { tprNew, tprProceeding, tprStop };
enum ProfileState_t { pstPreheat=0, pstActivation=1, pstReflow=2, pstCooling=3 };

class ThermoProfile_t {
private:
    ThermoChunk_t Chunk[4];
    bool TargetReached;
    uint32_t TimeStart;
    bool JustResetted;
public:
    ProfileRslt_t CalcTargetT(uint32_t TimemS, float CurrT, float *TargetT);
    ProfileState_t State;
    void Reset() {
        State = pstPreheat;
        TargetReached = false;
        JustResetted = true;
    }
    ThermoProfile_t(
            ThermoChunk_t APreheat,
            ThermoChunk_t AActivation,
            ThermoChunk_t AReflow,
            ThermoChunk_t ACooling) :
                TargetReached(false), TimeStart(0), JustResetted(true), State(pstPreheat)
    {
        Chunk[pstPreheat]    = APreheat;
        Chunk[pstActivation] = AActivation;
        Chunk[pstReflow]     = AReflow;
        Chunk[pstCooling]    = ACooling;
    }
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
    void CheckIfFanRequired(float tPCB, float tRequired);
public:
    bool IsOn = false;
    float tHtrManual = 180;
    Mode_t Mode = modeProfile;
    void SetMode(Mode_t NewMode);
    void LoadProfiles();
    void SaveProfiles();
    void Start();
    void Stop();
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
