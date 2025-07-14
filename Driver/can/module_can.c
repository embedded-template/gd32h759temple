#include "module_can.h"

__attribute__((aligned(32))) uint8_t rx_buffer[MODULE_CAN_DMA_BUFFER_SIZE];

RING_BUFF_PRE_INIT(module_rx, MODULE_RING_BUFFER_RX_SIZE);
RING_BUFF_PRE_INIT(module_tx, MODULE_RING_BUFFER_TX_SIZE);

can_handle_t module_can_handle = {0};

can_handle_t* module_can_handle_get(void)
{
    return &module_can_handle;
}

void module_can_init(void)
{
    module_can_handle.bReady = true;
    module_can_handle.rx_ring_buffer = module_rx_ring_buffer_get();
    module_can_handle.tx_ring_buffer = module_tx_ring_buffer_get();
    //can init
    //dma init
}

void module_can_init(void)
{
    
}

