/*!
 * \file      utilities.h
 *
 * \brief     Helper functions implementation
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
#include <stdio.h>
#include "utilities.h"

/*!
 * Redefinition of rand() and srand() standard C functions.
 * These functions are redefined in order to get the same behavior across
 * different compiler toolchains implementations.
 */
// Standard random functions redefinition start
#define RAND_LOCAL_MAX 2147483647L

static uint32_t next = 1;

int32_t rand1( void )
{
    return ( ( next = next * 1103515245L + 12345L ) % RAND_LOCAL_MAX );
}

void srand1( uint32_t seed )
{
    next = seed;
}
// Standard random functions redefinition end

int32_t randr( int32_t min, int32_t max )
{
    return ( int32_t )rand1( ) % ( max - min + 1 ) + min;
}

void memcpy1( uint8_t *dst, const uint8_t *src, uint16_t size )
{
    while( size-- )
    {
        *dst++ = *src++;
    }
}

void memcpyr( uint8_t *dst, const uint8_t *src, uint16_t size )
{
    dst = dst + ( size - 1 );
    while( size-- )
    {
        *dst-- = *src++;
    }
}

void memset1( uint8_t *dst, uint8_t value, uint16_t size )
{
    while( size-- )
    {
        *dst++ = value;
    }
}

int8_t Nibble2HexChar( uint8_t a )
{
    if( a < 10 )
    {
        return '0' + a;
    }
    else if( a < 16 )
    {
        return 'A' + ( a - 10 );
    }
    else
    {
        return '?';
    }
}

uint32_t Crc32( uint8_t *buffer, uint16_t length )
{
    // The CRC calculation follows CCITT - 0x04C11DB7
    const uint32_t reversedPolynom = 0xEDB88320;

    // CRC initial value
    uint32_t crc = 0xFFFFFFFF;

    if( buffer == NULL )
    {
        return 0;
    }

    for( uint16_t i = 0; i < length; ++i )
    {
        crc ^= ( uint32_t )buffer[i];
        for( uint16_t i = 0; i < 8; i++ )
        {
            crc = ( crc >> 1 ) ^ ( reversedPolynom & ~( ( crc & 0x01 ) - 1 ) );
        }
    }

    return ~crc;
}

uint32_t Crc32Init( void )
{
    return 0xFFFFFFFF;
}

uint32_t Crc32Update( uint32_t crcInit, uint8_t *buffer, uint16_t length )
{
    // The CRC calculation follows CCITT - 0x04C11DB7
    const uint32_t reversedPolynom = 0xEDB88320;

    // CRC initial value
    uint32_t crc = crcInit;

    if( buffer == NULL )
    {
        return 0;
    }

    for( uint16_t i = 0; i < length; ++i )
    {
        crc ^= ( uint32_t )buffer[i];
        for( uint16_t i = 0; i < 8; i++ )
        {
            crc = ( crc >> 1 ) ^ ( reversedPolynom & ~( ( crc & 0x01 ) - 1 ) );
        }
    }
    return crc;
}

uint32_t Crc32Finalize( uint32_t crc )
{
    return ~crc;
}


int vfprintf1(FILE *fp, const char * fmt, va_list arg)
{
    int i, d, ret, len, j, k;
    char c, *s;
    uint8_t *hbuf;
    double lf;
    char strfmt[10];

    i = 0;
    while(*fmt) {
        if(*fmt == '%'){
            strfmt[0] = '%';
            j=1;
            while( ( fmt[j]>='0' && fmt[j]<='9' ) ||
                  ( fmt[j]== '-' ) || ( fmt[j]== '+' ) || ( fmt[j]== '.' ) ){
                strfmt[j] = fmt[j];
                j++;
            }
            strfmt[j] = fmt[j];
            fmt += j;
            j++;
            strfmt[j] = '\0';

            switch(*fmt){
            case '%':
                ret = fprintf(fp, strfmt);
                i+=ret;
                break;
            case 'd':
                d = va_arg(arg, int);
                ret = fprintf(fp, strfmt, d);
                i+=ret;
                break;
            case 'u':
                d = va_arg(arg, int);
                ret = fprintf(fp, strfmt, (uint32_t)d);
                i+=ret;
                break;
            case 'x':
            case 'X':
                d = va_arg(arg, int);
                ret = fprintf(fp, strfmt, d);
                i+=ret;
                break;
            case 'S':
                hbuf = va_arg(arg, uint8_t *);
                len = va_arg(arg, int);
                for(d=0; d<len; d++){
                    fprintf(fp, "%c", hbuf[d]);
                }
                break;
            case 'h':
            case 'H':
                hbuf = va_arg(arg, uint8_t *);
                len = va_arg(arg, int);
                if (len == 0) {
                    fprintf(fp, "NULL");
                } else {
                    for(d=0; d<len; d++){
                        if(*fmt == 'h'){
                            fprintf(fp, "%02X", hbuf[d]);
                        }else{
                            fprintf(fp, "%02X ", hbuf[d]);
                        }
                    }
                }
                break;
            case 'M':
                hbuf = va_arg(arg, uint8_t *);
                len = va_arg(arg, int);
                for (k = 0; k < len; k++) {
                    for (j = 0; j < 8; j++) {
                        if ((hbuf[k] & (1 << j)) != 0) {
                            fprintf(fp, "%d ", 8 * k + j);
                        }
                    }
                }
                break;
            case 'B':
                hbuf = va_arg(arg, uint8_t *);
                len = va_arg(arg, int);
                for(d=0; d<len; d++){
                    ret = fprintf(fp, "%c", hbuf[d]);
                    i+=ret;
                }
                break;
            case 's':
                s = va_arg(arg, char *);
                ret = fprintf(fp, strfmt, s);
                i+=ret;
                break;
            case 'c':
                c = (char)va_arg(arg, int);
                ret = fprintf(fp, strfmt, c);
                i+=ret;
                break;
            case 'f':
                lf = va_arg(arg, double);
                ret = fprintf(fp, strfmt, lf);
                i+=ret;
                break;
            }
            fmt++;
        } else {
            fputc(*fmt++, fp);
            i++;
        }
    }

    return i;
}

int fprintf1(FILE *f, const char *fmt, ...)
{
    int i = 0;
    va_list ap;

    va_start(ap, fmt);

    i = vfprintf1(f, fmt, ap);

    va_end(ap);

    return i;
}
