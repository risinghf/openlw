/*!
 * \file      sx1262mbxdas-board.c
 *
 * \brief     Target board SX1262MBXDAS shield driver implementation
 *
 * \remark    This target board is only available with the SX126xDVK1xAS
 *            development kit.
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
#include <stdlib.h>
#include "stm32wlxx.h"
#include "utilities.h"
#include "board-config.h"
#include "board.h"
#include "delay.h"
#include "radio.h"
#include "sx126x-board.h"

typedef enum{
    ANT_CHL_TX = 0,
    ANT_CHL_RX,
}AntChl_t;
static volatile AntChl_t CurAntChl = ANT_CHL_RX;
static volatile uint8_t  AntForce = 0;
static void SX126xAntSwSelect(AntChl_t chl);
static AntChl_t SX126xCurAntChlGet(void);

#if defined( USE_RADIO_DEBUG )
/*!
 * \brief Writes new Tx debug pin state
 *
 * \param [IN] state Debug pin state
 */
static void SX126xDbgPinTxWrite( uint8_t state );

/*!
 * \brief Writes new Rx debug pin state
 *
 * \param [IN] state Debug pin state
 */
static void SX126xDbgPinRxWrite( uint8_t state );
#endif

/*!
 * \brief Holds the internal operating mode of the radio
 */
static RadioOperatingModes_t OperatingMode;

/*!
 * Antenna switch GPIO pins objects
 */
Gpio_t AntPow;
Gpio_t DeviceSel;
DioIrqHandler *dioirq_cb=NULL;

/*!
 * Debug GPIO pins objects
 */
#if defined( USE_RADIO_DEBUG )
Gpio_t DbgPinTx;
Gpio_t DbgPinRx;
#endif

void SX126xIoInit( void )
{

}

void SX126xIoIrqInit( DioIrqHandler dioIrq )
{
    SpiInit( &SX126x.Spi, SPI_SUBGHZ, RADIO_MOSI, RADIO_MISO, RADIO_SCLK, RADIO_NSS );

    CLEAR_BIT(RCC->CSR, RCC_CSR_RFRST);
    while(READ_BIT(RCC->CSR, RCC_CSR_RFRSTF) == (RCC_CSR_RFRSTF));
    SET_BIT(PWR->SUBGHZSPICR, PWR_SUBGHZSPICR_NSS);

    SET_BIT(EXTI->IMR2, EXTI_IMR2_IM44);
    SET_BIT(PWR->CR3, PWR_CR3_EWRFBUSY);

    dioirq_cb = dioIrq;
    NVIC_EnableIRQ(SUBGHZ_Radio_IRQn);
}

void SX126xIoDeInit( void )
{

}

