#pragma once

#include "logging_stack.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <stdint.h>

// 获取系统时间字符串的函数声明
static inline const char* get_system_time_string(void)
{
    static char time_buffer[32];
    TickType_t current_tick = xTaskGetTickCount();

    // 将tick转换为毫秒 (configTICK_RATE_HZ = 1000, 所以1 tick = 1ms)
    uint32_t total_ms = (uint32_t)current_tick;

    // 计算时、分、秒、毫秒
    uint32_t hours = total_ms / (1000U * 60U * 60U);
    uint32_t minutes = (total_ms % (1000U * 60U * 60U)) / (1000U * 60U);
    uint32_t seconds = (total_ms % (1000U * 60U)) / 1000U;
    uint32_t milliseconds = total_ms % 1000U;

    // 格式化时间字符串 HH:MM:SS.mmm
    snprintf(time_buffer, sizeof(time_buffer), "%02u:%02u:%02u.%03u",
             hours, minutes, seconds, milliseconds);

    return time_buffer;
}

#define LogTime get_system_time_string()

#define Log_info(format, ...) LogInfo((format, ##__VA_ARGS__))
#define Log_warn(format, ...) LogWarn((format, ##__VA_ARGS__))
#define Log_error(format, ...) LogError((format, ##__VA_ARGS__))
#define Log_debug(format, ...) LogDebug((format, ##__VA_ARGS__))
#define Log_always(format, ...) LogAlways((format, ##__VA_ARGS__))
