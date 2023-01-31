/*!
 * \file      sysIrqHandlers.c
 *
 * \brief     Default IRQ handlers
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
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#include <stdint.h>
#include "stm32l0xx.h"

/*!
 * \brief  This function handles NMI exception.
 * \param  None
 * \retval None
 */
void NMI_Handler( void )
{
    while(1);
}

/*!
 * \brief  This function handles Hard Fault exception.
 * \param  None
 * \retval None
 */
void HardFault_Handler( void )
{
    // NVIC_SystemReset();
    while(1);
}

