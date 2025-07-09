#pragma once


#include "gd32h7xx.h"
#include <stdio.h>

#define  DEBUG_USARTX_MODE                   0



#define  DEBUG_USARTX                   USART1
#define  DEBUG_USART_CLK                RCU_USART1
#define  DEBUG_USART_BAUDRATE           115200
#define  DEBUG_USART_GPIO_APBXCLKCMD    rcu_periph_clock_enable
#define  DEBUG_USART_GPIO_CLK_TX        RCU_GPIOD
#define  DEBUG_USART_GPIO_CLK_RX        RCU_GPIOD
#define  DEBUG_USART_TX_GPIO_PORT       GPIOD   
#define  DEBUG_USART_TX_GPIO_PIN        GPIO_PIN_5
#define  DEBUG_USART_RX_GPIO_PORT       GPIOD
#define  DEBUG_USART_RX_GPIO_PIN        GPIO_PIN_6
#define  DEBUG_USART_TX_RX_GPIO_AF      GPIO_AF_7

#define DEBUG_RX_BUFFER_SIZE 20
#define DEBUG_TX_BUFFER_SIZE 200


void debug_task(void* pvParameters);

