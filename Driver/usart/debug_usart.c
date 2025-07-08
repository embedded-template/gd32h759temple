
#include "debug_usart.h"


void DEBUG_USART_Config(void)
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

    //nvic_irq_enable(DEBUG_USART_IRQ, 1U, 1U);

    usart_enable(DEBUG_USARTX);

    //usart_interrupt_enable(DEBUG_USARTX, USART_INT_RBNE);
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
