/*!
 * \file      board.c
 *
 * \brief     Target board general functions implementation
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
#include <stdio.h>
#include "stm32l0xx.h"
#include "utilities.h"
#include "gpio.h"
#include "adc.h"
#include "spi.h"
#include "i2c.h"
#include "uart.h"
#include "timer.h"
#include "sysIrqHandlers.h"
#include "board-config.h"
#include "lpm-board.h"
#include "rtc-board.h"
#include "sx1276-board.h"

#include "board.h"

/*!
 * Unique Devices IDs register set ( STM32L0xxx )
 */
#define         ID1                     ( 0x1FF80050 )
#define         ID2                     ( 0x1FF80054 )
#define         ID3                     ( 0x1FF80064 )
/*!
 * LED GPIO pins objects
 */
Gpio_t Led1;
Gpio_t Led2;

/*
 * MCU objects
 */
Uart_t Uart1;

void Error_Handler(void);

/*!
 * Initializes the unused GPIO to a know status
 */
static void BoardUnusedIoInit( void );

/*!
 * System Clock Configuration
 */
static void SystemClockConfig( void );

/*!
 * System Clock Re-Configuration when waking up from STOP mode
 */
static void SystemClockReConfig( void );

/*!
 * Flag to indicate if the MCU is Initialized
 */
static bool McuInitialized = false;

/*!
 * Flag used to indicate if board is powered from the USB
 */
static bool UsbIsConnected = false;

/*!
 * UART2 FIFO buffers size
 */
#define UART1_FIFO_TX_SIZE                                1024
#define UART1_FIFO_RX_SIZE                                1024

uint8_t Uart1TxBuffer[UART1_FIFO_TX_SIZE];
uint8_t Uart1RxBuffer[UART1_FIFO_RX_SIZE];

/*!
 * Not use Pin Table
 * Will be deinit
 */
static PinNames ugpio_tab[32] = {
    PIN0,           // PIN0
    PIN1,           // PIN1
    PIN2,           // PIN2
    PIN3,           // PIN3
    PIN4,           // PIN4
    PIN5,           // PIN5
    PIN6,           // PIN6
    PIN7,           // PIN7
    PIN8,           // PIN8
    PIN9,           // PIN9
    PIN10,          // PIN10
    PIN11,          // PIN11
    PIN12,          // PIN12
    PIN13,          // PIN13
    PIN14,          // PIN14
    PIN15,          // PIN15
    PIN16,          // PIN16
    PIN17,          // PIN17
    PIN18,          // PIN18
    PIN19,          // PIN19
    PIN20,          // PIN20
    PIN21,          // PIN21
    PIN22,          // PIN22
    PIN23,          // PIN23
    PIN24,          // PIN24
    PIN25,          // PIN25
    PIN26,          // PIN26
    PIN27,          // PIN27
    PIN28,          // PIN28
    PIN29,          // PIN29
    PIN30,          // PIN30
    PIN31,          // PIN31
};

void BoardCriticalSectionBegin( uint32_t *mask )
{
    *mask = __get_PRIMASK( );
    __disable_irq( );
}

void BoardCriticalSectionEnd( uint32_t *mask )
{
    __set_PRIMASK( *mask );
}

void BoardInitPeriph( void )
{

}

void BoardInitMcu( void )
{
    if( McuInitialized == false )
    {
        HAL_Init( );

        SystemClockConfig( );

        // Disable Systick, now the rtc is used for delay function
        SysTick->CTRL  &= ~SysTick_CTRL_TICKINT_Msk;    // Systick IRQ off
        SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;            // Clear SysTick Exception pending flag

//        UsbIsConnected = true;

        FifoInit( &Uart1.FifoTx, Uart1TxBuffer, UART1_FIFO_TX_SIZE );
        FifoInit( &Uart1.FifoRx, Uart1RxBuffer, UART1_FIFO_RX_SIZE );

        RtcInit( );

        BoardUnusedIoInit( );

//        if( GetBoardPowerSource( ) == BATTERY_POWER )
//        {
//            // Disables OFF mode - Enables lowest power mode (STOP)
//            LpmSetOffMode( LPM_APPLI_ID, LPM_DISABLE );
//        }
    }
    else
    {
        SystemClockReConfig( );
    }

    // Configure your terminal for 8 Bits data (7 data bit + 1 parity bit), no parity and no flow ctrl
    UartInit( &Uart1, UART_1, UART_TX, UART_RX );
    UartConfig( &Uart1, RX_TX, 9600, UART_8_BIT, UART_1_STOP_BIT, NO_PARITY, NO_FLOW_CTRL );

    SpiInit( &SX1276.Spi, SPI_1, RADIO_MOSI, RADIO_MISO, RADIO_SCLK, NC );
	SX1276IoInit();

    if( McuInitialized == false )
    {
        McuInitialized = true;
        SX1276IoDbgInit( );
    }
}

void BoardResetMcu( void )
{
    CRITICAL_SECTION_BEGIN( );

    //Restart system
    NVIC_SystemReset( );
}

void BoardDeInitMcu( void )
{
    SpiDeInit( &SX1276.Spi );
    SX1276IoDeInit( );
    UartDeInit( &Uart1 );

    /* Deinit IO */

}

uint32_t BoardGetRandomSeed( void )
{
    return ( ( *( uint32_t* )ID1 ) ^ ( *( uint32_t* )ID2 ) ^ ( *( uint32_t* )ID3 ) );
}

