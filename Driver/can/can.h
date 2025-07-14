#pragma once
#include "ring_buffer.h"



typedef struct dma_can_buffer_t
{
    uint8_t *buffer;
    uint32_t message_size;
} dma_can_buffer_t;

typedef struct can_handle_t
{
    bool bReady;
    ring_buff_t* rx_ring_buffer;
    ring_buff_t* tx_ring_buffer;
} can_handle_t;