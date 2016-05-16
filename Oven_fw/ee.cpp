/*
 * ee.cpp
 *
 *  Created on: 3 мая 2016 г.
 *      Author: Kreyl
 */

#include "ee.h"
#include "uart.h"

uint8_t EE_t::Read(uint32_t Addr, void *ptr, uint32_t Sz) {
    uint8_t DevAddr = EE_BLOCK1_DEV_ADDR;
    uint8_t ByteAddr;
    while(Sz != 0) {
        if(Addr >= EE_BLOCK_SZ) {
            ByteAddr = (uint8_t)(Addr - EE_BLOCK_SZ);
            DevAddr = EE_BLOCK2_DEV_ADDR;
        }
        else ByteAddr = (uint8_t)Addr;

        uint32_t SzToRead;
        if(Addr + Sz >= EE_BLOCK_SZ) SzToRead = EE_BLOCK_SZ - Addr;
        else SzToRead = Sz;

        uint8_t Rslt = pi2c->WriteRead(DevAddr, &ByteAddr, 1, (uint8_t*)ptr, SzToRead);
        if(Rslt != OK) return Rslt;

        Addr += SzToRead;
        Sz -= SzToRead;
    }
    return 0;
}

uint8_t EE_t::Write(uint32_t Addr, void *ptr, uint32_t Sz) {
    uint8_t DevAddr = EE_BLOCK1_DEV_ADDR;
    uint8_t ByteAddr;
    while(Sz != 0) {
        if(Addr >= EE_BLOCK_SZ) {
            ByteAddr = (uint8_t)(Addr - EE_BLOCK_SZ);
            DevAddr = EE_BLOCK2_DEV_ADDR;
        }
        else ByteAddr = (uint8_t)Addr;

        uint32_t SzToW;
        if(Sz < 8) SzToW = Sz;
        else SzToW = 8;
        if(Addr + SzToW >= EE_BLOCK_SZ) SzToW = EE_BLOCK_SZ - Addr;
        Uart.Printf("DA=%X; BA=%u; SzToW=%u\r", DevAddr, ByteAddr, SzToW);

        uint8_t Rslt = pi2c->WriteWrite(DevAddr, &ByteAddr, 1, (uint8_t*)ptr, SzToW);
        if(Rslt != OK) return Rslt;
        // Wait write cycle completion
        systime_t t = chVTGetSystemTime();
        do {
            chThdSleepMilliseconds(2);
            Rslt = pi2c->Write(DevAddr, &ByteAddr, 1);
            if(chVTTimeElapsedSinceX(t) > 999) return TIMEOUT;
        } while(Rslt != OK);

        Addr += SzToW;
        Sz -= SzToW;
    }
    Uart.Printf("Done\r");
    return OK;
}
