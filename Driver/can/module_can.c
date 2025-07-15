#include "module_can.h"
#include "stdio.h"


/** @brief CAN中断接收缓冲区，32字节对齐 */
__attribute__((aligned(32))) uint32_t module_rx_buffer[4];

/** @brief 环形缓冲区预初始化 */
RING_BUFF_PRE_INIT(module_rx, MODULE_RING_BUFFER_RX_SIZE);
RING_BUFF_PRE_INIT(module_tx, MODULE_RING_BUFFER_TX_SIZE);

/** @brief CAN模块句柄 */
can_handle_t module_can_handle = {0};


static void module_can_hw_init(void);
static void module_can_gpio_config(void);
static void module_can_parameter_config(void);
static void module_can_fifo_config(void);
static void module_tx_task(void* pvParameters);
static void module_rx_task(void* pvParameters);

#ifdef MODULE_CAN_TEST
#include "log.h"
static void module_can_test_task(void* pvParameters);
#endif

/**
 * @brief 获取CAN模块句柄
 * @return CAN模块句柄指针
 */
can_handle_t* module_can_handle_get(void)
{
    return &module_can_handle;
}

/**
 * @brief CAN模块初始化
 * @details 初始化CAN模块句柄、环形缓冲区、队列和硬件
 */
void module_can_init(void)
{
    /* 初始化模块句柄 */
    module_can_handle.bReady = true;
    module_can_handle.rx_ring_buffer = module_rx_ring_buff();
    module_can_handle.tx_ring_buffer = module_tx_ring_buff();
    module_can_handle.rx_queue = xQueueCreate(10, sizeof(can_rx_fifo_struct));

    /* 硬件初始化 */
    module_can_hw_init();

    /* 创建CAN任务 */
    xTaskCreate(module_tx_task, "module_tx", 100, NULL, 5, NULL);
    xTaskCreate(module_rx_task, "module_rx", 100, NULL, 5, NULL);
    #ifdef MODULE_CAN_TEST
    xTaskCreate(module_can_test_task, "can_test", 200, NULL, 3, NULL);
    #endif
}



/**
 * @brief CAN硬件初始化
 * @details 配置时钟、GPIO、CAN参数、FIFO和中断
 */
static void module_can_hw_init(void)
{
    /* 时钟配置 */
    rcu_can_clock_config(MODULE_CAN_IDX, MODULE_CAN_CLOCK_SOURCE);
    rcu_periph_clock_enable(MODULE_CAN_RCU);
    rcu_periph_clock_enable(MODULE_CAN_GPIO_RCU);

    /* GPIO配置 */
    module_can_gpio_config();

    /* CAN参数配置 */
    module_can_parameter_config();

    /* FIFO配置 */
    module_can_fifo_config();

    /* 中断配置 */
    nvic_irq_enable(MODULE_CAN_IRQn, MODULE_CAN_IRQ_PRIORITY, MODULE_CAN_IRQ_SUB_PRIORITY);
    can_interrupt_enable(MODULE_CAN_PERIPH, CAN_INT_FIFO_AVAILABLE);

    /* 进入正常模式 */
    can_operation_mode_enter(MODULE_CAN_PERIPH, CAN_LOOPBACK_SILENT_MODE);
}

/**
 * @brief CAN GPIO配置
 * @details 配置CAN的RX和TX引脚
 */
static void module_can_gpio_config(void)
{
    /* 配置RX引脚 */
    gpio_output_options_set(MODULE_CAN_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, MODULE_CAN_RX_PIN);
    gpio_mode_set(MODULE_CAN_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, MODULE_CAN_RX_PIN);
    gpio_af_set(MODULE_CAN_GPIO_PORT, MODULE_CAN_GPIO_AF, MODULE_CAN_RX_PIN);

    /* 配置TX引脚 */
    gpio_output_options_set(MODULE_CAN_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, MODULE_CAN_TX_PIN);
    gpio_mode_set(MODULE_CAN_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, MODULE_CAN_TX_PIN);
    gpio_af_set(MODULE_CAN_GPIO_PORT, MODULE_CAN_GPIO_AF, MODULE_CAN_TX_PIN);
}