void SX126xIoDbgInit( void )
{
#if defined( USE_RADIO_DEBUG )
    GpioInit( &DbgPinTx, RADIO_DBG_PIN_TX, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &DbgPinRx, RADIO_DBG_PIN_RX, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
#endif
}

void SX126xIoTcxoInit( void )
{
#if defined( USE_TCXO )
    CalibrationParams_t calibParam;

    SX126xSetDio3AsTcxoCtrl( TCXO_CTRL_1_7V, SX126xGetBoardTcxoWakeupTime( ) << 6 ); // convert from ms to SX126x time base
    calibParam.Value = 0x7F;
    SX126xCalibrate( calibParam );
#endif
}

uint32_t SX126xGetBoardTcxoWakeupTime( void )
{
    return BOARD_TCXO_WAKEUP_TIME;
}

void SX126xIoRfSwitchInit( void )
{
    SX126xSetDio2AsRfSwitchCtrl( true );
}

RadioOperatingModes_t SX126xGetOperatingMode( void )
{
    return OperatingMode;
}

void SX126xSetOperatingMode( RadioOperatingModes_t mode )
{
    OperatingMode = mode;
#if defined( USE_RADIO_DEBUG )
    switch( mode )
    {
        case MODE_TX:
            SX126xDbgPinTxWrite( 1 );
            SX126xDbgPinRxWrite( 0 );
            break;
        case MODE_RX:
        case MODE_RX_DC:
            SX126xDbgPinTxWrite( 0 );
            SX126xDbgPinRxWrite( 1 );
            break;
        default:
            SX126xDbgPinTxWrite( 0 );
            SX126xDbgPinRxWrite( 0 );
            break;
    }
#endif

    switch( mode )
    {
    case MODE_SLEEP:
    case MODE_STDBY_RC:
    case MODE_STDBY_XOSC:
    case MODE_FS:
        break;
    case MODE_TX:
        SX126xAntSwSelect(ANT_CHL_TX);
        break;
    case MODE_RX:
    case MODE_RX_DC:
    case MODE_CAD:
        SX126xAntSwSelect(ANT_CHL_RX);
        break;
    }
}

void SX126xReset( void )
{
    SET_BIT(RCC->CSR, RCC_CSR_RFRST);
    DelayMs( 20 );
    CLEAR_BIT(RCC->CSR, RCC_CSR_RFRST);
    DelayMs( 10 );
}

void SX126xWaitOnBusy( void )
{
    uint32_t Mask1, Mask2;
    do {
        Mask1 = (READ_BIT(PWR->SR2, PWR_SR2_RFBUSYMS) == (PWR_SR2_RFBUSYMS)) ? 1UL : 0UL;
        Mask2 = (READ_BIT(PWR->SR2, PWR_SR2_RFBUSYS) == (PWR_SR2_RFBUSYS)) ? 1UL : 0UL;
    }
    while ((Mask1 & Mask2) == 1);
}

void SX126xWakeup( void )
{
    CLEAR_BIT(PWR->SUBGHZSPICR, PWR_SUBGHZSPICR_NSS);
    DelayMs(1);
    SET_BIT(PWR->SUBGHZSPICR, PWR_SUBGHZSPICR_NSS);
}

void SX126xWriteCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
    SX126xCheckDeviceReady( );

    CLEAR_BIT(PWR->SUBGHZSPICR, PWR_SUBGHZSPICR_NSS); // GpioWrite( &SX126x.Spi.Nss, 0 );

    SpiInOut( &SX126x.Spi, ( uint8_t )command );

    for( uint16_t i = 0; i < size; i++ )
    {
        SpiInOut( &SX126x.Spi, buffer[i] );
    }

    SET_BIT(PWR->SUBGHZSPICR, PWR_SUBGHZSPICR_NSS); // GpioWrite( &SX126x.Spi.Nss, 1 );

    if( command != RADIO_SET_SLEEP )
    {
        SX126xWaitOnBusy( );
    }
}

uint8_t SX126xReadCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
    uint8_t status = 0;

    SX126xCheckDeviceReady( );

    CLEAR_BIT(PWR->SUBGHZSPICR, PWR_SUBGHZSPICR_NSS); // GpioWrite( &SX126x.Spi.Nss, 0 );

    SpiInOut( &SX126x.Spi, ( uint8_t )command );
    status = SpiInOut( &SX126x.Spi, 0x00 );
    for( uint16_t i = 0; i < size; i++ )
    {
        buffer[i] = SpiInOut( &SX126x.Spi, 0 );
    }

    SET_BIT(PWR->SUBGHZSPICR, PWR_SUBGHZSPICR_NSS); // GpioWrite( &SX126x.Spi.Nss, 1 );

    SX126xWaitOnBusy( );

    return status;
}

void SX126xWriteRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
    SX126xCheckDeviceReady( );

    CLEAR_BIT(PWR->SUBGHZSPICR, PWR_SUBGHZSPICR_NSS); // GpioWrite( &SX126x.Spi.Nss, 0 );

    SpiInOut( &SX126x.Spi, RADIO_WRITE_REGISTER );
    SpiInOut( &SX126x.Spi, ( address & 0xFF00 ) >> 8 );
    SpiInOut( &SX126x.Spi, address & 0x00FF );

    for( uint16_t i = 0; i < size; i++ )
    {
        SpiInOut( &SX126x.Spi, buffer[i] );
    }

    SET_BIT(PWR->SUBGHZSPICR, PWR_SUBGHZSPICR_NSS); // GpioWrite( &SX126x.Spi.Nss, 1 );

    SX126xWaitOnBusy( );
}

void SX126xWriteRegister( uint16_t address, uint8_t value )
{
    SX126xWriteRegisters( address, &value, 1 );
}

void SX126xReadRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
    SX126xCheckDeviceReady( );

    CLEAR_BIT(PWR->SUBGHZSPICR, PWR_SUBGHZSPICR_NSS); // GpioWrite( &SX126x.Spi.Nss, 0 );

    SpiInOut( &SX126x.Spi, RADIO_READ_REGISTER );
    SpiInOut( &SX126x.Spi, ( address & 0xFF00 ) >> 8 );
    SpiInOut( &SX126x.Spi, address & 0x00FF );
    SpiInOut( &SX126x.Spi, 0 );
    for( uint16_t i = 0; i < size; i++ )
    {
        buffer[i] = SpiInOut( &SX126x.Spi, 0 );
    }
    SET_BIT(PWR->SUBGHZSPICR, PWR_SUBGHZSPICR_NSS); // GpioWrite( &SX126x.Spi.Nss, 1 );

    SX126xWaitOnBusy( );
}

