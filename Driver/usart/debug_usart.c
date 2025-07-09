
#include "debug_usart.h"
#include <stdio.h>
#include "stdbool.h"
#include "FreeRtos.h"
#include "task.h"

#include "utilities/macro.h"


__attribute__ ((aligned(32))) uint8_t rx_buffer[CONSOLE_DMA_BUFFER_RX_SIZE];
__attribute__ ((aligned(32))) uint8_t tx_buffer[CONSOLE_DMA_BUFFER_TX_SIZE];


RING_BUFF_PRE_INIT(console_rx, CONSOLE_RING_BUFFER_RX_SIZE);
RING_BUFF_PRE_INIT(console_tx, CONSOLE_RING_BUFFER_TX_SIZE);

console_t console = {0};

/**
 * @brief 串口 初始化。
 * 设置接收寄存器非空中断：用于启动接收超时中断。
 * 设置接收超时中断，用于获取本次dma接收的数据长度
 * 
 */
void uart1_init(void)
{
    rcu_periph_clock_enable(CONSOLE_USART_GPIO_CLK_TX);
    rcu_periph_clock_enable(CONSOLE_USART_GPIO_CLK_RX);
    rcu_periph_clock_enable(CONSOLE_USART_CLK);

    gpio_af_set(CONSOLE_USART_TX_GPIO_PORT, CONSOLE_USART_TX_RX_GPIO_AF, CONSOLE_USART_TX_GPIO_PIN);
    gpio_mode_set(CONSOLE_USART_TX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, CONSOLE_USART_TX_GPIO_PIN);
    gpio_output_options_set(CONSOLE_USART_TX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, CONSOLE_USART_TX_GPIO_PIN);

    gpio_af_set(CONSOLE_USART_RX_GPIO_PORT, CONSOLE_USART_TX_RX_GPIO_AF, CONSOLE_USART_RX_GPIO_PIN);
    gpio_mode_set(CONSOLE_USART_RX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, CONSOLE_USART_RX_GPIO_PIN);
    gpio_output_options_set(CONSOLE_USART_RX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, CONSOLE_USART_RX_GPIO_PIN);

    usart_deinit(CONSOLE_USARTX);

    usart_baudrate_set(CONSOLE_USARTX, CONSOLE_USART_BAUDRATE);
    usart_word_length_set(CONSOLE_USARTX, USART_WL_8BIT);
    usart_stop_bit_set(CONSOLE_USARTX, USART_STB_1BIT);
    usart_parity_config(CONSOLE_USARTX, USART_PM_NONE);

    usart_hardware_flow_rts_config(CONSOLE_USARTX, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(CONSOLE_USARTX, USART_CTS_DISABLE);

    usart_receive_config(CONSOLE_USARTX, USART_RECEIVE_ENABLE);
    usart_transmit_config(CONSOLE_USARTX, USART_TRANSMIT_ENABLE);

    usart_interrupt_enable(CONSOLE_USARTX, USART_INT_RBNE);
    usart_receiver_timeout_enable(CONSOLE_USARTX);
    usart_receiver_timeout_threshold_config(CONSOLE_USARTX, 100);

    nvic_irq_enable(CONSOLE_USART_IRQ, 1U, 1U);

    usart_enable(CONSOLE_USARTX);

}

void uart1_tx_dma_config(void){
    dma_single_data_parameter_struct dma_init_struct;
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA0);
    /* enable DMAMUX clock */
    rcu_periph_clock_enable(RCU_DMAMUX);

    /* initialize DMA channel 0 */
    dma_deinit(DMA0, DMA_CH0);
    dma_single_data_para_struct_init(&dma_init_struct);
    dma_init_struct.request      = DMA_REQUEST_USART1_TX;
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPH;
    dma_init_struct.memory0_addr  = (uint32_t)tx_buffer;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_struct.number       = console.tx_dma_buffer.size;
    dma_init_struct.periph_addr  = (uint32_t)CONSOLE_TDATA_ADDRESS;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA0, DMA_CH0, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH0);
    /* enable DMA channel 0 */
    dma_channel_enable(DMA0, DMA_CH0);

    /* USART DMA enable for transmission */
    usart_dma_transmit_config(USART1, USART_TRANSMIT_DMA_ENABLE);

    /* 启用 DMA 传输完成中断 */
    dma_interrupt_enable(DMA0, DMA_CH0, DMA_CHXCTL_FTFIE);
    nvic_irq_enable(DMA0_Channel0_IRQn, 2, 0);
}


void uart1_rx_dma_config(void) {

    dma_single_data_parameter_struct dma_init_struct;
    dma_deinit(DMA0, DMA_CH1);

    dma_single_data_para_struct_init(&dma_init_struct);
    dma_init_struct.request      = DMA_REQUEST_USART1_RX;
    dma_init_struct.direction    = DMA_PERIPH_TO_MEMORY;
    dma_init_struct.memory0_addr  = (uint32_t)rx_buffer;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_struct.number       = ARRAYNUM(rx_buffer);
    dma_init_struct.periph_addr  = (uint32_t)CONSOLE_RDATA_ADDRESS;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA0, DMA_CH1, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH1);
    /* enable DMA channel 1 */
    dma_channel_enable(DMA0, DMA_CH1);
    /* USART DMA enable for reception */
    usart_dma_receive_config(USART1, USART_RECEIVE_DMA_ENABLE);

    /* 启用 DMA 接收完成中断 */
    dma_interrupt_enable(DMA0, DMA_CH1, DMA_INT_FTF);
    nvic_irq_enable(DMA0_Channel1_IRQn, 2, 0);
}

