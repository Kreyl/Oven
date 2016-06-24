/*
 * kl_pid.h
 *
 *  Created on: 17 θώνÿ 2016 γ.
 *      Author: Kreyl
 */

#pragma once

#include "uart.h"

class PID_t {
private:
    float OldValue;   // Required for diff calculations
    float Integral, MaxI, MinI;
    // PID coeffs. "0" means "disabled".
    float Kp;
    float Ki;
    float Kd;
public:
    float TargetValue;
    float Calculate(float NewValue) {
        float Err = TargetValue - NewValue;
        float Rslt = 0, integ = 0, dif = 0;
        // Proportional
        if(Kp != 0) Rslt += Kp * Err;
        // Integral
        if(Ki != 0) {
            Integral += Err;
            if(Integral > MaxI) Integral = MaxI;
            else if(Integral < MinI) Integral = MinI;
            integ = Ki * Integral;
            Rslt += integ;
        }
        // Differential
        if(Kd != 0) {
            dif = Kd * (OldValue - NewValue);
            Rslt += dif;
            OldValue = NewValue;   // Save current value
        }
        // Output limitation
        if(Rslt > 100) Rslt = 100;
        else if(Rslt < -100) Rslt = -100;
//        Uart.Printf("t=%.1f; Err=%.1f; Rslt=%.1f\r\n", NewValue, Err, Rslt);
//        Uart.Printf("%.1f;%.1f;%.1f", NewValue, Err, Rslt);
//        Uart.Printf("%.1f, %.1f", Err, Rslt);
//        if(Kd != 0) Uart.Printf(" ,%.1f", dif);
//        if(Ki != 0) Uart.Printf(" ,%.1f", integ);
//        Uart.Printf("\r\n");
        return Rslt;
    }

    PID_t(float AKp, float AKi, float AMaxI, float AMinI, float AKd) :
        OldValue(0),
        Integral(0), MaxI(AMaxI), MinI(AMinI),
        Kp(AKp), Ki(AKi), Kd(AKd),
        TargetValue(0) {}
};
