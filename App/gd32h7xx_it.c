#include "gd32h7xx.h"
#include "gd32h7xx_it.h"
#include "main.h"
#include "semphr.h"
#include "queue.h"
#include "lwip/sys.h"

extern xSemaphoreHandle g_rx_semaphore;

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
    /* if NMI exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

///*!
//    \brief      this function handles SVC exception
//    \param[in]  none
//    \param[out] none
//    \retval     none
//*/
//void SVC_Handler(void)
//{
//    /* if SVC exception occurs, go to infinite loop */
//    while(1) {
//    }
//}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
    /* if DebugMon exception occurs, go to infinite loop */
    while(1) {
    }
}

///*!
//    \brief      this function handles PendSV exception
//    \param[in]  none
//    \param[out] none
//    \retval     none
//*/
//void PendSV_Handler(void)
//{
//    /* if PendSV exception occurs, go to infinite loop */
//    while(1) {
//    }
//}



/*!
    \brief      this function handles EXTI10_15 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/


#ifdef USE_ENET0
/*!
    \brief      this function handles ethernet interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ENET0_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    /* frame received */
    if(SET == enet_interrupt_flag_get(ENET0, ENET_DMA_INT_FLAG_RS)){
        /* give the semaphore to wakeup LwIP task */
        xSemaphoreGiveFromISR(g_rx_semaphore, &xHigherPriorityTaskWoken);
    }

    /* clear the enet DMA Rx interrupt pending bits */
    enet_interrupt_flag_clear(ENET0, ENET_DMA_INT_FLAG_RS_CLR);
    enet_interrupt_flag_clear(ENET0, ENET_DMA_INT_FLAG_NI_CLR);

    /* switch tasks if necessary */
    if(pdFALSE != xHigherPriorityTaskWoken){
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    }
}
#endif /* USE_ENET0 */
#ifdef USE_ENET1
/*!
    \brief      this function handles ethernet interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ENET1_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    /* frame received */
    if(SET == enet_interrupt_flag_get(ENET1, ENET_DMA_INT_FLAG_RS)){
        /* give the semaphore to wakeup LwIP task */
        xSemaphoreGiveFromISR(g_rx_semaphore, &xHigherPriorityTaskWoken);
    }

    /* clear the enet DMA Rx interrupt pending bits */
    enet_interrupt_flag_clear(ENET1, ENET_DMA_INT_FLAG_RS_CLR);
    enet_interrupt_flag_clear(ENET1, ENET_DMA_INT_FLAG_NI_CLR);

    /* switch tasks if necessary */
    if(pdFALSE != xHigherPriorityTaskWoken){
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    }
}
#endif /* USE_ENET1 */
