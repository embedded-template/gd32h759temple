#ifndef __USART_H
#define	__USART_H


#include "gd32h7xx.h"
#include <stdio.h>

#define  DEBUG_USARTX_MODE                   0



#define  DEBUG_USARTX                   USART0
#define  DEBUG_USART_CLK                RCU_USART0
#define  DEBUG_USART_BAUDRATE           115200
#define  DEBUG_USART_GPIO_APBXCLKCMD    rcu_periph_clock_enable
#define  DEBUG_USART_GPIO_CLK_TX        RCU_GPIOA
#define  DEBUG_USART_GPIO_CLK_RX        RCU_GPIOA
#define  DEBUG_USART_TX_GPIO_PORT       GPIOA   
#define  DEBUG_USART_TX_GPIO_PIN        GPIO_PIN_9
#define  DEBUG_USART_RX_GPIO_PORT       GPIOA
#define  DEBUG_USART_RX_GPIO_PIN        GPIO_PIN_10
#define  DEBUG_USART_TX_RX_GPIO_AF      GPIO_AF_7

#define  DEBUG_USART_IRQ                USART0_IRQn
#define  DEBUG_USART_IRQHANDLER         USART0_IRQHandler


void DEBUG_USART_Config(void);

#endif
