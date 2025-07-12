#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "ring_buffer.h"

#define MODULE_RING_BUFFER_RX_SIZE 1000 //（32+8*8+4）*10。其中32为id，8*8为数据，2字节的数据头和2字节的数据尾
#define MODULE_RING_BUFFER_TX_SIZE 1000

#define CAN_DATA_HEAD   "HE"
#define CAN_DATA_TAIL   "TL"

RING_BUFF_EXTERN(module_rx)
RING_BUFF_EXTERN(module_tx)

typedef struct module_data_t
{
    bool bReady;
    ring_buff_t* rx_ring_buffer;
    ring_buff_t* tx_ring_buffer;
} module_data_t;

bool module_data_send(uint32_t id, uint8_t* pdata);
bool module_data_recv(uint8_t id, uint8_t* pdata);
uint32_t module_data_time(void);
