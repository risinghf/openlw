/*
    _/_/_/    _/_/_/    _/_/_/  _/_/_/  _/      _/    _/_/_/  _/    _/  _/_/_/_/
   _/    _/    _/    _/          _/    _/_/    _/  _/        _/    _/  _/
  _/_/_/      _/      _/_/      _/    _/  _/  _/  _/  _/_/  _/_/_/_/  _/_/_/
 _/    _/    _/          _/    _/    _/    _/_/  _/    _/  _/    _/  _/
_/    _/  _/_/_/  _/_/_/    _/_/_/  _/      _/    _/_/_/  _/    _/  _/
    (C)2021 RisingHF, all rights reserved.
*/

#ifndef __LW_H__
#define __LW_H__

#include "LoRaMac.h"

typedef enum{
    LW_MODE_ABP,
    LW_MODE_OTAA,
}lw_mode_t;

typedef enum {
    LW_EVT_ID_JOIN_DONE = 0,
    LW_EVT_ID_UL_DONE,
    LW_EVT_ID_RX_DONE,
} lw_evt_id_t;

typedef enum {
    LW_SE_ID_DEVADDR = 0,
    LW_SE_ID_DEVEUI,
    LW_SE_ID_APPEUI,
    LW_SE_ID_APPSKEY,
    LW_SE_ID_NWKSKEY,
    LW_SE_ID_APPKEY,
} lw_se_id_t;

typedef struct {
    lw_evt_id_t id;
    union {
        struct {
            uint8_t ok;
        } join_done;

        struct {
            Mcps_t type;
            LoRaMacEventInfoStatus_t status;
            uint8_t ack;
        } ul_done;

        struct {
            Mcps_t type;
            LoRaMacEventInfoStatus_t status;
            struct {
                uint8_t fpending        : 1;
                uint8_t multicast       : 1;
                uint8_t maccmd          : 1;
                uint8_t rfu3            : 1;
                uint8_t rfu4            : 1;
                uint8_t rfu5            : 1;
                uint8_t rfu6            : 1;
                uint8_t rfu7            : 1;
            } flag;
            int16_t rssi;
            int8_t  snr;
            uint8_t win;
            uint8_t port;
            uint8_t *buf;
            uint16_t size;
        } rx_done;
    } params;
} lw_evt_t;

typedef int (*lw_evt_handler_t) (const lw_evt_t *p_lw_evt);

void lw_init( LoRaMacRegion_t region, lw_mode_t mode, lw_evt_handler_t handler);

void lw_run(void);

int lw_join(int8_t dr);

bool lw_is_joined(void);

int lw_send(Mcps_t type, uint8_t port, uint8_t *buf, int size, int8_t dr);

int lw_se_id_set(lw_se_id_t type, uint8_t *buf);
int lw_se_id_get(lw_se_id_t type, uint8_t *buf);

int lw_dr_set(int8_t dr);
int8_t lw_dr_get(void);

int lw_adr_set(bool enable);
bool lw_adr_get(void);

int lw_retry_set(uint8_t cnt);
int lw_retry_get(void);

int lw_channel_mask_set(uint16_t *mask);
void lw_channel_mask_get(uint16_t *mask);

int lw_channel_params_set(uint8_t ch_inx, const ChannelParams_t *ch_params);
int lw_channel_get(uint8_t ch_inx, ChannelParams_t *ch_params);

#endif // end of __LW_H__