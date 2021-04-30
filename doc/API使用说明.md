## LoRaMac

### void lw_init( LoRaMacRegion_t region, lw_mode_t mode, lw_evt_handler_t handler)

* **简介**
  * LoRaWAN协议栈初始化。该函数会对协议栈使用的外设以及运行参数进行初始化操作。

* **参数**
  * [in]	region	 频率计划，支持EU868， EU433
  * [in]    mode      入网模式
  * [in]    handler   LoRaWAN用户处理回调
* **返回值**
  * 无



### void lw_run(void)

* **简介**
  * 协议栈运行函数,   该函数需要在用户主循环中调用，用于处理协议栈内部事件。

* **参数**
  * 无
* **返回值**
  * 无



### int lw_join(int8_t dr)

* **简介**
  * LoRaWAN 入网请求。
  * 如果成功调用该请求，当入网事务完成时会产生`LW_EVT_ID_JOIN_DONE`事件。

* **参数**
  * [in] dr    入网速率
* **返回值**
  * LoRaMacStatus_t 枚举类型值



### bool lw_is_joined(void)

* **简介**
  * 查询LoRaWAN协议栈是否入网。

* **参数**
  * 无
* **返回值**
  * true: 入网
  * false: 未入网



### int lw_send(Mcps_t type, uint8_t port, uint8_t *buf, int size, int8_t dr)

* **简介**
  * LoRaWAN上行数据请求
  * 如果成功调用该请求，当事务完成后会产生`LW_EVT_ID_UL_DONE`事件。如果接收到下行数据，还会产生`LW_EVT_ID_RX_DONE`事件。
* **参数**
  * [in]    type   上行数据类型
  * [in]    port    上行数据端口
  * [in]    buf      用户数据指针
  * [in]    size     用户数据字节数
  * [in]    dr        上行速率，仅当ADR关闭时该参数才有效
* **返回值**
  * LoRaMacStatus_t 枚举类型值



### int lw_se_id_set(lw_se_id_t type, uint8_t *buf)

* **简介**
  * 设置LoRaWAN协议栈ID和KEY
* **参数**
  * [in] type    id和key类型
  * [in]  buf     id和key数据指针
* **返回值**
  * LoRaMacStatus_t 枚举类型值

### int lw_se_id_get(lw_se_id_t type, uint8_t *buf)

* **简介**

  * 设置LoRaWAN协议栈当前ID和KEY

    | ID类型           | 长度(单位：字节) |
    | ---------------- | ---------------- |
    | LW_SE_ID_DEVADDR | 4                |
    | LW_SE_ID_DEVEUI  | 8                |
    | LW_SE_ID_APPEUI  | 8                |
    | LW_SE_ID_APPSKEY | 16               |
    | LW_SE_ID_NWKSKEY | 16               |
    | LW_SE_ID_APPKEY  | 16               |

* **参数**

  * [in]   type    id和key类型
  * [out] buf     id和key数据指针

* **返回值**

### int lw_dr_set(int8_t dr)

* **简介**

  * 设置LoRaWAN协议栈上行速率。

* **参数**

  * [in] dr    上行速率

* **返回值**

  * LoRaMacStatus_t 枚举类型值

    

### int8_t lw_dr_get(void)

* **简介**
  * 获取LoRaWAN协议栈上行速率。
* **参数**
  * 无
* **返回值**
  * 当前上行速率



### int lw_adr_set(bool enable)

* **简介**
  * 设置LoRaWAN ADR功能
* **参数**
  * [in] enable	使能或者禁能
* **返回值**
  * LoRaMacStatus_t 枚举类型值



### bool lw_adr_get(void)

* **简介**
  * 获取LoRaWAN 当前ADR功能设置
* **参数**
  * 无
* **返回值**
  * 当前ADR功能设置



### int lw_retry_set(uint8_t cnt)

* **简介**
  * 设置LoRaWAN重复上行次数。用于改善上行服务质量。一旦接收到下行，就会立刻终止重复上行。
* **参数**
  * [in] cnt    上行次数
* **返回值**
  * LoRaMacStatus_t 枚举类型值



### int lw_retry_get(void)

* **简介**
  * 获取LoRaWAN重复上行次数配置
* **参数**
  * 无
* **返回值**
  * LoRaWAN重复上行次数



### int lw_channel_mask_set(uint16_t *mask)

* **简介**
  * 设置LoRaWAN上行信道掩码
* **参数**
  * [in]mask      信道掩码指针。每个信道对应1bit，一共96个信道。对应位置1表示开启信道，置0表示关闭信道。参数是长度为6的一维数组，如uint16_t mask[6]。
* **返回值**
  * LoRaMacStatus_t 枚举类型值



### void lw_channel_mask_get(uint16_t *mask)

* **简介**
  * 获取当前LoRaWAN上行信道掩码
* **参数**
  * [out] mask     参数是长度为6的一维数组，如uint16_t mask[6]
* **返回值**
  * 无



## Timer

### void TimerInit( TimerEvent_t *obj, void ( *callback )( void *context ) )

* **简介**
  * 定时器初始化。定时器在使用前必须先初始化。
* **参数**
  * [in] obj    定时器对象指针
  * [in] callback   定时器回调函数指针
* **返回值**
  * 无



### void TimerSetValue( TimerEvent_t *obj, uint32_t value )

* **简介**
  * 设置定时器超时时间
* **参数**
  * [in] obj    定时器对象指针
  * [in] value 超时时间，单位ms
* **返回值**

### void TimerStart(TimerEvent_t *obj)

* **简介**
  * 启动定时器。
  * 每次定时器后，只会定时一次，不会周期性定时。
* **参数**
  * [in] obj    定时器对象指针
* **返回值**
  * 无



### void TimerStop(TimerEvent_t *obj)

* **简介**
  * 停止定时器。
* **参数**
  * [in] obj    定时器对象指针
* **返回值**
  * 无



### TimerTime_t TimerGetCurrentTime( void )

* **简介**
  * 获取当前时间戳
* **参数**
  * 无
* **返回值**
  * 当前时间戳，单位ms