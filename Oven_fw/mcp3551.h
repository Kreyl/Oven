/*
 * mcp3551.h
 *
 *  Created on: 16 ??? 2016 ?.
 *      Author: Kreyl
 */

#pragma once

#include "kl_lib.h"

// 5 MHz top SPI freq. t conv = 73ms

#define MCP3551_TIMEOUT_MS      99

//union McpRslt_t {
//    uint32_t W32;
//    struct {
//
//    };
//};

class MCP3551_t {
private:
    Spi_t ISpi;
    PortPin_t CS, Clk, SDO;
public:
    void Init() const {
        // GPIO
        PinSetupOut(CS, omPushPull, pudNone);
        PinSet(CS);
        PinSetupAlterFunc(Clk, omPushPull, pudNone, AF5);
        PinSetupAlterFunc(SDO, omPushPull, pudPullUp, AF5); // setup all registers
        // SPI
        ISpi.Setup(boMSB, cpolIdleLow, cphaFirstEdge, sbFdiv64, bitn8);
        ISpi.Enable();
    }
    uint32_t GetData() const {
        PinClear(CS);
        // Wait data ready
        systime_t TimeStart = chVTGetSystemTimeX();
        while(PinIsSet(SDO)) {
            chThdSleepMilliseconds(7);
            if(chVTTimeElapsedSinceX(TimeStart) > MS2ST(MCP3551_TIMEOUT_MS)) {
                Uart.Printf("MCP Timeout\r");
                PinSet(CS);
                return 0;
            }
        }
        // Data is ready
        uint32_t Rslt;
        Convert::DWordBytes_t dwb;
        dwb.DWord = 0;
        ISpi.WriteRead3Bytes(&dwb.b[1]);
//        Uart.Printf("%A\r", dwb.b, 4, ' ');
        Rslt = __REV(dwb.DWord);
        PinSet(CS);
        return Rslt;
    }

    MCP3551_t(SPI_TypeDef *ASpi, PortPin_t ACS, PortPin_t AClk, PortPin_t ASDO) :
        ISpi(ASpi), CS(ACS), Clk(AClk), SDO(ASDO) {}
};
