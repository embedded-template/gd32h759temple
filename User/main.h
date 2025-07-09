#pragma once
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>

typedef struct task_info_t
{
    TaskFunction_t pxTaskCode;
    const char* const pcName;
    const configSTACK_DEPTH_TYPE uxStackDepth;
    uint32_t time;
    UBaseType_t uxPriority;
    TaskHandle_t* const pxCreatedTask;
} task_info_t;

#define NAME_debug_task "test"
#define STACK_debug_task 100
#define PARAM_debug_task 100
#define PRIORITY_debug_task 3

/* 栈溢出调试信息全局变量声明 */
extern volatile TaskHandle_t g_overflow_task_handle;
extern volatile char g_overflow_task_name[configMAX_TASK_NAME_LEN];
extern volatile uint32_t g_stack_overflow_count;
