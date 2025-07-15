#pragma once
#include "gd32h7xx.h"
#include "ring_buffer.h"
#include "driver_test_config.h"

typedef struct can_handle_t
{
    bool bReady;
    ring_buff_t* rx_ring_buffer;
    ring_buff_t* tx_ring_buffer;
    QueueHandle_t rx_queue;
} can_handle_t;

typedef struct can_data_t
{
    uint32_t id;
    uint8_t data[8];
} can_data_t;