/**
 * @brief CAN参数配置
 * @details 配置CAN控制器参数，包括波特率、工作模式等
 */
static void module_can_parameter_config(void)
{
    can_parameter_struct can_parameter;

    /* 复位CAN寄存器 */
    can_deinit(MODULE_CAN_PERIPH);

    /* 初始化CAN参数结构体 */
    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);

    /* 基本参数配置 */
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
    can_parameter.memory_size = CAN_MEMSIZE_32_UNIT;

    /* 过滤器配置 */
    can_parameter.mb_public_filter = 0U;

    /* 波特率配置 (125kbps, 采样点80%) */
    can_parameter.resync_jump_width = MODULE_CAN_RESYNC_JUMP_WIDTH;
    can_parameter.prop_time_segment = MODULE_CAN_PROP_TIME_SEGMENT;
    can_parameter.time_segment_1 = MODULE_CAN_TIME_SEGMENT_1;
    can_parameter.time_segment_2 = MODULE_CAN_TIME_SEGMENT_2;
    can_parameter.prescaler = MODULE_CAN_PRESCALER;

    /* 应用配置 */
    can_init(MODULE_CAN_PERIPH, &can_parameter);
}

/**
 * @brief CAN FIFO配置
 * @details 配置接收FIFO和清空发送邮箱
 */
static void module_can_fifo_config(void)
{
    can_fifo_parameter_struct can_fifo_parameter;

    /* 进入非活动模式进行配置 */
    can_operation_mode_enter(MODULE_CAN_PERIPH, CAN_INACTIVE_MODE);

    /* 初始化FIFO参数结构体 */
    can_struct_para_init(CAN_FIFO_INIT_STRUCT, &can_fifo_parameter);

    /* FIFO参数配置 */
    can_fifo_parameter.dma_enable = DISABLE;
    can_fifo_parameter.filter_format_and_number = CAN_RXFIFO_FILTER_A_NUM_8; /* A类过滤器，8个 */
    can_fifo_parameter.fifo_public_filter = 0U;                              /* 暂不设置过滤 */

    /* 应用FIFO配置 */
    can_rx_fifo_config(MODULE_CAN_PERIPH, &can_fifo_parameter);

    /* 清空FIFO和发送邮箱 */
    can_rx_fifo_clear(MODULE_CAN_PERIPH);
    can_flag_clear(MODULE_CAN_PERIPH, MODULE_CAN_TX_MAILBOX_FLAG);
}


/**
 * @brief CAN发送任务
 * @details 从发送环形缓冲区中读取数据并通过邮箱8发送
 * @param pvParameters 任务参数（未使用）
 */
