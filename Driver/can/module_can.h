#pragma once

#include "FreeRTOS.h"
#include "can.h"
#include "task.h"

#define MODULE_RING_BUFFER_RX_SIZE 16 * 10 // 10条数据的缓存
#define MODULE_RING_BUFFER_TX_SIZE 16 * 10 // 10条数据的缓存

RING_BUFF_EXTERN(module_rx)
RING_BUFF_EXTERN(module_tx)

/* CAN硬件配置宏定义 */
#define MODULE_CAN_PERIPH CAN1
#define MODULE_CAN_IDX IDX_CAN1
#define MODULE_CAN_CLOCK_SOURCE RCU_CANSRC_APB2
#define MODULE_CAN_RCU RCU_CAN1
#define MODULE_CAN_IRQn CAN1_Message_IRQn
#define MODULE_CAN_IRQHandler CAN1_Message_IRQHandler

/* CAN GPIO配置宏定义 */
#define MODULE_CAN_GPIO_PORT GPIOB
#define MODULE_CAN_GPIO_RCU RCU_GPIOB
#define MODULE_CAN_RX_PIN GPIO_PIN_5
#define MODULE_CAN_TX_PIN GPIO_PIN_13
#define MODULE_CAN_GPIO_AF GPIO_AF_9

/* CAN邮箱配置宏定义 */
#define MODULE_CAN_TX_MAILBOX 8
#define MODULE_CAN_TX_MAILBOX_FLAG CAN_FLAG_MB8

/* CAN中断配置宏定义 */
#define MODULE_CAN_IRQ_PRIORITY 1U
#define MODULE_CAN_IRQ_SUB_PRIORITY 0U

/* CAN FIFO配置宏定义 */
#define MODULE_CAN_FIFO_ADDRESS ((uint32_t) CAN_RAM(MODULE_CAN_PERIPH))
#define MODULE_CAN_DMA_RX_REQUEST DMA_REQUEST_CAN0

bool module_data_send(uint32_t id, uint8_t* pdata);
bool module_data_recv(uint8_t id, uint8_t* pdata);
uint32_t module_data_time(void);
