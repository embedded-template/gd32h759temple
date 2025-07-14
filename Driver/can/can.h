#pragma once
#include "gd32h7xx.h"
#include "ring_buffer.h"

typedef struct can_handle_t
{
    bool bReady;
    ring_buff_t* rx_ring_buffer;
    ring_buff_t* tx_ring_buffer;
    QueueHandle_t rx_queue;
} can_handle_t;