uint8_t SX126xReadRegister( uint16_t address )
{
    uint8_t data;
    SX126xReadRegisters( address, &data, 1 );
    return data;
}

void SX126xWriteBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
    SX126xCheckDeviceReady( );

    CLEAR_BIT(PWR->SUBGHZSPICR, PWR_SUBGHZSPICR_NSS); // GpioWrite( &SX126x.Spi.Nss, 0 );

    SpiInOut( &SX126x.Spi, RADIO_WRITE_BUFFER );
    SpiInOut( &SX126x.Spi, offset );
    for( uint16_t i = 0; i < size; i++ )
    {
        SpiInOut( &SX126x.Spi, buffer[i] );
    }
    SET_BIT(PWR->SUBGHZSPICR, PWR_SUBGHZSPICR_NSS); // GpioWrite( &SX126x.Spi.Nss, 1 );

    SX126xWaitOnBusy( );
}

void SX126xReadBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
    SX126xCheckDeviceReady( );

    CLEAR_BIT(PWR->SUBGHZSPICR, PWR_SUBGHZSPICR_NSS); // GpioWrite( &SX126x.Spi.Nss, 0 );

    SpiInOut( &SX126x.Spi, RADIO_READ_BUFFER );
    SpiInOut( &SX126x.Spi, offset );
    SpiInOut( &SX126x.Spi, 0 );
    for( uint16_t i = 0; i < size; i++ )
    {
        buffer[i] = SpiInOut( &SX126x.Spi, 0 );
    }
    SET_BIT(PWR->SUBGHZSPICR, PWR_SUBGHZSPICR_NSS); // GpioWrite( &SX126x.Spi.Nss, 1 );

    SX126xWaitOnBusy( );
}

void SX126xSetRfTxPower( int8_t power )
{
    SX126xSetTxParams( power, RADIO_RAMP_40_US );
}

uint8_t SX126xGetDeviceId( void )
{
#if 0
    if( GpioRead( &DeviceSel ) == 1 )
    {
        return SX1261;
    }
    else
    {
        return SX1262;
    }
#else
    return SX1262;
#endif
}


void SX126xAntSwOn( void )
{
    AntChl_t chl = SX126xCurAntChlGet();
    switch(chl)
    {
        case ANT_CHL_TX:
            GpioInit( &AntPow, RADIO_ANT_SWITCH_RXTX1, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
            GpioInit( &AntPow, RADIO_ANT_SWITCH_RXTX2, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1 );
            break;
        case ANT_CHL_RX:
            GpioInit( &AntPow, RADIO_ANT_SWITCH_RXTX1, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1 );
            GpioInit( &AntPow, RADIO_ANT_SWITCH_RXTX2, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
            break;
        default:
            break;
    }
}

void SX126xAntSwOff( void )
{
    GpioInit( &AntPow, RADIO_ANT_SWITCH_RXTX1, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &AntPow, RADIO_ANT_SWITCH_RXTX2, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
}

bool SX126xCheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}

#if defined( USE_RADIO_DEBUG )
static void SX126xDbgPinTxWrite( uint8_t state )
{
    GpioWrite( &DbgPinTx, state );
}

static void SX126xDbgPinRxWrite( uint8_t state )
{
    GpioWrite( &DbgPinRx, state );
}
#endif

static AntChl_t SX126xCurAntChlGet(void)
{
    if(AntForce != 0){
        if(CurAntChl == ANT_CHL_RX){
            return ANT_CHL_TX;
        }else{
            return ANT_CHL_RX;
        }
    }else{
        return CurAntChl;
    }
}

static void SX126xAntSwSelect(AntChl_t chl)
{
    /** update current ANT channel */
    CurAntChl = chl;

    SX126xAntSwOn();
}

void SUBGHZ_Radio_IRQHandler(void)
{
    /* Disable the Radio interrupt */
    NVIC_DisableIRQ(SUBGHZ_Radio_IRQn);

    if( dioirq_cb != NULL ) dioirq_cb(NULL);
    /* Enable the Radio interrupt */
    NVIC_EnableIRQ(SUBGHZ_Radio_IRQn);
}
