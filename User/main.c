#include "main.h"
#include "gd32h7xx.h"
#include "usart/debug_usart.h"
#include <stdio.h>

task_info_t task_info_all[] = {{console_task, NAME_debug_task, STACK_debug_task,
                                PARAM_debug_task, PRIORITY_debug_task, NULL}};

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

    for (int i = 0; i < TASK_NUM; i++)
    {
        xTaskCreate(task_info_all[i].pxTaskCode, task_info_all[i].pcName,
                    task_info_all[i].uxStackDepth, &task_info_all[i].time,
                    task_info_all[i].uxPriority,
                    task_info_all[i].pxCreatedTask);
    }
    vTaskStartScheduler();
    while (1);
}

#if (configCHECK_FOR_STACK_OVERFLOW)
void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName)
{
    for (;;)
    {
    }
}
#endif
