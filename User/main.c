#include "main.h"
#include "gd32h7xx.h"
#include "usart/console_usart.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "log.h"
#include "cli.h"

task_info_t task_info_all[] = {{console_task, NAME_console_task, STACK_console_task,
                                PARAM_console_task, PRIORITY_console_task, NULL},
                                {CLI_Task, NAME_cli_task, STACK_cli_task,
                                PARAM_cli_task, PRIORITY_cli_task, NULL}};

#define TASK_NUM (sizeof(task_info_all) / sizeof(task_info_t))

void vTask1(void* pvParameters)
{
    while (1)
    {
        printf("Task1\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(void)
{
    uint8_t ucaRxBuf[256];
    uint16_t usRxCount = 0;

    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);

		console_init();

    for (int i = 0; i < TASK_NUM; i++)
    {
        xTaskCreate(task_info_all[i].pxTaskCode, task_info_all[i].pcName,
                    task_info_all[i].uxStackDepth, &task_info_all[i].time,
                    task_info_all[i].uxPriority,
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
        strncpy((char*) g_overflow_task_name, pcTaskName,
                configMAX_TASK_NAME_LEN - 1);
        g_overflow_task_name[configMAX_TASK_NAME_LEN - 1] =
            '\0'; /* 确保字符串结束 */
    }
    else
    {
        strcpy((char*) g_overflow_task_name, "Unknown");
    }

    /* 增加栈溢出计数 */
    g_stack_overflow_count++;

    /* 输出错误信息 */
    console_t* console = get_console();
    console->bReady = false;
    printf("任务:%s stack 溢出, task handle: 0x%08lX, Count: %d\r\n",
           (char*) g_overflow_task_name, (unsigned long) g_overflow_task_handle,
           g_stack_overflow_count);

    /* 进入死循环，防止系统继续运行 */
    for (;;)
    {
        /* 可以在这里添加LED闪烁等指示代码 */
    }
}
#endif
