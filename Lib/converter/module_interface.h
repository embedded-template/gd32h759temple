#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "yfy_interface.h"

typedef enum module_type_t
{
    eModuleTypeYfy,
    eModuleTypeYyln,
    eModuleTypeMax,
} module_type_t;

/**
 * @brief 电源控制接口
 * TODO:将所有接口完善
 */
typedef struct power_interface_t
{
    TaskHandle_t xHandle;
    char* name;
    void (*module_task)(void* pvParameters);
    void (*power_manage)(void* pvParameters);
    bool found;

    // ========== 发送读取命令的函数 ==========
    void (*module_type_query)(void);
    bool (*module_type_ok)(void);

} power_interface_t;

power_interface_t* power_interface_get(void);
void module_interface_init_task(void* pvParameters);