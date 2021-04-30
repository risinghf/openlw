/*
    _/_/_/    _/_/_/    _/_/_/  _/_/_/  _/      _/    _/_/_/  _/    _/  _/_/_/_/
   _/    _/    _/    _/          _/    _/_/    _/  _/        _/    _/  _/
  _/_/_/      _/      _/_/      _/    _/  _/  _/  _/  _/_/  _/_/_/_/  _/_/_/
 _/    _/    _/          _/    _/    _/    _/_/  _/    _/  _/    _/  _/
_/    _/  _/_/_/  _/_/_/    _/_/_/  _/      _/    _/_/_/  _/    _/  _/
    (C)2021 RisingHF, all rights reserved.
*/

#include "print.h"


void print_hex_buf( const uint8_t *buf, uint32_t size )
{
    uint32_t i = 0;

    for( i = 0; i < size; i++ ) {
        printf( "%02X ", buf[i] );
    }
}

void print_new_line(void)
{
    printf("\r\n");
}


