#include "main.h"
#include "cli.h"
#include "log.h"
#include "usart/console_usart.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "can/module_can.h"
#include "gd32h7xx.h"
#include "tcp_client.h"

task_info_t task_info_all[] = {
    {CLI_Task, NAME_cli_task, STACK_cli_task, PARAM_cli_task, PRIORITY_cli_task, NULL},
    {tcp_client_task, NAME_tcp_client_task, STACK_tcp_client_task, PARAM_tcp_client_task, PRIORITY_tcp_client_task, NULL},
};

#define TASK_NUM (sizeof(task_info_all) / sizeof(task_info_t))


void cache_enable(void)
{
    /* enable I-Cache */
    SCB_EnableICache();
    /* enable D-Cache */
    SCB_EnableDCache();
}

void check_reset_source(void)
{
    if (rcu_flag_get(RCU_FLAG_BORRST)) {
        Log_info("BOR reset");
    }
    if (rcu_flag_get(RCU_FLAG_EPRST)) {
        Log_info("External reset");
    }
    if (rcu_flag_get(RCU_FLAG_PORRST)) {
        Log_info("Power-on reset");
    }
    if (rcu_flag_get(RCU_FLAG_SWRST)) {
        Log_info("Software reset");
    }
    if (rcu_flag_get(RCU_FLAG_FWDGTRST)) {
        Log_info("free watchdog timer reset flag");
    }
    if (rcu_flag_get(RCU_FLAG_WWDGTRST)) {
        Log_info("Watchdog reset");
    }
    if (rcu_flag_get(RCU_FLAG_LPRST)) {
        Log_info("Low-power reset");
    }

    rcu_all_reset_flag_clear();
}

void mpu_config(void)
{
    mpu_region_init_struct mpu_init_struct;
    mpu_region_struct_para_init(&mpu_init_struct);

    /* disable the MPU */
    ARM_MPU_SetRegion(0U, 0U);

    /* Configure the DMA descriptors and Rx/Tx buffer*/
    mpu_init_struct.region_base_address = 0x30000000;
    mpu_init_struct.region_size = MPU_REGION_SIZE_16KB;
    mpu_init_struct.access_permission = MPU_AP_FULL_ACCESS;
    mpu_init_struct.access_bufferable = MPU_ACCESS_BUFFERABLE;
    mpu_init_struct.access_cacheable = MPU_ACCESS_NON_CACHEABLE;
    mpu_init_struct.access_shareable = MPU_ACCESS_NON_SHAREABLE;
    mpu_init_struct.region_number = MPU_REGION_NUMBER0;
    mpu_init_struct.subregion_disable = MPU_SUBREGION_ENABLE;
    mpu_init_struct.instruction_exec = MPU_INSTRUCTION_EXEC_PERMIT;
    mpu_init_struct.tex_type = MPU_TEX_TYPE0;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();

    /* Configure the LwIP RAM heap */
    mpu_init_struct.region_base_address = 0x30004000;
    mpu_init_struct.region_size = MPU_REGION_SIZE_16KB;
    mpu_init_struct.access_permission = MPU_AP_FULL_ACCESS;
    mpu_init_struct.access_bufferable = MPU_ACCESS_NON_BUFFERABLE;
    mpu_init_struct.access_cacheable = MPU_ACCESS_NON_CACHEABLE;
    mpu_init_struct.access_shareable = MPU_ACCESS_SHAREABLE;
    mpu_init_struct.region_number = MPU_REGION_NUMBER1;
    mpu_init_struct.subregion_disable = MPU_SUBREGION_ENABLE;
    mpu_init_struct.instruction_exec = MPU_INSTRUCTION_EXEC_PERMIT;
    mpu_init_struct.tex_type = MPU_TEX_TYPE1;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();

    /* enable the MPU */
    ARM_MPU_Enable(MPU_MODE_PRIV_DEFAULT);
}

int main(void)
{
    uint8_t ucaRxBuf[256];
    uint16_t usRxCount = 0;

    //nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    SCB_EnableICache();     // 使能 I-Cache
    SCB_EnableDCache();     // 使能 D-Cache

    /* configure the MPU */
    mpu_config();
    //cache_enable();
    console_init();
    module_can_init();
    check_reset_source();

    for (int i = 0; i < TASK_NUM; i++)
    {
        xTaskCreate(task_info_all[i].pxTaskCode, task_info_all[i].pcName, task_info_all[i].uxStackDepth, &task_info_all[i].time, task_info_all[i].uxPriority,
                    task_info_all[i].pxCreatedTask);
    }
    Log_info("系统初始化完成");
    vTaskStartScheduler();
    while (1);
}

#if (configCHECK_FOR_STACK_OVERFLOW)
/**
 * @brief 栈溢出钩子函数。输出栈溢出任务信息。
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName)
{
    /* 全局变量用于记录栈溢出任务信息 */
    volatile TaskHandle_t g_overflow_task_handle = NULL;
    volatile char g_overflow_task_name[configMAX_TASK_NAME_LEN] = {0};
    volatile uint32_t g_stack_overflow_count = 0;

    /* 记录出错任务的句柄 */
    g_overflow_task_handle = xTask;

    /* 记录出错任务的名称 */
    if (pcTaskName != NULL)
    {
        strncpy((char*) g_overflow_task_name, pcTaskName, configMAX_TASK_NAME_LEN - 1);
        g_overflow_task_name[configMAX_TASK_NAME_LEN - 1] = '\0'; /* 确保字符串结束 */
    }
    else
    {
        strcpy((char*) g_overflow_task_name, "Unknown");
    }

    /* 增加栈溢出计数 */
    g_stack_overflow_count++;

    /* 输出错误信息 */
    uart_handle_t* console = get_console();
    console->bReady = false;
    printf("任务:%s stack 溢出, task handle: 0x%08lX, Count: %d\r\n", (char*) g_overflow_task_name, (unsigned long) g_overflow_task_handle,
           g_stack_overflow_count);

    /* 进入死循环，防止系统继续运行 */
    for (;;)
    {
        /* 可以在这里添加LED闪烁等指示代码 */
    }
}
#endif
