#include <stdio.h>
#include "stm32l0xx.h"
#include "utilities.h"
#include "board.h"
#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "lw.h"
#include "log.h"

bool BoardIsIdle( void );

#if defined(__CC_ARM)
static const char *toolchains = "MDK";
#elif defined(__ICCARM__)
static const char *toolchains = "IAR";
#elif defined(__GNUC__)
static const char *toolchains = "GCC";
#endif

static const char *const NAME = "MAIN";

static TimerEvent_t SysTimer;
static volatile uint8_t SysFlag = 1;
static void OnSysTimerEvent(void* context)
{
    SysFlag = 1;
    TimerStart( &SysTimer );
}

static void print_se_id(void)
{
    uint8_t id_buf[SE_KEY_SIZE];
    uint32_t devaddr;

    lw_se_id_get(LW_SE_ID_DEVADDR, (uint8_t *)(&devaddr));
    LOG(INFO, "DevAddr: %08X", devaddr);

    lw_se_id_get(LW_SE_ID_DEVEUI, id_buf);
    LOG(INFO, "DevEui: %h", id_buf, SE_EUI_SIZE);

    lw_se_id_get(LW_SE_ID_APPEUI, id_buf);
    LOG(INFO, "JoinEui: %h", id_buf, SE_EUI_SIZE);

    lw_se_id_get(LW_SE_ID_APPSKEY, id_buf);
    LOG(INFO, "AppSKey: %h", id_buf, SE_KEY_SIZE);

    lw_se_id_get(LW_SE_ID_NWKSKEY, id_buf);
    LOG(INFO, "NwkSKey: %h", id_buf, SE_KEY_SIZE);

    lw_se_id_get(LW_SE_ID_APPKEY, id_buf);
    LOG(INFO, "AppKey: %h", id_buf, SE_KEY_SIZE);
}

static int lw_evt_dispatch(const lw_evt_t *p_lw_evt)
{
    switch(p_lw_evt->id){
    case LW_EVT_ID_JOIN_DONE:
        if(p_lw_evt->params.join_done.ok) {
            LOG(DEBUG, "join ok");
            print_se_id();
        } else {
            LOG(DEBUG, "join ng");
        }
        LOG(DEBUG, "join done");
        break;
    case LW_EVT_ID_UL_DONE:
        if(p_lw_evt->params.ul_done.ack) {
            LOG(DEBUG, "ack receive");
        }
        LOG(DEBUG, "ul done");
        break;
    case LW_EVT_ID_RX_DONE:
        LOG(DEBUG, "rssi: %d, snr:%d", p_lw_evt->params.rx_done.rssi, p_lw_evt->params.rx_done.snr);
        if((p_lw_evt->params.rx_done.buf != NULL) &&
           (p_lw_evt->params.rx_done.size != 0)) {
            LOG(DEBUG, "port: %u, data: %h",
                p_lw_evt->params.rx_done.port,
                p_lw_evt->params.rx_done.buf, p_lw_evt->params.rx_done.size);
        }
        LOG(DEBUG, "rx done");
        break;
    default:
        break;
    }

    return 0;
}

int main(void)
{
    uint32_t devaddr = 0x12345678;
    uint8_t deveui[8] = {0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x01};
    uint16_t cn470_chmask[6] = {0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};
    uint8_t lw_tx_buf[8] = {1, 2, 3, 4, 5, 6, 7, 8};

    BoardInitMcu( );
    BoardInitPeriph( );

    log_set_level(DEBUG);

#if 0
    // test for EU868
    lw_init(LORAMAC_REGION_EU868, LW_MODE_OTAA, lw_evt_dispatch);
#else
    // test for CN470OLD CH00~CH07
    lw_init(LORAMAC_REGION_CN470OLD, LW_MODE_OTAA, lw_evt_dispatch);
    lw_channel_mask_set(cn470_chmask);
#endif

    lw_se_id_set(LW_SE_ID_DEVADDR, (uint8_t *)(&devaddr));
    lw_se_id_set(LW_SE_ID_DEVEUI, deveui);

    LOG(INFO, "ToolChains: %s", toolchains);
    print_se_id();

    TimerInit( &SysTimer, OnSysTimerEvent );
    TimerSetValue( &SysTimer,  10000);
    TimerStart( &SysTimer );

    while(1) {
        if(SysFlag) {
            SysFlag = 0;
            if(!lw_is_joined()) {
                lw_join(0);
                LOG(DEBUG, "do join");
            } else {
                lw_send(MCPS_CONFIRMED, 8, lw_tx_buf, sizeof(lw_tx_buf), 2);
            }
        }

        lw_run();

        if( BoardIsIdle() ) {
            //enter lowpowr
            EnterLowPowerHandler( );
        }
    }
}

