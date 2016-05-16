/*
 * mcp3551.h
 *
 *  Created on: 16 ??? 2016 ?.
 *      Author: Kreyl
 */

#pragma once

#include "kl_lib.h"

// 5 MHz top SPI freq. t conv = 73ms

class MCP3551_t {
private:
    Spi_t ISpi;
    PortPin_t CS, Clk, SDO;
    PinAF_t SpiAF;
    PinIrq_t PinIrq;
public:
    void Init() const {
        // GPIO
        PinSetupOut(CS, omPushPull, pudNone);
        PinSet(CS);
        PinIrq.Init(pudPullUp, ttFalling);
        PinSetupAlterFunc(Clk, omPushPull, pudNone, SpiAF);
        PinSetupAlterFunc(SDO, omPushPull, pudPullUp, SpiAF); // setup all registers
        // SPI
        ISpi.Setup(boMSB, cpolIdleLow, cphaFirstEdge, sbFdiv16, bitn8);
        ISpi.Enable();
    }

    void StartMeasurement() const {
        // Prepare IRQ
        PinIrq.CleanIrqFlag();
        PinIrq.EnableIrq(IRQ_PRIO_LOW);
        // Select chip to start measurement. SDO will fall to low when ready.
        PinClear(CS);
    }
    void DisableIRQ() const { PinIrq.DisableIrq(); }

    // Call when ready
    uint32_t GetData() const {
        Convert::DWordBytes_t dwb;
        dwb.DWord = 0;
        ISpi.WriteRead3Bytes(&dwb.b[1]);
//        Uart.Printf("%A\r", dwb.b, 4, ' ');
        dwb.DWord = __REV(dwb.DWord);
        PinSet(CS);
        return dwb.DWord;
    }

    MCP3551_t(SPI_TypeDef *ASpi, PortPin_t ACS, PortPin_t AClk, PortPin_t ASDO, PinAF_t ASpiAF) :
        ISpi(ASpi), CS(ACS), Clk(AClk), SDO(ASDO), SpiAF(ASpiAF), PinIrq(ASDO) {}
};
