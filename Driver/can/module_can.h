#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "can.h"



#define MODULE_RING_BUFFER_RX_SIZE 32*10    //10条数据的缓存
#define MODULE_RING_BUFFER_TX_SIZE 32*10    //10条数据的缓存

RING_BUFF_EXTERN(module_rx)
RING_BUFF_EXTERN(module_tx)

#define MODULE_CAN_DMA_BUFFER_SIZE 32*10

bool module_data_send(uint32_t id, uint8_t* pdata);
bool module_data_recv(uint8_t id, uint8_t* pdata);
uint32_t module_data_time(void);
