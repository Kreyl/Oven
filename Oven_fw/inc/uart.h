/*
 * File:   uartsoftware.h
 * Author: Laurelindo
 *
 * Created on 21 Октябрь 2009 г., 0:20
 */

#ifndef _UART_H
#define	_UART_H

#include <inttypes.h>
#include <stdbool.h>
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#ifdef __cplusplus
 extern "C" {
#endif

// ============================ Definitions ====================================
#define UART_HARDWARE   // Software or hardware uart

#ifdef UART_HARDWARE
// USART1 definitions
#define USART1_CLK      RCC_APB2Periph_USART1
#define USART1_TX_PIN   GPIO_Pin_9
#define USART1_RX_PIN   GPIO_Pin_10
#define USART1_GPIO     GPIOA
#define USART1_GPIO_CLK RCC_APB2Periph_GPIOA

//#define TxPin
//#define RxPin
//#define UARTPort

#else
#define UARTTxOneBit    1	// for BaudRate = 490000, sysClock = 8 MHz
#define SET_TX_PIN( )	(UARTPort |=  (1<<TxPin))
#define CLEAR_TX_PIN( ) (UARTPort &= ~(1<<TxPin))
#define GET_RX_PIN( )	(UARTPin & (1<<RxPin))
#endif

// ============================ Prototypes =====================================
void UART_Init(void);
void UART_Print (uint8_t AByte);
char UintToHexChar (uint8_t b);
void UART_PrintAsHex (uint8_t AByte);
void UART_PrintUint (uint32_t ANumber);
void UART_PrintInt (int32_t ANumber);
void UART_PrintString (const char *S);
void UART_PrintArrAsString (uint8_t *Arr, uint8_t ALength);
void UART_PrintArrAsHex(uint8_t *Arr, uint8_t ALength);
void UART_NewLine (void);

void UART_StrUint (const char *S, uint32_t ANumber);
void UART_StrInt  (const char *S,  int32_t ANumber);
void UART_StrHex8 (const char *S, uint8_t  ANumber);
void UART_StrHex16(const char *S, uint16_t ANumber);


#ifdef __cplusplus
}
#endif


#endif

