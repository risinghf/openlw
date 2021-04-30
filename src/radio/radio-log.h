/*
    _/_/_/    _/_/_/    _/_/_/  _/_/_/  _/      _/    _/_/_/  _/    _/  _/_/_/_/
   _/    _/    _/    _/          _/    _/_/    _/  _/        _/    _/  _/
  _/_/_/      _/      _/_/      _/    _/  _/  _/  _/  _/_/  _/_/_/_/  _/_/_/
 _/    _/    _/          _/    _/    _/    _/_/  _/    _/  _/    _/  _/
_/    _/  _/_/_/  _/_/_/    _/_/_/  _/      _/    _/_/_/  _/    _/  _/
 *(C)2020 RisingHF, all rights reserved.
 *
 */

#ifndef __RADIO_LOG_H__
#define __RADIO_LOG_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define RADIO_LOG_BUF_LEN                 (260)
#define RADIO_NAME                        "RADIO"

typedef struct {
    bool flag;
    uint8_t op_type; // 0: RX, 1: TX, 2: CAD
    uint8_t dt_type; // 0: RX, !0: TX
    int8_t pow;

    uint32_t freq;
    uint32_t sf;
    uint32_t bw;
    uint16_t pream;

    uint16_t wr;
    uint16_t cnt;
    uint8_t buf[RADIO_LOG_BUF_LEN];
} LogRf_t;

void RadioLogInit(void);

LogRf_t *RadioLogObjectGet(void);

void RadioLogRfDataPush(uint8_t dt_type, uint8_t *buf, int len, int16_t rssi, uint8_t snr);

void RadioLogPop(void);

#endif // end of __RADIO_LOG_H__