void BoardGetUniqueId( uint8_t *id )
{
    id[7] = ( ( *( uint32_t* )ID1 )+ ( *( uint32_t* )ID3 ) ) >> 24;
    id[6] = ( ( *( uint32_t* )ID1 )+ ( *( uint32_t* )ID3 ) ) >> 16;
    id[5] = ( ( *( uint32_t* )ID1 )+ ( *( uint32_t* )ID3 ) ) >> 8;
    id[4] = ( ( *( uint32_t* )ID1 )+ ( *( uint32_t* )ID3 ) );
    id[3] = ( ( *( uint32_t* )ID2 ) ) >> 24;
    id[2] = ( ( *( uint32_t* )ID2 ) ) >> 16;
    id[1] = ( ( *( uint32_t* )ID2 ) ) >> 8;
    id[0] = ( ( *( uint32_t* )ID2 ) );
}

uint16_t BoardBatteryMeasureVoltage( void )
{
    return 0;
}

uint32_t BoardGetBatteryVoltage( void )
{
    return 0;
}

uint8_t BoardGetBatteryLevel( void )
{
    return 0;
}

static void BoardUnusedIoInit( void )
{
    Gpio_t gpio;
    int i = 0;

#ifdef DEBUG_PIN
    HAL_DBGMCU_EnableDBGSleepMode( );
    HAL_DBGMCU_EnableDBGStopMode( );
    HAL_DBGMCU_EnableDBGStandbyMode( );
#endif

    for(i=0; i<(sizeof(ugpio_tab)/(sizeof(PinNames))); i++)
    {
        if(ugpio_tab[i]==NC)
        {
            continue;
        }
        GpioInit( &gpio, ugpio_tab[i], PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    }
}

void SystemClockConfig( void )
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /* HSI is enabled after System reset, activate PLL with HSI as source */
    RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.MSIState = RCC_MSI_OFF;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
   
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL          = RCC_PLLMUL_6;
    RCC_OscInitStruct.PLL.PLLDIV          = RCC_PLLDIV_3;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /**Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
    {
        Error_Handler();
    }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}

void SystemClockReConfig( void )
{
    // default case, the MSI is wake-up system clock
}

void SysTick_Handler( void )
{
    HAL_IncTick( );
    HAL_SYSTICK_IRQHandler( );
}

uint8_t GetBoardPowerSource( void )
{
    if( UsbIsConnected == false )
    {
        return BATTERY_POWER;
    }
    else
    {
        return USB_POWER;
    }
}

/**
  * \brief Enters Low Power Stop Mode
  *
  * \note ARM exists the function when waking up
  */
void LpmEnterStopMode( void)
{
    CRITICAL_SECTION_BEGIN( );

    BoardDeInitMcu( );

    // Enter Stop Mode
	/*TODO*/
   // HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
    LpmExitStopMode();
    CRITICAL_SECTION_END( );
}

/*!
 * \brief Exists Low Power Stop Mode
 */
void LpmExitStopMode( void )
{
    // Disable IRQ while the MCU is not running on HSI
    CRITICAL_SECTION_BEGIN( );

    // Initilizes the peripherals
    BoardInitMcu( );

    CRITICAL_SECTION_END( );
}

/*!
 * \brief Enters Low Power Sleep Mode
 *
 * \note ARM exits the function when waking up
 */
void LpmEnterSleepMode( void)
{
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}


void BoardLowPowerHandler( void )
{
    CRITICAL_SECTION_BEGIN( );
    /*!
     * If an interrupt has occurred after __disable_irq( ), it is kept pending
     * and cortex will not enter low power anyway
     */

    LpmEnterLowPower( );

    CRITICAL_SECTION_END( );
}

bool UartMcuIsIdle( void );
bool BoardIsIdle( void )
{
    return UartMcuIsIdle();
}

#if defined(__CC_ARM) ||  defined(__ICCARM__)

// Keil and IAR compiler
int fputc( int c, FILE *stream )
{
    while( UartPutChar( &Uart1, ( uint8_t )c ) != 0 );
    return c;
}

int fgetc( FILE *stream )
{
    uint8_t c = 0;
    while( UartGetChar( &Uart1, &c ) != 0 );
    // Echo back the character
    while( UartPutChar( &Uart1, c ) != 0 );
    return ( int )c;
}

#elif defined(__GNUC__)

/*
 * Function to be used by stdout for printf etc
 */
int _write( int fd, const void *buf, size_t count )
{
    while( UartPutBuffer( &Uart1, ( uint8_t* )buf, ( uint16_t )count ) != 0 ){ };
    return count;
}

/*
 * Function to be used by stdin for scanf etc
 */
int _read( int fd, const void *buf, size_t count )
{
    size_t bytesRead = 0;
    while( UartGetBuffer( &Uart1, ( uint8_t* )buf, count, ( uint16_t* )&bytesRead ) != 0 ){ };
    // Echo back the character
    while( UartPutBuffer( &Uart1, ( uint8_t* )buf, ( uint16_t )bytesRead ) != 0 ){ };
    return bytesRead;
}

#endif

void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}


#ifdef USE_FULL_ASSERT

#include <stdio.h>

/*
 * Function Name  : assert_failed
 * Description    : Reports the name of the source file and the source line number
 *                  where the assert_param error has occurred.
 * Input          : - file: pointer to the source file name
 *                  - line: assert_param error line source number
 * Output         : None
 * Return         : None
 */
void assert_failed( uint8_t* file, uint32_t line )
{
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %lu\n", file, line) */

    printf( "Wrong parameters value: file %s on line %lu\n", ( const char* )file, line );
    /* Infinite loop */
    while( 1 )
    {
    }
}
#endif
