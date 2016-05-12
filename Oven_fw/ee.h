/*
 * ee.h
 *
 *  Created on: 3 мая 2016 г.
 *      Author: Kreyl
 */

#pragma once

#include "board.h"
#include "kl_lib.h"

class EE_t {
private:

public:
    void Init() {
        //PinSetupOut(EE_PWR, omPushPull, pudNone);
    }
    void On()   {
//        PinSet(EE_PWR);
//        chThdSleepMilliseconds(1);
    }
    void Off()  {
//        PinClear(EE_PWR);
    }
};

extern EE_t ee;
