/*!
 * \file      board-config.h
 *
 * \brief     Board configuration
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    Daniel Jaeckle ( STACKFORCE )
 *
 * \author    Johannes Bruder ( STACKFORCE )
 */
#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*!
 * Defines the time required for the TCXO to wakeup [ms].
 */
#if defined( USE_TCXO )
#define BOARD_TCXO_WAKEUP_TIME                      5
#else
#define BOARD_TCXO_WAKEUP_TIME                      0
#endif

/*!
 * Board MCU pins definitions
 */
#define RADIO_RESET                                 NC

#define RADIO_MOSI                                  NC
#define RADIO_MISO                                  NC
#define RADIO_SCLK                                  NC

#define RADIO_NSS                                   NC
#define RADIO_BUSY                                  NC
#define RADIO_DIO_1                                 NC

#define RADIO_ANT_SWITCH_RXTX1	                    PA_4
#define RADIO_ANT_SWITCH_RXTX2                      PA_5

// Debug pins definition.
#define RADIO_DBG_PIN_TX                            NC
#define RADIO_DBG_PIN_RX                            NC


#define LED_1                                       NC
#define LED_2                                       NC

#define UART_TX                                     PB_6
#define UART_RX                                     PB_7

//#define DEBUG_PIN

#define PIN0                    (NC)        // Not exist pin
#define PIN1                    (NC)        // VCC
#define PIN2                    (NC)        // GND

#ifdef  DEBUG_PIN
#define PIN3                    (NC)     // SWCLK
#define PIN4                    (NC)     // SWDIO
#else
#define PIN3                    (PA_13)     // SWCLK
#define PIN4                    (PA_14)     // SWDIO
#endif

#define PIN5                    (PB_15)
#define PIN6                    (PA_15)
#define PIN7                    (PB_4)
#define PIN8                    (PB_3)
#define PIN9                    (NC)        // USART1_RX(PB_7)
#define PIN10                   (NC)        // USART1_TX(PB_6)
#define PIN11                   (PB_5)
#define PIN12                   (PC_1)
#define PIN13                   (PC_0)
#define PIN14                   (NC)        // GND
#define PIN15                   (NC)        // RFIO
#define PIN16                   (NC)        // GND
#define PIN17                   (NC)        // RST
#define PIN18                   (PA_3)
#define PIN19                   (PA_2)
#define PIN20                   (PB_10)
#define PIN21                   (PA_9)
#define PIN22                   (NC)        // GND
#define PIN23                   (PA_0)
#define PIN24                   (PB_13)
#define PIN25                   (PB_9)
#define PIN26                   (PB_14)
#define PIN27                   (PA_10)
#define PIN28                   (PB_0)
#define PIN29                   (NC)
#define PIN30                   (NC)
#define PIN31                   (NC)

#ifdef __cplusplus
}
#endif

#endif // __BOARD_CONFIG_H__
