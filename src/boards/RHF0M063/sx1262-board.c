/*
  ______                              _
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: SX126x driver specific target board functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#include <stdlib.h>
#include "stm32l0xx.h"
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

/*!
 * Debug GPIO pins objects
 */
#if defined( USE_RADIO_DEBUG )
Gpio_t DbgPinTx;
Gpio_t DbgPinRx;
#endif

void SX126xIoInit( void )
{
    GpioInit( &SX126x.BUSY, RADIO_BUSY, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &SX126x.DIO1, RADIO_DIO_1, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &SX126x.Spi.Nss, RADIO_NSS, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1 );
}

void SX126xIoIrqInit( DioIrqHandler dioIrq )
{
    GpioSetInterrupt( &SX126x.DIO1, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, dioIrq );
}

void SX126xIoDeInit( void )
{
    GpioInit( &SX126x.BUSY, RADIO_BUSY, PIN_INPUT, PIN_PUSH_PULL, PIN_PULL_UP, 0 );
    GpioInit( &SX126x.DIO1, RADIO_DIO_1, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
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

RadioOperatingModes_t SX126xGetOperatingMode( void )
{
    return OperatingMode;
}

void SX126xIoRfSwitchInit( void )
{
    SX126xSetDio2AsRfSwitchCtrl( true );
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
    DelayMs( 10 );
    GpioInit( &SX126x.Reset, RADIO_RESET, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    DelayMs( 20 );
    GpioInit( &SX126x.Reset, RADIO_RESET, PIN_INPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1 );
    DelayMs( 10 );
}

void SX126xWaitOnBusy( void )
{
//   extern volatile bool radio_reinit_flg;
//    int timeout = 0;
    while( GpioRead( &SX126x.BUSY ) == 1 ){
        // CyDelayUs(1);
//        timeout++;
//        if(timeout > 20000*1000){
//           radio_reinit_flg = true;
//           break;
//        }
    }
}

void SX126xWakeup( void )
{
    CRITICAL_SECTION_BEGIN( );

    GpioWrite( &SX126x.Spi.Nss, 0 );

    SpiInOut( &SX126x.Spi, RADIO_GET_STATUS );
    SpiInOut( &SX126x.Spi, 0x00 );

    GpioWrite( &SX126x.Spi.Nss, 1 );

    // Wait for chip to be ready.
    SX126xWaitOnBusy( );

    CRITICAL_SECTION_END( );
}

void SX126xWriteCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
    SX126xCheckDeviceReady( );

    CRITICAL_SECTION_BEGIN( );

    GpioWrite( &SX126x.Spi.Nss, 0 );

    SpiInOut( &SX126x.Spi, ( uint8_t )command );

    for( uint16_t i = 0; i < size; i++ )
    {
        SpiInOut( &SX126x.Spi, buffer[i] );
    }

    GpioWrite( &SX126x.Spi.Nss, 1 );

    if( command != RADIO_SET_SLEEP )
    {
        SX126xWaitOnBusy( );
    }

    CRITICAL_SECTION_END( );
}

uint8_t SX126xReadCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
    uint8_t status = 0;
    
    SX126xCheckDeviceReady( );

    CRITICAL_SECTION_BEGIN( );

    GpioWrite( &SX126x.Spi.Nss, 0 );

    SpiInOut( &SX126x.Spi, ( uint8_t )command );
    status = SpiInOut( &SX126x.Spi, 0x00 );
    for( uint16_t i = 0; i < size; i++ )
    {
        buffer[i] = SpiInOut( &SX126x.Spi, 0 );
    }

    GpioWrite( &SX126x.Spi.Nss, 1 );

    SX126xWaitOnBusy( );

    CRITICAL_SECTION_END( );
    
    return status;
}

void SX126xWriteRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
    SX126xCheckDeviceReady( );

    CRITICAL_SECTION_BEGIN( );

    GpioWrite( &SX126x.Spi.Nss, 0 );

    SpiInOut( &SX126x.Spi, RADIO_WRITE_REGISTER );
    SpiInOut( &SX126x.Spi, ( address & 0xFF00 ) >> 8 );
    SpiInOut( &SX126x.Spi, address & 0x00FF );

    for( uint16_t i = 0; i < size; i++ )
    {
        SpiInOut( &SX126x.Spi, buffer[i] );
    }

    GpioWrite( &SX126x.Spi.Nss, 1 );

    SX126xWaitOnBusy( );

    CRITICAL_SECTION_END( );
}

void SX126xWriteRegister( uint16_t address, uint8_t value )
{
    SX126xWriteRegisters( address, &value, 1 );
}

