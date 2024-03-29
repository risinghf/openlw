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

#define DEBUG_PIN

#define PIN0                    (NC)        // Not exist pin
#define PIN1                    (NC)
#define PIN2                    (NC)
#define PIN3                    (NC)
#define PIN4                    (NC)
#define PIN5                    (PA_10)
#define PIN6                    (PA_9)
#define PIN7                    (PA_2) 
#define PIN8                    (PA_3)
#define PIN9                    (NC)         

#define PIN10                   (NC)         
#define PIN11                   (NC)          
#define PIN12                   (NC) //PB_6 USART_TX
#define PIN13                   (NC) //PB_7 USART_RX
#define PIN14                   (PB_3)
#define PIN15                   (PB_4)
#ifdef  DEBUG_PIN
#define PIN16                   (NC)
#define PIN17                   (NC)
#else
#define PIN16                   (PA_13) //SWDIO
#define PIN17                   (PA_14) //SWCLK		
#endif
#define PIN18                   (NC)
#define PIN19                   (NC)
#define PIN20                   (NC)
#define PIN21                   (NC)
#define PIN22                   (NC)
#define PIN23                   (NC)
#define PIN24                   (NC)
#define PIN25                   (NC)
#define PIN26                   (NC)
#define PIN27                   (NC)
#define PIN28                   (NC)
#define PIN29                   (NC)
#define PIN30                   (NC)
#define PIN31                   (NC)
#ifdef __cplusplus
}
#endif

#endif // __BOARD_CONFIG_H__
