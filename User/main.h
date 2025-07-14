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

#define NAME_cli_task "cli"
#define STACK_cli_task 200
#define PARAM_cli_task 10
#define PRIORITY_cli_task 3

#define NAME_power_control_task "power_control"
#define STACK_power_control_task 200
#define PARAM_power_control_task 10
#define PRIORITY_power_control_task 3