void SX126xReadRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
    SX126xCheckDeviceReady( );

    CRITICAL_SECTION_BEGIN( );

    GpioWrite( &SX126x.Spi.Nss, 0 );

    SpiInOut( &SX126x.Spi, RADIO_READ_REGISTER );
    SpiInOut( &SX126x.Spi, ( address & 0xFF00 ) >> 8 );
    SpiInOut( &SX126x.Spi, address & 0x00FF );
    SpiInOut( &SX126x.Spi, 0 );
    for( uint16_t i = 0; i < size; i++ )
    {
        buffer[i] = SpiInOut( &SX126x.Spi, 0 );
    }
    GpioWrite( &SX126x.Spi.Nss, 1 );

    SX126xWaitOnBusy( );

    CRITICAL_SECTION_END( );
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

    CRITICAL_SECTION_BEGIN( );

    GpioWrite( &SX126x.Spi.Nss, 0 );

    SpiInOut( &SX126x.Spi, RADIO_WRITE_BUFFER );
    SpiInOut( &SX126x.Spi, offset );
    for( uint16_t i = 0; i < size; i++ )
    {
        SpiInOut( &SX126x.Spi, buffer[i] );
    }
    GpioWrite( &SX126x.Spi.Nss, 1 );

    SX126xWaitOnBusy( );

    CRITICAL_SECTION_END( );
}

void SX126xReadBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
    SX126xCheckDeviceReady( );

    CRITICAL_SECTION_BEGIN( );

    GpioWrite( &SX126x.Spi.Nss, 0 );

    SpiInOut( &SX126x.Spi, RADIO_READ_BUFFER );
    SpiInOut( &SX126x.Spi, offset );
    SpiInOut( &SX126x.Spi, 0 );
    for( uint16_t i = 0; i < size; i++ )
    {
        buffer[i] = SpiInOut( &SX126x.Spi, 0 );
    }
    GpioWrite( &SX126x.Spi.Nss, 1 );

    SX126xWaitOnBusy( );

    CRITICAL_SECTION_END( );
}

void SX126xSetRfTxPower( int8_t power )
{
#if 0
    SX126xSetTxParams( power, RADIO_RAMP_40_US );
#else
    SX126xSetTxParams( power, RADIO_RAMP_200_US );
#endif
}

uint8_t SX126xGetPaSelect( uint32_t channel )
{
    return SX1262;
    /*
    if( GpioRead( &DeviceSel ) == 1 )
    {
        return SX1261;
    }
    else
    {
        return SX1262;
    }
    */
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

// (RADIO_ANT_SWITCH_RXTX1 -> Hight && RADIO_ANT_SWITCH_RXTX2 -> Low) =>> RF receiver input
// (RADIO_ANT_SWITCH_RXTX1 -> Low && RADIO_ANT_SWITCH_RXTX2 -> Hight) =>> RF transmitter output
static AntChl_t SX126xCurAntChlGet(void);
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
    GpioInit( &AntPow, RADIO_ANT_SWITCH_RXTX1, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &AntPow, RADIO_ANT_SWITCH_RXTX2, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
}

bool SX126xCheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}


/******************************EXT**********************************************/
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

void SX126xAntForceSet(uint8_t val)
{
    AntForce = val;
}

void SX126xAntSwSelect(AntChl_t chl)
{
    /** update current ANT channel */
    CurAntChl = chl;

#if 0
    switch(chl)
    {
        case ANT_CHL_TX:
            GpioInit( &AntPow, ANT_SWITCH_POWER, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
            break;

        case ANT_CHL_RX:
            GpioInit( &AntPow, ANT_SWITCH_POWER, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1 );
            break;

        default:
            break;
    }
#else
    SX126xAntSwOn();
#endif
}

int8_t SX126xSetRfTxPower_Ext(int8_t power, uint32_t ch, bool write)
{
    RadioRampTimes_t rampTime = RADIO_RAMP_40_US;

    ch = ch;

    uint8_t buf[2];

    if( SX126xGetPaSelect( 0 ) == SX1261 )
    {
        if( power == 15 )
        {
            if(write)
            {
                SX126xSetPaConfig( 0x06, 0x00, 0x01, 0x01 );
            }
        }
        else
        {
            if(write)
            {
                SX126xSetPaConfig( 0x04, 0x00, 0x01, 0x01 );
            }
        }
        if( power >= 14 )
        {
            power = 14;
        }
        else if( power < -3 )
        {
            power = -3;
        }
        if(write)
        {
            SX126xWriteRegister( REG_OCP, 0x18 ); // current max is 80 mA for the whole device
        }
    }
    else // sx1262
    {
        if(write)
        {
            SX126xSetPaConfig( 0x04, 0x07, 0x00, 0x01 );
        }
        if( power > 22 )
        {
            power = 22;
        }
        else if( power < -3 )
        {
            power = -3;
        }
        if(write)
        {
            SX126xWriteRegister( REG_OCP, 0x38 ); // current max 160mA for the whole device
        }
    }

    if(write)
    {
        buf[0] = power;
        buf[1] = ( uint8_t )rampTime;
        SX126xWriteCommand( RADIO_SET_TXPARAMS, buf, 2 );
    }

    return power;
}

bool SX126XRxOnGoing(void)
{
    uint16_t irqRegs = SX126xGetIrqStatus( );
    uint16_t irqMsk = IRQ_PREAMBLE_DETECTED|IRQ_HEADER_VALID;

    // SX126xClearIrqStatus( irqMsk ); // the flags will be clear when rx_done or rx_timeout interrupt, as well as before SetRx function call

    if( (irqRegs & irqMsk) == irqMsk ) {
        return true;
    } else {
        return false;
    }
}
