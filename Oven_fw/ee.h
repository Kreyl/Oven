/*
 * ee.h
 *
 *  Created on: 3 мая 2016 г.
 *      Author: Kreyl
 */

#pragma once

#include "board.h"
#include "kl_lib.h"
#include "i2cL476.h"

#define EE_BLOCK_SZ         256
#define EE_PAGE_SZ          8

#define EE_BLOCK1_DEV_ADDR  0x50
#define EE_BLOCK2_DEV_ADDR  0x51

class EE_t {
private:
    i2c_t *pi2c;
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

    uint8_t Read(uint32_t Addr, void *ptr, uint32_t Sz);
    uint8_t Write(uint32_t Addr, void *ptr, uint32_t Sz);
    EE_t(i2c_t *Ai2c) : pi2c(Ai2c) {}
};
