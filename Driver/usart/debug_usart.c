
#include "debug_usart.h"
#include <stdio.h>
#include "stdbool.h"
#include "FreeRtos.h"
#include "task.h"

FlagStatus g_transfer_complete;

__attribute__ ((aligned(32))) uint8_t rxbuffer[DEBUG_RX_BUFFER_SIZE];
__attribute__ ((aligned(32))) uint8_t txbuffer[DEBUG_TX_BUFFER_SIZE];

#define ARRAYNUM(arr_nanme)       (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define USART1_TDATA_ADDRESS      (&USART_TDATA(USART1))
#define USART1_RDATA_ADDRESS      (&USART_RDATA(USART1))

uint8_t readcount = 0;


/**
 * @brief 串口 初始化。
 * 设置接收寄存器非空中断：用于启动接收超时中断。
 * 设置接收超时中断，用于获取本次dma接收的数据长度
 * 
 */
void uart1_init(void)
{
    rcu_periph_clock_enable(DEBUG_USART_GPIO_CLK_TX);
    rcu_periph_clock_enable(DEBUG_USART_GPIO_CLK_RX);
    rcu_periph_clock_enable(DEBUG_USART_CLK);

    gpio_af_set(DEBUG_USART_TX_GPIO_PORT, DEBUG_USART_TX_RX_GPIO_AF, DEBUG_USART_TX_GPIO_PIN);
    gpio_mode_set(DEBUG_USART_TX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, DEBUG_USART_TX_GPIO_PIN);
    gpio_output_options_set(DEBUG_USART_TX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, DEBUG_USART_TX_GPIO_PIN);

    gpio_af_set(DEBUG_USART_RX_GPIO_PORT, DEBUG_USART_TX_RX_GPIO_AF, DEBUG_USART_RX_GPIO_PIN);
    gpio_mode_set(DEBUG_USART_RX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, DEBUG_USART_RX_GPIO_PIN);
    gpio_output_options_set(DEBUG_USART_RX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, DEBUG_USART_RX_GPIO_PIN);

    usart_deinit(DEBUG_USARTX);

    usart_baudrate_set(DEBUG_USARTX, DEBUG_USART_BAUDRATE);
    usart_word_length_set(DEBUG_USARTX, USART_WL_8BIT);
    usart_stop_bit_set(DEBUG_USARTX, USART_STB_1BIT);
    usart_parity_config(DEBUG_USARTX, USART_PM_NONE);

    usart_hardware_flow_rts_config(DEBUG_USARTX, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(DEBUG_USARTX, USART_CTS_DISABLE);

    usart_receive_config(DEBUG_USARTX, USART_RECEIVE_ENABLE);
    usart_transmit_config(DEBUG_USARTX, USART_TRANSMIT_ENABLE);

    usart_interrupt_enable(DEBUG_USARTX, USART_INT_RBNE);
    usart_receiver_timeout_enable(DEBUG_USARTX);
    usart_receiver_timeout_threshold_config(DEBUG_USARTX, 100);

    nvic_irq_enable(DEBUG_USART_IRQ, 1U, 1U);

    usart_enable(DEBUG_USARTX);

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
    dma_init_struct.memory0_addr  = (uint32_t)txbuffer;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_struct.number       = ARRAYNUM(txbuffer);
    dma_init_struct.periph_addr  = (uint32_t)USART1_TDATA_ADDRESS;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA0, DMA_CH0, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH0);
    /* enable DMA channel 0 */
    dma_channel_enable(DMA0, DMA_CH0);

    /* USART DMA enable for transmission */
    usart_dma_transmit_config(USART1, USART_TRANSMIT_DMA_ENABLE);

     /* wait DMA Channel transfer complete */
    while(RESET == dma_flag_get(DMA0, DMA_CH0, DMA_FLAG_FTF)) {
    }
}


void uart1_rx_dma_config(void) {

    dma_single_data_parameter_struct dma_init_struct;
    dma_deinit(DMA0, DMA_CH1);

    dma_single_data_para_struct_init(&dma_init_struct);
    dma_init_struct.request      = DMA_REQUEST_USART1_RX;
    dma_init_struct.direction    = DMA_PERIPH_TO_MEMORY;
    dma_init_struct.memory0_addr  = (uint32_t)rxbuffer;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_struct.number       = ARRAYNUM(rxbuffer);
        dma_init_struct.periph_addr  = (uint32_t)USART1_RDATA_ADDRESS;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA0, DMA_CH1, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH1);
    /* enable DMA channel 1 */
    dma_channel_enable(DMA0, DMA_CH1);
    /* USART DMA enable for reception */
        usart_dma_receive_config(USART1, USART_RECEIVE_DMA_ENABLE);

}

void debug_init(void)
{
    uart1_init();
    uart1_tx_dma_config();
}

void cli_rx(void* pvParameters)
{
    while(1) {
      vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void debug_tx(void* pvParameters)
{
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void debug_task(void* pvParameters)
{
    debug_init();
    while(1) {
        uart1_rx_dma_config();
        /* wait DMA channel 1 transfer complete */
        while((RESET == dma_flag_get(DMA0, DMA_CH1, DMA_FLAG_FTF)) & (readcount < 7)){
            vTaskDelay(pdMS_TO_TICKS(*((uint32_t*)pvParameters)));
        }
        readcount = 0;
        /* invalidate d-cache by address */
        SCB_InvalidateDCache_by_Addr((uint32_t*)rxbuffer, 32);
        printf("\n\r%s\n\r", rxbuffer);
    }
}

/**
 * @brief  重定向printf输出到串口
 */
int fputc(int ch, FILE *f) 
{
    usart_data_transmit(DEBUG_USARTX, (uint8_t)ch);

    while (RESET == usart_flag_get(DEBUG_USARTX, USART_FLAG_TBE))
        ;
    return ch;
}

/**
 * @brief  重定向scanf输入到串口
 */
int fgetc(FILE *f) 
{
    while (RESET == usart_flag_get(DEBUG_USARTX, USART_FLAG_RBNE))
        ;
    return (int)usart_data_receive(DEBUG_USARTX);
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
        g_transfer_complete = SET;
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
        g_transfer_complete = SET;
    }
}



void USART1_IRQHandler(void)
{
    //如果接收超时中断没有启用
    if(!(USART_CTL0(DEBUG_USARTX) & USART_CTL0_RTIE))
    {
        //启用接收超时中断
         usart_interrupt_enable(DEBUG_USARTX, USART_INT_RT);
        //关闭接收数据非空中断
        usart_interrupt_disable(DEBUG_USARTX, USART_INT_RBNE);
    }
    
    // 检查是否为接收超时中断
    if(usart_interrupt_flag_get(DEBUG_USARTX, USART_INT_FLAG_RT) == SET) {
        // 清除接收超时中断标志
        usart_interrupt_flag_clear(DEBUG_USARTX, USART_INT_FLAG_RT);
        
        //获取dma0 channel 1 的cnt寄存器的值

        readcount = dma_transfer_number_get(DMA0, DMA_CH1);
        readcount = ARRAYNUM(rxbuffer) - readcount;
        // 可选：暂时禁用接收超时，直到下一个数据包开始
        usart_interrupt_disable(DEBUG_USARTX, USART_INT_RT);
        // 启用接收数据非空中断
        usart_interrupt_enable(DEBUG_USARTX, USART_INT_RBNE);
    }
}