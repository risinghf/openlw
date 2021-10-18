/*!
 * \file      spi-board.c
 *
 * \brief     Target board SPI driver implementation
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
#include "stm32wlxx.h"
#include "utilities.h"
#include "board.h"
#include "gpio.h"
#include "spi-board.h"

// static SPI_HandleTypeDef SpiHandle[2];

void SpiInit( Spi_t *obj, SpiId_t spiId, PinNames mosi, PinNames miso, PinNames sclk, PinNames nss )
{
    CRITICAL_SECTION_BEGIN( );

    obj->SpiId = spiId;
    SPI_TypeDef * spiHandle = NULL;

    if( spiId == SPI_SUBGHZ )
    {
        spiHandle =  (SPI_TypeDef *)SUBGHZSPI_BASE;

        RCC->APB3ENR |= RCC_APB3ENR_SUBGHZSPIEN;

        spiHandle->CR1 |= (SPI_CR1_SSM | SPI_CR1_SSI);

        SpiFormat(obj, 8, 0, 0, 0);

        SpiFrequency(obj, 16000000);

        spiHandle->CR1 |= SPI_CR1_SPE;

    }

    CRITICAL_SECTION_END( );
}

void SpiDeInit( Spi_t *obj )
{

}

void SpiFormat( Spi_t *obj, int8_t bits, int8_t cpol, int8_t cpha, int8_t slave )
{
    uint32_t tmpreg;
    SPI_TypeDef * spiHandle = NULL;

    if(obj->SpiId == SPI_SUBGHZ) {
        spiHandle = (SPI_TypeDef *)SUBGHZSPI_BASE;
    }

    /** disable spi first */
    spiHandle->CR1 &= ~SPI_CR1_SPE;

    if ((((bits == 8) || (bits == 16)) == false) ||
            (((cpol == 0) || (cpol == 1)) == false) ||
            (((cpha == 0) || (cpha == 1)) == false)) {
        // SPI error
        while (1);
    }

    /** clear DFF, MSTR, CPOL, CPHA */
    tmpreg = spiHandle->CR1;
    tmpreg &= ~(SPI_CR1_CPHA | SPI_CR1_CPOL | SPI_CR1_MSTR );
    tmpreg |= (slave == 0x01) ? 0 : (SPI_CR1_MSTR | SPI_CR1_SSI);
    tmpreg |= (cpol == 0x01) ? SPI_CR1_CPOL : 0;
    tmpreg |= (cpha == 0x01) ? SPI_CR1_CPHA : 0;
    spiHandle->CR1 = tmpreg;

    tmpreg = spiHandle->CR2;
    tmpreg &= ~( SPI_CR2_DS );
    tmpreg |= (bits == 8) ? ( SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 ) : SPI_CR2_DS;
    spiHandle->CR2 = tmpreg;
    WRITE_REG(spiHandle->CR2, SPI_CR2_NSSP | SPI_CR2_FRXTH);

    spiHandle->I2SCFGR &= (uint16_t)~((uint16_t)SPI_I2SCFGR_I2SMOD);

    /** re-enable spi */
    spiHandle->CR1 |= SPI_CR1_SPE;
}

void SpiFrequency( Spi_t *obj, uint32_t hz )
{
    uint32_t i;
    SPI_TypeDef * spiHandle = NULL;

    if(obj->SpiId == SPI_SUBGHZ) {
        spiHandle = (SPI_TypeDef *)SUBGHZSPI_BASE;
    }

    /** disable spi first */
    spiHandle->CR1 &= ~SPI_CR1_SPE;

    for (i = 0; i < 8; i++) {
        if (hz >= (SystemCoreClock / (1 << (i + 1)))) {
            break;
        }
    }

    if (i == 8) {
        while (1);
    }

    spiHandle->CR1 = (spiHandle->CR1 & ~SPI_CR1_BR) | (i << 3);

    /** re-enable spi */
    spiHandle->CR1 |= SPI_CR1_SPE;
}

uint16_t SpiInOut( Spi_t *obj, uint16_t outData )
{
    uint8_t rxData;
    SPI_TypeDef * spiHandle = NULL;

    if(obj->SpiId == SPI_SUBGHZ) {
        spiHandle = (SPI_TypeDef *)SUBGHZSPI_BASE;
    }

    /** wait until TX buffer is empty */
    while ((spiHandle->SR & SPI_SR_TXE) != SPI_SR_TXE);

    *((volatile uint8_t *)&spiHandle->DR) = outData & 0xFF;

    /** wait until RX buffer is not empty */
    while ((spiHandle->SR & SPI_SR_RXNE) != SPI_SR_RXNE);

    rxData = *((volatile uint8_t *)&spiHandle->DR);

    return (uint16_t)rxData;
}

