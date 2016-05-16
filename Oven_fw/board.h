/*
 * board.h
 *
 *  Created on: 12 сент. 2015 г.
 *      Author: Kreyl
 */

#pragma once

#include <inttypes.h>

// ==== General ====
#define BOARD_NAME          "Oven"
// MCU type as defined in the ST header.
#define STM32L476xx

// Freq of external crystal if any. Leave it here even if not used.
#define CRYSTAL_FREQ_HZ 12000000

// OS timer settings
#define STM32_ST_IRQ_PRIORITY   2
#define STM32_ST_USE_TIMER      5
#define SYS_TIM_CLK             (Clk.APB1FreqHz)    // Timer 5 is clocked by APB1

//  Periphery
#define I2C1_ENABLED            FALSE
#define I2C2_ENABLED            FALSE
#define I2C3_ENABLED            TRUE

#define ADC_REQUIRED            FALSE
#define STM32_DMA_REQUIRED      TRUE    // Leave this macro name for OS

// LCD
#define LCD_W           320 // }
#define LCD_H           240 // } Pixels count

#if 1 // ========================== GPIO =======================================
// UART
#define UART_GPIO       GPIOA
#define UART_TX_PIN     2
#define UART_RX_PIN     3
#define UART_AF         AF7 // for all USARTs

// LEDs GPIO and timer
#define LED_RED         { GPIOC, 6, TIM3, 1 }
#define LED_GREEN       { GPIOC, 7, TIM3, 2 }
#define LED_BLUE        { GPIOC, 8, TIM3, 3 }
#define LED_WHITE       { GPIOC, 9, TIM3, 4 }

// LCD
#define LCD_DATA_GPIO   GPIOB
#define LCD_RESET       { GPIOD, 2 }
#define LCD_CSX         { GPIOC, 3 }
#define LCD_DC          { GPIOA, 15 }
#define LCD_WR          { GPIOC, 5 }
#define LCD_RD          { GPIOC, 4 }
#define LCD_TE          { GPIOA, 0 }

// Touch
#define TOUCH_INT       { GPIOC, 2 }

// I2C
#define I2C1_GPIO       GPIOB
#define I2C1_SCL        6
#define I2C1_SDA        7
#define I2C2_GPIO       GPIOB
#define I2C2_SCL        10
#define I2C2_SDA        11
#define I2C3_GPIO       GPIOC
#define I2C3_SCL        0
#define I2C3_SDA        1

// ADCs
#define ADC_H_CS        { GPIOA, 7 }
#define ADC_H_CLK       { GPIOA, 5 }
#define ADC_H_SDO       { GPIOA, 6 }
#define ADC_H_SPI_AF    AF5
#define ADC_P_CS        { GPIOC, 12 }
#define ADC_P_CLK       { GPIOC, 10 }
#define ADC_P_SDO       { GPIOC, 11 }
#define ADC_P_SPI_AF    AF6

#endif // GPIO

#if 1 // ========================= Timer =======================================
//#define TMR_DAC_CHUNK               TIM6
//#define TMR_DAC_SMPL                TIM7
//#define TMR_DAC_CHUNK_IRQ           TIM6_IRQn
//#define TMR_DAC_CHUNK_IRQ_HANDLER   VectorEC
#endif // Timer

#if 1 // =========================== SPI =======================================
#define ADC_H_SPI       SPI1
#define ADC_P_SPI       SPI3
#endif

#if 1 // =========================== I2C =======================================
#define I2C_TOUCH       i2c3
#endif

#if 1 // ========================== USART ======================================
#define UART            USART2
#define UART_TX_REG     UART->TDR
#define UART_RX_REG     UART->RDR
#endif

#if ADC_REQUIRED // ======================= Inner ADC ==========================
// Clock divider: clock is generated from the APB2
#define ADC_CLK_DIVIDER		adcDiv2

// ADC channels
#define BAT_CHNL 	        1

#define ADC_VREFINT_CHNL    17  // All 4xx and F072 devices. Do not change.
#define ADC_CHANNELS        { BAT_CHNL, ADC_VREFINT_CHNL }
#define ADC_CHANNEL_CNT     2   // Do not use countof(AdcChannels) as preprocessor does not know what is countof => cannot check
#define ADC_SAMPLE_TIME     ast55d5Cycles
#define ADC_SAMPLE_CNT      8   // How many times to measure every channel

#define ADC_MAX_SEQ_LEN     16  // 1...16; Const, see ref man
#define ADC_SEQ_LEN         (ADC_SAMPLE_CNT * ADC_CHANNEL_CNT)
#if (ADC_SEQ_LEN > ADC_MAX_SEQ_LEN) || (ADC_SEQ_LEN == 0)
#error "Wrong ADC channel count and sample count"
#endif
#endif

#if 1 // =========================== DMA =======================================
// ==== Uart ====
// Remap is made automatically if required
#define UART_DMA_TX     STM32_DMA1_STREAM7
#define UART_DMA_RX     STM32_DMA1_STREAM6
#define UART_DMA_CHNL   2

// DAC
//#define DAC_DMA         STM32_DMA1_STREAM2

// ==== I2C ====
#define I2C1_DMA_TX     STM32_DMA2_STREAM7
#define I2C1_DMA_RX     STM32_DMA1_STREAM6
#define I2C1_DMA_CHNL   5
#define I2C2_DMA_TX     STM32_DMA1_STREAM4
#define I2C2_DMA_RX     STM32_DMA1_STREAM5
#define I2C2_DMA_CHNL   3
#define I2C3_DMA_TX     STM32_DMA1_STREAM2
#define I2C3_DMA_RX     STM32_DMA1_STREAM3
#define I2C3_DMA_CHNL   3

#define STM32_I2C_I2C3_RX_DMA_STREAM   STM32_DMA_STREAM_ID(1, 3)
#define STM32_I2C_I2C3_TX_DMA_STREAM   STM32_DMA_STREAM_ID(1, 2)


#if ADC_REQUIRED
/* DMA request mapped on this DMA channel only if the corresponding remapping bit is cleared in the SYSCFG_CFGR1
 * register. For more details, please refer to Section10.1.1: SYSCFG configuration register 1 (SYSCFG_CFGR1) on
 * page173 */
#define ADC_DMA         STM32_DMA1_STREAM1
#define ADC_DMA_MODE    STM32_DMA_CR_CHSEL(0) |   /* DMA2 Stream4 Channel0 */ \
                        DMA_PRIORITY_LOW | \
                        STM32_DMA_CR_MSIZE_HWORD | \
                        STM32_DMA_CR_PSIZE_HWORD | \
                        STM32_DMA_CR_MINC |       /* Memory pointer increase */ \
                        STM32_DMA_CR_DIR_P2M |    /* Direction is peripheral to memory */ \
                        STM32_DMA_CR_TCIE         /* Enable Transmission Complete IRQ */
#endif // ADC

#endif // DMA
