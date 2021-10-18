/*
    _/_/_/    _/_/_/    _/_/_/  _/_/_/  _/      _/    _/_/_/  _/    _/  _/_/_/_/
   _/    _/    _/    _/          _/    _/_/    _/  _/        _/    _/  _/
  _/_/_/      _/      _/_/      _/    _/  _/  _/  _/  _/_/  _/_/_/_/  _/_/_/
 _/    _/    _/          _/    _/    _/    _/_/  _/    _/  _/    _/  _/
_/    _/  _/_/_/  _/_/_/    _/_/_/  _/      _/    _/_/_/  _/    _/  _/
    (C)2021 RisingHF, all rights reserved.
*/

#include "lw.h"
#include "board.h"
#include "radio-log.h"
#include "log.h"
#include "region/Region.h"

static const char *const NAME = "LW";

typedef struct {
    LoRaMacRegion_t region;
    lw_mode_t mode;
    lw_evt_t evt_obj;
    lw_evt_handler_t evt_handler;
} lw_t;

static lw_t lw;

/*!
 * Used to notify LmHandler of LoRaMac events
 */
static LoRaMacPrimitives_t LoRaMacPrimitives;

/*!
 * LoRaMac callbacks
 */
static LoRaMacCallback_t LoRaMacCallbacks;


static int lw_evt_handler_process(const lw_evt_t *p_lw_evt)
{
    if(lw.evt_handler != NULL) {
        return lw.evt_handler(p_lw_evt);
    }

    return -1;
}

static void McpsConfirm( McpsConfirm_t *mcpsConfirm )
{
    lw.evt_obj.id = LW_EVT_ID_UL_DONE;
    lw.evt_obj.params.ul_done.type = mcpsConfirm->McpsRequest;
    lw.evt_obj.params.ul_done.status = mcpsConfirm->Status;
    lw.evt_obj.params.ul_done.ack = 0;
    switch(mcpsConfirm->Status){
    case LORAMAC_EVENT_INFO_STATUS_OK:
        switch( mcpsConfirm->McpsRequest ){
        default:
        case MCPS_UNCONFIRMED:
        case MCPS_PROPRIETARY:
            break;
        case MCPS_CONFIRMED:
            if (mcpsConfirm->AckReceived) {
                lw.evt_obj.params.ul_done.ack = 1;
            }
        }
    case LORAMAC_EVENT_INFO_STATUS_RX2_TIMEOUT:
    case LORAMAC_EVENT_INFO_STATUS_ERROR:
    case LORAMAC_EVENT_INFO_STATUS_RX2_ERROR:
    case LORAMAC_EVENT_INFO_STATUS_TX_TIMEOUT:
    default:
        break;
    }

    lw_evt_handler_process(&lw.evt_obj);
}

static void McpsIndication( McpsIndication_t *mcpsIndication )
{
    lw.evt_obj.id = LW_EVT_ID_RX_DONE;
    lw.evt_obj.params.rx_done.status = mcpsIndication->Status;
    lw.evt_obj.params.rx_done.type = mcpsIndication->McpsIndication;
    lw.evt_obj.params.rx_done.size = 0;
    lw.evt_obj.params.rx_done.buf = NULL;

    switch(mcpsIndication->Status){
    default:
    case LORAMAC_EVENT_INFO_STATUS_ADDRESS_FAIL:
    case LORAMAC_EVENT_INFO_STATUS_DOWNLINK_REPEATED:
    case LORAMAC_EVENT_INFO_STATUS_ERROR:
    case LORAMAC_EVENT_INFO_STATUS_MIC_FAIL:
        break;
    case LORAMAC_EVENT_INFO_STATUS_OK:
        lw.evt_obj.params.rx_done.flag.fpending = mcpsIndication->FramePending;
        lw.evt_obj.params.rx_done.flag.multicast = mcpsIndication->Multicast;
        lw.evt_obj.params.rx_done.win = mcpsIndication->RxSlot;
        lw.evt_obj.params.rx_done.rssi = mcpsIndication->Rssi;
        lw.evt_obj.params.rx_done.snr = (int8_t)(mcpsIndication->Snr / 4 + 0.5);
        if( mcpsIndication->RxData == true ){
            lw.evt_obj.params.rx_done.port = mcpsIndication->Port;
            lw.evt_obj.params.rx_done.buf = mcpsIndication->Buffer;
            lw.evt_obj.params.rx_done.size = mcpsIndication->BufferSize;;
        }
        break;
    }

    lw_evt_handler_process(&lw.evt_obj);
}

