#pragma once

#include "logging_stack.h"
#include "FreeRTOS.h"

// Function to convert ticks to time string
static inline void format_tick_time(TickType_t tick, char* buffer, size_t buffer_size) {
    // Assuming configTICK_RATE_HZ is 1000 (1ms per tick)
    // Adjust if your tick rate is different
    const uint32_t tick_rate_hz = configTICK_RATE_HZ;
    uint32_t total_ms = tick * 1000 / tick_rate_hz;
    
    uint32_t hours = total_ms / (3600 * 1000);
    uint32_t minutes = (total_ms % (3600 * 1000)) / (60 * 1000);
    uint32_t seconds = (total_ms % (60 * 1000)) / 1000;
    uint32_t milliseconds = total_ms % 1000;
    
    snprintf(buffer, buffer_size, "%02lu:%02lu:%02lu.%03lu", 
             (unsigned long)hours, 
             (unsigned long)minutes, 
             (unsigned long)seconds, 
             (unsigned long)milliseconds);
}

// Function to get formatted time string
static inline const char* get_log_time(void) {
    static char time_str[16];
    format_tick_time(xTaskGetTickCount(), time_str, sizeof(time_str));
    return time_str;
}

// Macro to use the function
#define LogTime get_log_time()

#define Log_info(format, ...) LogInfo((format, ##__VA_ARGS__))
#define Log_warn(format, ...) LogWarn((format, ##__VA_ARGS__))
#define Log_error(format, ...) LogError((format, ##__VA_ARGS__))
#define Log_debug(format, ...) LogDebug((format, ##__VA_ARGS__))
#define Log_always(format, ...) LogAlways((format, ##__VA_ARGS__))