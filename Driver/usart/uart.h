#pragma once

#include "ring_buffer.h"

typedef struct dma_uart_buffer_t
{
    uint8_t* buffer;
    uint32_t size;
} dma_uart_buffer_t;

typedef struct uart_handle_t
{
    bool bReady;
    ring_buff_t* rx_ring_buffer;
    ring_buff_t* tx_ring_buffer;
    dma_uart_buffer_t rx_dma_buffer;
    dma_uart_buffer_t tx_dma_buffer;
} uart_handle_t;