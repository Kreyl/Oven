/*
 * adc.h
 *
 *  Created on: 11.07.2011
 *      Author: Kreyl
 */

#ifndef ADC_H_
#define ADC_H_

#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"

#define ADC_AVERAGE_COUNT   16

class ADC_t {
private:

public:
    uint16_t ADCValues[ADC_AVERAGE_COUNT];
    uint16_t AverageValues(void);
    void Init(void);
};

extern ADC_t Adc;


#endif /* ADC_H_ */
