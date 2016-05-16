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
struct ThermoChunk_t {
    float SpeedMin, SpeedMax;
    float tEnd;
    uint32_t DurationMinS, DurationMaxS;
};

struct ThermoProfile_t {
    ThermoChunk_t Preheat, Soak, Reflow, Cooling;
};

#define PROFILES_CNT_MAX    4
struct Profiles_t {
    uint32_t Cnt;
    ThermoProfile_t Prof[PROFILES_CNT_MAX];
};
#endif

class App_t {
private:
    thread_t *PThread;
    float tHeater, tPCB;
    float CalcTemperature(uint32_t AdcCode);
    Profiles_t Profiles;
public:
    void LoadProfiles();
    void SaveProfiles();

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