static void MlmeConfirm( MlmeConfirm_t *mlmeConfirm )
{
    switch( mlmeConfirm->MlmeRequest ) {
    case MLME_JOIN:
        lw.evt_obj.id = LW_EVT_ID_JOIN_DONE;
        lw.evt_obj.params.join_done.ok = false;

        if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK ) {
            lw.evt_obj.params.join_done.ok = true;
        }
        lw_evt_handler_process(&lw.evt_obj);
        break;
    case MLME_LINK_CHECK:
        // do nothing
        break;
    case MLME_DEVICE_TIME:
        // TODO: CLASS B
        break;
    case MLME_BEACON_ACQUISITION:
        // TODO: CLASS B
        break;
    case MLME_PING_SLOT_INFO:
        // TODO: CLASS B
        break;
    default:
        break;
    }
}

static void MlmeIndication( MlmeIndication_t *mlmeIndication )
{
    switch( mlmeIndication->MlmeIndication ){
    case MLME_SCHEDULE_UPLINK:
        // The MAC layer signals that we shall provide an uplink as soon as possible
        break;
    case MLME_BEACON_LOST:
        // TODO: CLASS B
        break;
    case MLME_BEACON:
        // TODO: CLASS B
        break;
    default:
        break;
    }
}

void lw_init( LoRaMacRegion_t region, lw_mode_t mode, lw_evt_handler_t handler)
{
    lw.region = region;
    lw.mode = mode;
    lw.evt_handler = handler;
    RadioLogInit();

    LoRaMacPrimitives.MacMcpsConfirm = McpsConfirm;
    LoRaMacPrimitives.MacMcpsIndication = McpsIndication;
    LoRaMacPrimitives.MacMlmeConfirm = MlmeConfirm;
    LoRaMacPrimitives.MacMlmeIndication = MlmeIndication;

    LoRaMacCallbacks.GetBatteryLevel = BoardGetBatteryLevel;
    LoRaMacCallbacks.GetTemperatureLevel = NULL;;
    LoRaMacCallbacks.NvmDataChange = NULL;
    LoRaMacCallbacks.MacProcessNotify = NULL;

    LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, region );

    /** TODO: restore from nvm */
    LoRaMacStart( );
}

void lw_run(void)
{
    /* Processes the LoRaMac events */
    LoRaMacProcess( );

    /** Processes the radio log */
    RadioLogPop( );


    void LoRaMacDebug(void);
    LoRaMacDebug();
}

int lw_join(int8_t dr)
{
    LoRaMacStatus_t status;
    MlmeReq_t mlmeReq;

    mlmeReq.Type = MLME_JOIN;
    mlmeReq.Req.Join.Datarate = dr;

    if(lw.mode == LW_MODE_ABP) {
        mlmeReq.Req.Join.NetworkActivation = ACTIVATION_TYPE_ABP;
    } else {
        mlmeReq.Req.Join.NetworkActivation = ACTIVATION_TYPE_OTAA;
    }

    status = LoRaMacMlmeRequest( &mlmeReq );

    return status;
}

bool lw_is_joined(void)
{
    MibRequestConfirm_t mibReq;
    LoRaMacStatus_t status;

    mibReq.Type = MIB_NETWORK_ACTIVATION;
    status = LoRaMacMibGetRequestConfirm( &mibReq );

    if((status == LORAMAC_STATUS_OK) && (mibReq.Param.NetworkActivation != ACTIVATION_TYPE_NONE)) {
        return true;
    }

    return false;
}


int lw_send(Mcps_t type, uint8_t port, uint8_t *buf, int size, int8_t dr)
{
    LoRaMacStatus_t status;
    McpsReq_t mcpsReq;
    LoRaMacTxInfo_t txInfo;

    status = LoRaMacQueryTxPossible( size, &txInfo );

    if( status != LORAMAC_STATUS_OK ){
        return status;
    }else{
        mcpsReq.Type = type;
        switch(type){
        case MCPS_UNCONFIRMED:
            mcpsReq.Req.Unconfirmed.fPort = port;
            mcpsReq.Req.Unconfirmed.fBuffer = buf;
            mcpsReq.Req.Unconfirmed.fBufferSize = size;
            mcpsReq.Req.Unconfirmed.Datarate = dr;
            break;
        case MCPS_CONFIRMED:
            mcpsReq.Req.Confirmed.fPort = port;
            mcpsReq.Req.Confirmed.fBuffer = buf;
            mcpsReq.Req.Confirmed.fBufferSize = size;
            mcpsReq.Req.Confirmed.Datarate = dr;
            break;
        case MCPS_PROPRIETARY:
            mcpsReq.Req.Proprietary.fBuffer = buf;
            mcpsReq.Req.Proprietary.fBufferSize = size;
            mcpsReq.Req.Proprietary.Datarate = dr;
            break;
        default:
            return LORAMAC_STATUS_PARAMETER_INVALID;
        }
    }

    status = LoRaMacMcpsRequest( &mcpsReq );
    if(status == LORAMAC_STATUS_OK){
        LOG(WARN, "tx %d, %h(%d)", port, buf, size, size);
    }else{
        LOG(WARN, "tx fail %d", status);
    }
    return status;
}

