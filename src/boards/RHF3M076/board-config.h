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
#define RADIO_RESET                                 PB_11

#define RADIO_MOSI                                  PA_7
#define RADIO_MISO                                  PA_6
#define RADIO_SCLK                                  PA_5

#define RADIO_NSS                                   PA_4

#define RADIO_DIO_0                                 PB_10
#define RADIO_DIO_1                                 PB_2
#define RADIO_DIO_2                                 PB_0
#define RADIO_DIO_3                                 PB_1
#define RADIO_DIO_4                                 NC
#define RADIO_DIO_5                                 NC

#define RADIO_TCXO_POWER                            NC

#define RADIO_ANT_SWITCH_RXTX1	                    PA_1
#define RADIO_ANT_SWITCH_RXTX2                      PA_2
#define RADIO_BAND_SWHF                             PA_0
#define RADIO_BAND_SWLF                             PC_13
  
  

#define LED_1                                       PB_4
#define LED_2                                       PB_3
#define LED_3                                       PA_3
#define LED_4                                       PB_5
#define LED_5                                     	PB_7
#define LED_6                                       PA_8


#define UART_TX                                     PB_6
#define UART_RX                                     PB_7

//#define DEBUG_PIN

#define PIN0                    (NC)        // Not exist pin
#define PIN1                    (NC)        // VCC
#define PIN2                    (NC)        // GND
#define PIN3                    (PA_8)     	// 
#define PIN4                    (PA_9)     	// FW_UART_TX
#define PIN5                    (PA_10)		//FW_UART_RX
#define PIN6                    (PB_12)		//NSS
#define PIN7                    (PB_13)		//SCK
#define PIN8                    (PB_14)      	//MISO
#define PIN9                    (PB_15)       //MOSI
#define PIN10                   (PA_11)       //USB_DM
#define PIN11                   (PA_12)      	//USB_DP
   
#ifdef  DEBUG_PIN
	#define PIN12				(NC)      	//SWDIO
	#define PIN13				(NC)      	//SWCLK
#else
	#define PIN12				(PA_13)      //SWDIO
	#define PIN13				(PA_14)      //SWCLK
#endif
   
#define PIN14                   (PA_15)       
#define PIN15                   (PB_3)       
#define PIN16                   (PB_4)     
#define PIN17                   (NC)        // RST
#define PIN18                   (NC)
#define PIN19                   (NC)
#define PIN20                   (PA_3)
  
#define PIN21                   (PB_5)
#define PIN22                   (PB_6)        //USART_TX
#define PIN23                   (PB_7)        //USART_RX
#define PIN24                   (PB_8)        //SCL
#define PIN25                   (PB_9)        //SDA
#define PIN26                   (PC_13)      //SW_LF
#define PIN27                   (NC)         //NRST
#define PIN28                   (PA_0)       //SW_HF
#define PIN29                   (NC)
#define PIN30                   (NC)
#define PIN31                   (NC)

#ifdef __cplusplus
}
#endif

#endif // __BOARD_CONFIG_H__