void console_init(void)
{
    console.rx_ring_buffer = console_rx_ring_buff();
    console.tx_ring_buffer = console_tx_ring_buff();
    console.rx_dma_buffer.buffer = rx_buffer;
    console.tx_dma_buffer.buffer = tx_buffer;
    console.bReady = true;
    uart1_init();
    uart1_tx_dma_config();
    uart1_rx_dma_config();
}

void console_rx(void* pvParameters)
{
    printf("console rx task 创建成功\n\r");
    while(1) {        
        if(console.rx_dma_buffer.size != 0)
        {
            //接收到数据，读取，并重置DMA
            int ret = console.rx_ring_buffer->write(console.rx_dma_buffer.buffer, console.rx_dma_buffer.size, 100);
            
            if(ret > 0)
            {
                console.rx_dma_buffer.size = 0;
            }
            if(ret == -1)
            {
                printf("console rx ring buffer full\n\r");
            }else if(ret == -2)
            {
                printf("console rx ring buffer write timeout\n\r");
            }
            uart1_rx_dma_config();
            SCB_InvalidateDCache_by_Addr((uint32_t*)console.rx_dma_buffer.buffer, CONSOLE_DMA_BUFFER_RX_SIZE);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void console_tx(void* pvParameters)
{
    printf("console tx task 创建成功\n\r");
    while(1) {
        if(!console.tx_ring_buffer->is_empty())
        {
            if(console.tx_dma_buffer.size == 0)
            {
                int ret = console.tx_ring_buffer->read(console.tx_dma_buffer.buffer, CONSOLE_DMA_BUFFER_TX_SIZE, 100);
                if(ret > 0){
                    console.tx_dma_buffer.size = ret;
                    // 确保 DMA 能看到最新数据
                    SCB_CleanDCache_by_Addr((uint32_t*)console.tx_dma_buffer.buffer, ret);
                    uart1_tx_dma_config();
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void console_test(void* pvParameters)
{
    printf("console test task 创建成功\n\r");
    uint8_t data[CONSOLE_RING_BUFFER_RX_SIZE] = {0};
    while(1) {
        int ret = console.rx_ring_buffer->read(data, CONSOLE_RING_BUFFER_RX_SIZE, 100);
        if(ret == -1)
        {
            printf("console rx ring buffer read timeout\n\r");
        }else if(ret == 0)
        {
            //printf("console rx ring buffer empty\n\r");
        }else if(ret > 0)
        {
            console.tx_ring_buffer->write(data, ret, 100);
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("console test task running\n\r");
    }
}

void console_task(void* pvParameters)
{
    console_init();
    xTaskCreate(console_rx, "console_rx", 1024, NULL, 5, NULL);
    xTaskCreate(console_tx, "console_tx", 1024, NULL, 5, NULL);
    xTaskCreate(console_test, "console_test", 1024, NULL, 5, NULL);

    vTaskDelete( NULL );
}

/**
 * @brief  重定向printf输出到串口
 */
int fputc(int ch, FILE *f) 
{
    if(console.bReady)
    {
        console.tx_ring_buffer->write((uint8_t*)&ch, 1, 100);
    }
    else
    {
        usart_data_transmit(CONSOLE_USARTX, (uint8_t)ch);
        while (RESET == usart_flag_get(CONSOLE_USARTX, USART_FLAG_TBE))
            ;
    }
    return ch;
}

/**
 * @brief  重定向scanf输入到串口
 */
int fgetc(FILE *f) 
{   
    while (RESET == usart_flag_get(CONSOLE_USARTX, USART_FLAG_RBNE))
        ;
    return (int)usart_data_receive(CONSOLE_USARTX);
}

/*!
    \brief      this function handles DMA_Channel0_IRQHandler exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DMA0_Channel0_IRQHandler(void)
{
    if(RESET != dma_interrupt_flag_get(DMA0, DMA_CH0, DMA_INT_FLAG_FTF)) {
        dma_interrupt_flag_clear(DMA0, DMA_CH0, DMA_INT_FLAG_FTF);
        console.tx_dma_buffer.size = 0;
    }
}

/*!
    \brief      this function handles DMA_Channel1_IRQHandler exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DMA0_Channel1_IRQHandler(void)
{
    if(RESET != dma_interrupt_flag_get(DMA0, DMA_CH1, DMA_INT_FLAG_FTF)) {
        dma_interrupt_flag_clear(DMA0, DMA_CH1, DMA_INT_FLAG_FTF);
    }
}



void CONSOLE_USART_IRQ_HANDLER(void)
{
    //如果接收超时中断没有启用
    if(!(USART_CTL0(CONSOLE_USARTX) & USART_CTL0_RTIE))
    {
        //启用接收超时中断
         usart_interrupt_enable(CONSOLE_USARTX, USART_INT_RT);
        //关闭接收数据非空中断
        usart_interrupt_disable(CONSOLE_USARTX, USART_INT_RBNE);
    }
    
    // 检查是否为接收超时中断
    if(usart_interrupt_flag_get(CONSOLE_USARTX, USART_INT_FLAG_RT) == SET) {
        // 清除接收超时中断标志
        usart_interrupt_flag_clear(CONSOLE_USARTX, USART_INT_FLAG_RT);
        
        //获取dma0 channel 1 的cnt寄存器的值

        console.rx_dma_buffer.size = CONSOLE_DMA_BUFFER_RX_SIZE - dma_transfer_number_get(DMA0, DMA_CH1);
        // 可选：暂时禁用接收超时，直到下一个数据包开始
        usart_interrupt_disable(CONSOLE_USARTX, USART_INT_RT);
        // 启用接收数据非空中断
        usart_interrupt_enable(CONSOLE_USARTX, USART_INT_RBNE);
    }
}
