#include "module_can.h"

__attribute__((aligned(32))) uint32_t rx_buffer[4];

RING_BUFF_PRE_INIT(module_rx, MODULE_RING_BUFFER_RX_SIZE);
RING_BUFF_PRE_INIT(module_tx, MODULE_RING_BUFFER_TX_SIZE);

can_handle_t module_can_handle = {0};

can_handle_t* module_can_handle_get(void)
{
    return &module_can_handle;
}

static void module_can_hw_init(void);

void module_can_init(void)
{
    module_can_handle.bReady = true;
    module_can_handle.rx_ring_buffer = module_rx_ring_buffer();
    module_can_handle.tx_ring_buffer = module_tx_ring_buffer();
    module_can_handle.rx_queue = xQueueCreate(10, sizeof(can_rx_fifo_struct));
    module_can_hw_init();
}

/**
 * @brief 使用fifo接收，使用邮箱8发送
 */
static void module_can_hw_init(void)
{
    rcu_can_clock_config(MODULE_CAN_IDX, MODULE_CAN_CLOCK_SOURCE);
    rcu_periph_clock_enable(MODULE_CAN_RCU);
    rcu_periph_clock_enable(MODULE_CAN_GPIO_RCU);

    /* configure RX GPIO */
    gpio_output_options_set(MODULE_CAN_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, MODULE_CAN_RX_PIN);
    gpio_mode_set(MODULE_CAN_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, MODULE_CAN_RX_PIN);
    gpio_af_set(MODULE_CAN_GPIO_PORT, MODULE_CAN_GPIO_AF, MODULE_CAN_RX_PIN);
    /* configure TX GPIO */
    gpio_output_options_set(MODULE_CAN_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, MODULE_CAN_TX_PIN);
    gpio_mode_set(MODULE_CAN_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, MODULE_CAN_TX_PIN);
    gpio_af_set(MODULE_CAN_GPIO_PORT, MODULE_CAN_GPIO_AF, MODULE_CAN_TX_PIN);

    can_parameter_struct can_parameter;
    can_rx_fifo_id_filter_struct can_fifo_parameter;

    /* initialize CAN register */
    can_deinit(MODULE_CAN_PERIPH);

    /* initialize CAN */
    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
    can_struct_para_init(CAN_FIFO_INIT_STRUCT, &can_fifo_parameter);

    /* initialize CAN parameters */
    can_parameter.internal_counter_source = CAN_TIMER_SOURCE_BIT_CLOCK;
    can_parameter.self_reception = DISABLE;
    can_parameter.mb_tx_order = CAN_TX_HIGH_PRIORITY_MB_FIRST;
    can_parameter.mb_tx_abort_enable = ENABLE;
    can_parameter.local_priority_enable = DISABLE;
    can_parameter.mb_rx_ide_rtr_type = CAN_IDE_RTR_FILTERED;
    can_parameter.mb_remote_frame = CAN_STORE_REMOTE_REQUEST_FRAME;
    can_parameter.rx_private_filter_queue_enable = DISABLE;
    can_parameter.edge_filter_enable = DISABLE;
    can_parameter.protocol_exception_enable = DISABLE;
    can_parameter.rx_filter_order = CAN_RX_FILTER_ORDER_MAILBOX_FIRST;
    can_parameter.memory_size = CAN_MEMSIZE_32_UNIT;
    /* filter configuration */
    can_parameter.mb_public_filter = 0U;
    /* baud rate 1Mbps, sample point at 80% */
    can_parameter.resync_jump_width = 1U;
    can_parameter.prop_time_segment = 2U;
    can_parameter.time_segment_1 = 5U;
    can_parameter.time_segment_2 = 2U;
    can_parameter.prescaler = 30U;
    can_init(MODULE_CAN_PERIPH, &can_parameter);

    can_operation_mode_enter(MODULE_CAN_PERIPH, CAN_INACTIVE_MODE);
    /* 设置fifo*/
    can_fifo_parameter.dma_enable = DISABLE;
    can_fifo_parameter.filter_format_and_number = CAN_RXFIFO_FILTER_A_NUM_8; // A类过滤器格式。默认八个。
    can_fifo_parameter.fifo_public_filter = 0U;                              // 暂不设置过滤
    can_rx_fifo_config(MODULE_CAN_PERIPH, &can_fifo_parameter);

    /*清空fifo*/
    can_rx_fifo_clear(MODULE_CAN_PERIPH);
    /*直接清空M8*/
    can_flag_clear(MODULE_CAN_PERIPH, MODULE_CAN_TX_MAILBOX_FLAG);

    nvic_irq_enable(MODULE_CAN_IRQn, MODULE_CAN_IRQ_PRIORITY, MODULE_CAN_IRQ_SUB_PRIORITY);

    can_interrupt_enable(MODULE_CAN_PERIPH, CAN_INT_FIFO_AVAILABLE);

    can_operation_mode_enter(MODULE_CAN_PERIPH, CAN_NORMAL_MODE);
}

