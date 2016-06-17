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
    float OldErr;   // Required for diff calculations
    float TargetValue;
    // PID coeffs. "0" means "disabled".
    float Kp;
    float Ki;
    float Kd;
public:
    void SetTarget(float NewTarget) { TargetValue = NewTarget; }
    float Calculate(float NewValue) {
        float Err = TargetValue - NewValue;
//        Uart.Printf()
        // Proportional
//        if(
        OldErr = Err;   // Save current err value
        return 0;
    }

    PID_t(float AKp, float AKi, float AKd) :
        OldErr(0), TargetValue(0),
        Kp(AKp), Ki(AKi), Kd(AKd) {}
};
