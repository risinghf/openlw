/*
    _/_/_/    _/_/_/    _/_/_/  _/_/_/  _/      _/    _/_/_/  _/    _/  _/_/_/_/
   _/    _/    _/    _/          _/    _/_/    _/  _/        _/    _/  _/
  _/_/_/      _/      _/_/      _/    _/  _/  _/  _/  _/_/  _/_/_/_/  _/_/_/
 _/    _/    _/          _/    _/    _/    _/_/  _/    _/  _/    _/  _/
_/    _/  _/_/_/  _/_/_/    _/_/_/  _/      _/    _/_/_/  _/    _/  _/
    (C)2021 RisingHF, all rights reserved.
*/

#include "radio-log.h"
#include "log.h"
#include "utilities.h"


static LogRf_t LogRf;

void RadioLogInit(void)
{
    memset(&LogRf, 0, sizeof(LogRf_t));
}

LogRf_t *RadioLogObjectGet(void)
{
    return &LogRf;
}

/*
    Element structure:
    len | type | rssi | snr | buf
*/
void RadioLogRfDataPush(uint8_t dt_type, uint8_t *buf, int len, int16_t rssi, uint8_t snr)
{
    if(DEBUG > log_get_level() ) {
        return;
    }

    LogRf_t *log_rf = &LogRf;
    int i;

    CRITICAL_SECTION_BEGIN();

    if ((log_rf->cnt + len + 5) > RADIO_LOG_BUF_LEN) {
        CRITICAL_SECTION_END();
        return;
    }

    log_rf->buf[log_rf->wr++] = len;
    log_rf->buf[log_rf->wr++] = dt_type;
    log_rf->buf[log_rf->wr++] = (rssi >> 0);
    log_rf->buf[log_rf->wr++] = (rssi >> 8);
    log_rf->buf[log_rf->wr++] = snr;
    for (i = 0; i < len; i++) {
        log_rf->buf[log_rf->wr] = buf[i];
        log_rf->wr++;
    }
    log_rf->cnt += len + 5;

    CRITICAL_SECTION_END();
}

void RadioLogPop(void)
{
    if(DEBUG > log_get_level() ) {
        return;
    }

    LogRf_t *log_rf = &LogRf;;
    const char *radio_name = RADIO_NAME;

    if(log_rf->flag == true){
        switch(log_rf->bw){
        case 7:
            log_rf->bw = 125;
            break;
        case 8:
            log_rf->bw = 250;
            break;
        case 9:
            log_rf->bw = 500;
            break;
        }
        switch (log_rf->op_type) {
        case 0:
            /* RX */
            if(log_rf->sf != 50000){
                log_puts(DEBUG, radio_name, "%s, %d, SF%d, %dKHz, %d", "RX", log_rf->freq, log_rf->sf, log_rf->bw, log_rf->pream);
            }else{
                log_puts(DEBUG, radio_name, "%s, %d, FSK %dbps", "RX", log_rf->freq, log_rf->sf);
            }
            break;
        case 1:
            /* TX */
            if(log_rf->sf != 50000){
                log_puts(DEBUG, radio_name, "%s, %d, SF%d, %dKHz, %d, %d", "TX", log_rf->freq, log_rf->sf, log_rf->bw, log_rf->pream, log_rf->pow);
            }else{
                log_puts(DEBUG, radio_name, "%s, %d, FSK %dbps, %d", "TX", log_rf->freq, log_rf->sf, log_rf->pow);
            }
            break;
        case 2:
            /* CAD */
            log_puts(DEBUG, radio_name, "%s, %d, SF%d, %dKHz", "CAD", log_rf->freq, log_rf->sf, log_rf->bw);
            break;
        }
        log_rf->flag = false;
    }

    {
        int i;
        uint8_t dt_type;
        uint16_t len;
        uint16_t rssi;
        int8_t snr;

        len = 0;
        if (log_rf->cnt > 0) {
            i = 0;
            len = log_rf->buf[i++];
            dt_type = log_rf->buf[i++];
            rssi = ((uint16_t)log_rf->buf[i++] << 0);
            rssi |= ((uint16_t)log_rf->buf[i++] << 8);
            snr = (int8_t)log_rf->buf[i++];

            if (dt_type) {
                log_puts(DEBUG, radio_name, "%s, %h",
                         "TX",
                         log_rf->buf + 5,
                         len);
            } else {
                log_puts(DEBUG, radio_name, "%s, %h, %d, %d",
                         "RX",
                         log_rf->buf + 5, len,
                         (int16_t)rssi,
                         (int)(((int8_t)snr)/4.0+0.5));
            }

            CRITICAL_SECTION_BEGIN();

            log_rf->cnt -= (len + 5);
            for (i = 0; i < log_rf->cnt; i++) {
                log_rf->buf[i] = log_rf->buf[len + 5 + i];
            }
            log_rf->wr = log_rf->cnt;

            CRITICAL_SECTION_END();
        }
    }
}