/**
 * @brief CAN发送任务。从环形缓冲区中读取数据，发送出去
 *
 */
void module_tx_task(void* pvParameters)
{
    can_rx_fifo_struct rx_fifo;
    can_mailbox_descriptor_struct mdpara;
    uint32_t data[3] = {0};
    while (1)
    {
        // 是否有数据需要发送
        while (!module_can_handle.rx_ring_buffer->is_empty())
        {
            can_struct_para_init(CAN_MDSC_STRUCT, &mdpara);
            module_can_handle.rx_ring_buffer->read((uint8_t*) &data, sizeof(data), 100);
            mdpara.rtr = 1U;
            mdpara.ide = 1U;
            mdpara.code = CAN_MB_TX_STATUS_DATA;
            mdpara.data_bytes = 8u;
            /* 发送的内容 */
            mdpara.id = data[0];
            mdpara.data = (uint32_t*) (data[1]);

            can_mailbox_message_transmit(MODULE_CAN_PERIPH, MODULE_CAN_TX_MAILBOX, &mdpara);

            if (can_flag_get(MODULE_CAN_PERIPH, MODULE_CAN_TX_MAILBOX_FLAG) == RESET)
            {
                // 等待发送完成
                vTaskDelay(1);
            }
            can_flag_clear(MODULE_CAN_PERIPH, MODULE_CAN_TX_MAILBOX_FLAG);
        }
    }
}

/**
 * @brief CAN接收任务.把输入放入环形缓冲区，供应用层使用
 *
 */
void module_rx_task(void* pvParameters)
{
    can_rx_fifo_struct rx_fifo;
    uint32_t data[3] = {0};
    while (1)
    {
        while (xQueueReceive(module_can_handle.rx_queue, &rx_fifo, 0) == pdTRUE)
        {
            // 只保留id和data
            data[0] = rx_fifo.id;
            data[1] = rx_fifo.data[0];
            data[2] = rx_fifo.data[1];
            module_can_handle.rx_ring_buffer->write((uint8_t*) data, sizeof(data), 100);
        }
    }

    vTaskDelay(pdMS_TO_TICKS(*(uint32_t*) pvParameters)));
}

/**
 * @brief CAN中断。FIFO中有数据时，触发，将数据通过Queue发送给接收任务
 *
 */
void MODULE_CAN_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdTRUE;
    if (can_interrupt_flag_get(CAN1, CAN_INT_FLAG_FIFO_AVAILABLE) == SET)
    {
        can_rx_fifo_read(CAN1, (can_rx_fifo_struct*) rx_buffer);
        xQueueSendFromISR(module_can_handle.rx_queue, (void*) rx_buffer, xHigherPriorityTaskWoken);
        can_interrupt_flag_clear(CAN1, CAN_INT_FLAG_FIFO_AVAILABLE);
    }
}