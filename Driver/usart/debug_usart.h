#pragma once


#include "gd32h7xx.h"
#include <stdio.h>
#include "ring_buffer/ring_buffer.h"

#define  CONSOLE_USARTX_MODE             0



#define  CONSOLE_USARTX                   USART1
#define  CONSOLE_USART_CLK                RCU_USART1
#define  CONSOLE_USART_BAUDRATE           115200
#define  CONSOLE_USART_GPIO_APBXCLKCMD    rcu_periph_clock_enable
#define  CONSOLE_USART_GPIO_CLK_TX        RCU_GPIOD
#define  CONSOLE_USART_GPIO_CLK_RX        RCU_GPIOD
#define  CONSOLE_USART_TX_GPIO_PORT       GPIOD   
#define  CONSOLE_USART_TX_GPIO_PIN        GPIO_PIN_5
#define  CONSOLE_USART_RX_GPIO_PORT       GPIOD
#define  CONSOLE_USART_RX_GPIO_PIN        GPIO_PIN_6
#define  CONSOLE_USART_TX_RX_GPIO_AF      GPIO_AF_7
#define  CONSOLE_TDATA_ADDRESS            (&USART_TDATA(USART1))
#define  CONSOLE_RDATA_ADDRESS            (&USART_RDATA(USART1))
#define  CONSOLE_USART_IRQ                USART1_IRQn
#define  CONSOLE_USART_IRQ_HANDLER        USART1_IRQHandler

#define  CONSOLE_DMA_BUFFER_RX_SIZE 20
#define  CONSOLE_DMA_BUFFER_TX_SIZE 200

#define  CONSOLE_RING_BUFFER_RX_SIZE 100
#define  CONSOLE_RING_BUFFER_TX_SIZE 400

RING_BUFF_EXTERN(console_rx)
RING_BUFF_EXTERN(console_tx)

typedef struct dma_buffer_t{
    uint8_t* buffer;
    uint32_t size;
}dma_buffer_t;

typedef struct console_t{
    bool         bReady;
    ring_buff_t* rx_ring_buffer;
    ring_buff_t* tx_ring_buffer;
    dma_buffer_t rx_dma_buffer;
    dma_buffer_t tx_dma_buffer;
}console_t;


void console_task(void* pvParameters);

