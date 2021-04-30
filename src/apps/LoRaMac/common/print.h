/*
    _/_/_/    _/_/_/    _/_/_/  _/_/_/  _/      _/    _/_/_/  _/    _/  _/_/_/_/
   _/    _/    _/    _/          _/    _/_/    _/  _/        _/    _/  _/
  _/_/_/      _/      _/_/      _/    _/  _/  _/  _/  _/_/  _/_/_/_/  _/_/_/
 _/    _/    _/          _/    _/    _/    _/_/  _/    _/  _/    _/  _/
_/    _/  _/_/_/  _/_/_/    _/_/_/  _/      _/    _/_/_/  _/    _/  _/
    (C)2021 RisingHF, all rights reserved.
*/

#ifndef __PRINT_H__
#define __PRINT_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

void print_hex_buf( const uint8_t *buf, uint32_t size );

void print_new_line(void);


#endif // end of __PRINT_H__