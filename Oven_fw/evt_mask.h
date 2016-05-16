/*
 * evt_mask.h
 *
 *  Created on: Apr 12, 2013
 *      Author: g.kruglov
 */

#pragma once
// Event masks
#define EVTMSK_UART_NEW_CMD     EVENT_MASK(1)

#define EVTMSK_MEASURE_TIME     EVENT_MASK(2)
#define EVTMSK_ADC_HEATER_DONE  EVENT_MASK(3)
#define EVTMSK_ADC_PCB_DONE     EVENT_MASK(4)

#define EVTMSK_USB_DATA_OUT     EVENT_MASK(20)
#define EVTMSK_USB_READY        EVENT_MASK(21)
#define EVTMSK_USB_SUSPEND      EVENT_MASK(22)
