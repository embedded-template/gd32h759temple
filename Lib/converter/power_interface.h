#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "yfy_interface.h"

typedef enum module_type_t
{
    eModuleTypeYfy,
    eModuleTypeMax,
} module_type_t;

/**
 * @brief 电源控制接口
 * TODO:将所有接口完善
 */
typedef struct power_interface_t
{
    TaskHandle_t xHandle;
    void (*module_task)(void* pvParameters);
    void (*send_set_sys_addr_model)(uint8_t model);
    void (*send_set_group_output)(uint8_t group_num, uint32_t volt, uint32_t current);
    void (*send_set_single_output)(uint8_t module_addr, uint32_t volt, uint32_t current);
    bool (*get_sys_module_num)(uint8_t* module_num);
    bool (*get_single_voltage)(uint8_t module_addr, float* voltage);
    bool (*get_single_current)(uint8_t module_addr, float* current);
    bool (*get_single_is_online)(uint8_t module_addr);

} power_interface_t;

power_interface_t* power_interface_get(void);
void power_control_task(void* pvParameters);