void module_tx_task(void* pvParameters)
{
    can_mailbox_descriptor_struct mdpara;
    uint32_t data[3] = {0};

    /* 避免编译器警告 */
    (void)pvParameters;

    while (1)
    {
        while (!module_can_handle.tx_ring_buffer->is_empty())
        {
            /* 初始化邮箱描述符 */
            can_struct_para_init(CAN_MDSC_STRUCT, &mdpara);

            /* 从发送缓冲区读取数据 */
            if (module_can_handle.tx_ring_buffer->read((uint8_t*)&data, sizeof(data), MODULE_CAN_TIMEOUT_MS) > 0)
            {
                /* 配置邮箱参数 */
                mdpara.ide = 1U;                        /* 扩展帧 */
                mdpara.rtr = 0U;                        /* 数据帧 */
                mdpara.esi = 0U;                        /* 错误状态指示器 */
                mdpara.code = CAN_MB_TX_STATUS_DATA;    /* 发送数据状态 */
                mdpara.data_bytes = MODULE_CAN_DATA_BYTES;
                mdpara.id = data[0] & 0x1FFFFFFF;       /* CAN ID (确保在29位范围内) */
                mdpara.data = &data[1];                 /* 数据指针 */

                /* 发送数据 */
                can_mailbox_config(MODULE_CAN_PERIPH, MODULE_CAN_TX_MAILBOX, &mdpara);

                /* 等待发送完成 */
                while (can_flag_get(MODULE_CAN_PERIPH, MODULE_CAN_TX_MAILBOX_FLAG) == RESET)
                {
                    vTaskDelay(pdMS_TO_TICKS(1));
                }

                /* 清除发送完成标志 */
                can_flag_clear(MODULE_CAN_PERIPH, MODULE_CAN_TX_MAILBOX_FLAG);
            }
        }

        /* 任务延时，避免占用过多CPU */
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief CAN接收任务
 * @details 从队列中读取FIFO数据并写入接收环形缓冲区，供应用层使用
 * @param pvParameters 任务参数（未使用）
 */
void module_rx_task(void* pvParameters)
{
    can_rx_fifo_struct rx_fifo;
    uint32_t data[3] = {0};
    /* 避免编译器警告 */
    (void)pvParameters;

    while (1)
    {
        /* 从队列中接收数据，带超时 */
        while (xQueueReceive(module_can_handle.rx_queue, &rx_fifo, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            /* 只保留id和data */
            data[0] = rx_fifo.id;
            data[1] = rx_fifo.data[0];
            data[2] = rx_fifo.data[1];

            /* 写入接收环形缓冲区 */
            module_can_handle.rx_ring_buffer->write((uint8_t*)data, sizeof(data), MODULE_CAN_TIMEOUT_MS);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/**
 * @brief CAN中断处理函数
 * @details FIFO中有数据时触发，将数据通过队列发送给接收任务
 */
void MODULE_CAN_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (can_interrupt_flag_get(MODULE_CAN_PERIPH, CAN_INT_FLAG_FIFO_AVAILABLE) == SET)
    {
        /* 读取FIFO数据 */
        can_rx_fifo_read(MODULE_CAN_PERIPH, (can_rx_fifo_struct*)module_rx_buffer);

        /* 发送到队列 */
        xQueueSendFromISR(module_can_handle.rx_queue, (void*)module_rx_buffer, &xHigherPriorityTaskWoken);

        /* 清除中断标志 */
        can_interrupt_flag_clear(MODULE_CAN_PERIPH, CAN_INT_FLAG_FIFO_AVAILABLE);
    }

    /* 如果有更高优先级任务被唤醒，则进行任务切换 */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


#ifdef MODULE_CAN_TEST
/**
 * @brief CAN测试任务
 * @details 定期发送测试数据并接收数据进行验证
 * @param pvParameters 任务参数（未使用）
 */
static void module_can_test_task(void* pvParameters)
{
    uint32_t tx_data[3] = {0};
    uint32_t rx_data[3] = {0};
    uint32_t test_counter = 0;
    uint32_t tx_count = 0;
    uint32_t rx_count = 0;
    uint32_t error_count = 0;

    /* 避免编译器警告 */
    (void)pvParameters;

    /* 等待CAN模块初始化完成 */
    vTaskDelay(pdMS_TO_TICKS(1000));

    while (1)
    {
        /* 准备发送数据 */
        tx_data[0] = 0x123;  /* CAN ID */
        tx_data[1] = 0x12345678 ;       /* 数据1 */
        tx_data[2] = 0x87654321 ;       /* 数据2 */

        /* 发送数据到发送环形缓冲区 */
        if (module_can_handle.tx_ring_buffer->write((uint8_t*)tx_data, sizeof(tx_data), MODULE_CAN_TIMEOUT_MS) > 0)
        {
            tx_count++;
        }

        /* 尝试接收数据 */
        while (module_can_handle.rx_ring_buffer->read((uint8_t*)rx_data, sizeof(rx_data), 10) > 0)
        {
            rx_count++;

            /* 简单的数据验证（在回环模式下，发送的数据应该能收到） */
            if ((rx_data[0] != 0x123) ||
                (rx_data[1] != 0x12345678) ||
                (rx_data[2] != 0x87654321))
            {
                error_count++;
            }
        }

        test_counter++;

        Log_debug("CAN Test: TX=%d, RX=%d, ERR=%d\n", tx_count, rx_count, error_count);

        /* 任务延时500ms */
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

#endif