/*
static KeyAddr_t KeyAddrList[NUM_OF_SEC_CTX] =
{
    { MULTICAST_0_ADDR, MC_APP_S_KEY_0, MC_NWK_S_KEY_0, MC_KEY_0 },
    { MULTICAST_1_ADDR, MC_APP_S_KEY_1, MC_NWK_S_KEY_1, MC_KEY_1 },
    { MULTICAST_2_ADDR, MC_APP_S_KEY_2, MC_NWK_S_KEY_2, MC_KEY_2 },
    { MULTICAST_3_ADDR, MC_APP_S_KEY_3, MC_NWK_S_KEY_3, MC_KEY_3 },
    { UNICAST_DEV_ADDR, APP_S_KEY, S_NWK_S_INT_KEY, NO_KEY }
};
*/
int lw_se_id_set(lw_se_id_t type, uint8_t *buf)
{
    MibRequestConfirm_t mibReq;

    if(buf == NULL){
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

    switch(type){
    case LW_SE_ID_DEVADDR:
        mibReq.Type = MIB_DEV_ADDR;
        memcpy1((uint8_t *)(&mibReq.Param.DevAddr), buf, 4);
        break;
    case LW_SE_ID_DEVEUI:
        mibReq.Type = MIB_DEV_EUI;
        mibReq.Param.DevEui = buf;
        break;
    case LW_SE_ID_APPEUI:
        mibReq.Type = MIB_JOIN_EUI;
        mibReq.Param.JoinEui = buf;
        break;
    case LW_SE_ID_APPSKEY:
        mibReq.Type = MIB_APP_S_KEY;
        mibReq.Param.AppSKey = buf;
        break;
    case LW_SE_ID_NWKSKEY:
        mibReq.Type = MIB_S_NWK_S_INT_KEY;
        mibReq.Param.SNwkSIntKey = buf;
        break;
    case LW_SE_ID_APPKEY:
        mibReq.Type = MIB_NWK_KEY;
        mibReq.Param.NwkKey = buf;
        break;
    default:
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

     /* Update LoRaMac id */
    return LoRaMacMibSetRequestConfirm( &mibReq );
}

int lw_se_id_get(lw_se_id_t type, uint8_t *buf)
{
    MibRequestConfirm_t mibReq;
    Key_t* keyItem;

    void GetKeyByIDExt(KeyIdentifier_t keyID, Key_t** keyItem);

    if(buf == NULL){
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

    switch(type){
    case LW_SE_ID_DEVADDR:
        mibReq.Type = MIB_DEV_ADDR;
        LoRaMacMibGetRequestConfirm( &mibReq );
        memcpy1(buf, (const uint8_t *)(&mibReq.Param.DevAddr), 4);
        break;
    case LW_SE_ID_DEVEUI:
        mibReq.Type = MIB_DEV_EUI;
        LoRaMacMibGetRequestConfirm( &mibReq );
        memcpy1(buf, mibReq.Param.DevEui, SE_EUI_SIZE);
        break;
    case LW_SE_ID_APPEUI:
        mibReq.Type = MIB_JOIN_EUI;
        LoRaMacMibGetRequestConfirm( &mibReq );
        memcpy1(buf, mibReq.Param.JoinEui, SE_EUI_SIZE);
        break;
    case LW_SE_ID_APPSKEY:
        GetKeyByIDExt( APP_S_KEY, &keyItem );
        memcpy1(buf, keyItem->KeyValue, SE_KEY_SIZE);
        break;
    case LW_SE_ID_NWKSKEY:
        GetKeyByIDExt( S_NWK_S_INT_KEY, &keyItem );
        memcpy1(buf, keyItem->KeyValue, SE_KEY_SIZE);
        break;
    case LW_SE_ID_APPKEY:
        GetKeyByIDExt( NWK_KEY, &keyItem );
        memcpy1(buf, keyItem->KeyValue, SE_KEY_SIZE);
        break;
    default:
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

    return LORAMAC_STATUS_OK;
}

int lw_dr_set(int8_t dr)
{
    MibRequestConfirm_t mibReq;
    LoRaMacStatus_t Status;

    mibReq.Type = MIB_CHANNELS_DEFAULT_DATARATE;
    mibReq.Param.ChannelsDatarate = dr;
    Status = LoRaMacMibSetRequestConfirm( &mibReq );
    if(LORAMAC_STATUS_OK != Status) {
        return Status;
    }

    mibReq.Type = MIB_CHANNELS_DATARATE;
    mibReq.Param.ChannelsDatarate = dr;
    return LoRaMacMibSetRequestConfirm( &mibReq );
}


int8_t lw_dr_get(void)
{
    MibRequestConfirm_t mibReq;

    mibReq.Type = MIB_CHANNELS_DATARATE;
    LoRaMacMibGetRequestConfirm( &mibReq );

    return mibReq.Param.ChannelsDatarate;
}

int lw_adr_set(bool enable)
{
    MibRequestConfirm_t mibReq;

    mibReq.Type = MIB_ADR;
    mibReq.Param.AdrEnable = enable;

    return LoRaMacMibSetRequestConfirm( &mibReq );
}

bool lw_adr_get(void)
{
    MibRequestConfirm_t mibReq;

    mibReq.Type = MIB_ADR;
    LoRaMacMibGetRequestConfirm( &mibReq );

    return mibReq.Param.AdrEnable;
}

int lw_retry_set(uint8_t cnt)
{
    MibRequestConfirm_t mibReq;

    mibReq.Type = MIB_CHANNELS_NB_TRANS;
    mibReq.Param.ChannelsNbTrans = cnt;

    return LoRaMacMibSetRequestConfirm( &mibReq );
}

int lw_retry_get(void)
{
    MibRequestConfirm_t mibReq;

    mibReq.Type = MIB_CHANNELS_NB_TRANS;
    LoRaMacMibGetRequestConfirm( &mibReq );

    return mibReq.Param.ChannelsNbTrans;
}

int lw_channel_mask_set(uint16_t *mask)
{
    MibRequestConfirm_t mibReq;
    int ret;

    mibReq.Type = MIB_CHANNELS_DEFAULT_MASK;
    mibReq.Param.ChannelsMask = mask;

    ret = LoRaMacMibSetRequestConfirm( &mibReq );
    if(LORAMAC_STATUS_OK != ret) {
        return ret;
    }

    mibReq.Type = MIB_CHANNELS_MASK;
    mibReq.Param.ChannelsMask = mask;
    return LoRaMacMibSetRequestConfirm( &mibReq );
}

void lw_channel_mask_get(uint16_t *mask)
{
    MibRequestConfirm_t mibReq;

    mibReq.Type = MIB_CHANNELS_MASK;
    LoRaMacMibGetRequestConfirm( &mibReq );

    if(mask != NULL) {
        memcpy1((uint8_t *)mask, (uint8_t *)(mibReq.Param.ChannelsMask), REGION_NVM_CHANNELS_MASK_SIZE);
    }

}

int lw_channel_params_set(uint8_t ch_inx, const ChannelParams_t *ch_params)
{
    uint8_t ch_num_max;
    GetPhyParams_t getPhy;
    PhyParam_t phyParam;

    getPhy.Attribute = PHY_MAX_NB_CHANNELS;
    phyParam = RegionGetPhyParam( lw.region, &getPhy );
    ch_num_max = phyParam.Value;

    if((ch_inx >= ch_num_max) || (ch_params == NULL)) {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

    // TODO check ch_params


    if( LoRaMacIsBusy( ) == true ) {
        return LORAMAC_STATUS_BUSY;
    }

    getPhy.Attribute = PHY_CHANNELS;
    phyParam = RegionGetPhyParam( lw.region, &getPhy );
    phyParam.Channels[ch_inx] = *ch_params;

    return LORAMAC_STATUS_OK;
}

int lw_channel_get(uint8_t ch_inx, ChannelParams_t *ch_params)
{
    uint8_t ch_num_max;
    GetPhyParams_t getPhy;
    PhyParam_t phyParam;

    getPhy.Attribute = PHY_MAX_NB_CHANNELS;
    phyParam = RegionGetPhyParam( lw.region, &getPhy );
    ch_num_max = phyParam.Value;

    if((ch_inx >= ch_num_max) || (ch_params == NULL)) {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

    getPhy.Attribute = PHY_CHANNELS;
    phyParam = RegionGetPhyParam( lw.region, &getPhy );
    *ch_params = phyParam.Channels[ch_inx];

    return LORAMAC_STATUS_OK;
}




