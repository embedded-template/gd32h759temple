#pragma once
#include "yfy_data.h"
#include "stdbool.h"

// 返回值定义
typedef enum {
    YFY_OK = 0,
    YFY_ERROR_INVALID_ADDR,
    YFY_ERROR_INVALID_GROUP,
    YFY_ERROR_NULL_POINTER
} yfy_result_t;


typedef struct yfy_module_handle_t
{
    bool (*send)(uint32_t id, uint8_t* pdata);
    bool (*recv)(uint8_t id, uint8_t* pdata);
}yfy_module_handle_t;

void yfy_module_handle_init(yfy_module_handle_t* handle);
yfy_module_handle_t* yfy_module_handle_get(void);

// ========== 第一类：读取单个模块数据的函数 ==========
// 基本数据读取
yfy_result_t yfy_get_module_voltage(uint8_t module_addr, float* voltage);
yfy_result_t yfy_get_module_current(uint8_t module_addr, float* current);
yfy_result_t yfy_get_module_group(uint8_t module_addr, uint8_t* group);
yfy_result_t yfy_get_module_temp(uint8_t module_addr, int8_t* temp);

// 模块状态表2读取
yfy_result_t yfy_get_module_limit_power(uint8_t module_addr, uint8_t* status);
yfy_result_t yfy_get_module_id_duplicate(uint8_t module_addr, uint8_t* status);
yfy_result_t yfy_get_module_severe_unbalanced(uint8_t module_addr, uint8_t* status);
yfy_result_t yfy_get_three_phase_input_missing(uint8_t module_addr, uint8_t* status);
yfy_result_t yfy_get_three_phase_input_unbalanced(uint8_t module_addr, uint8_t* status);
yfy_result_t yfy_get_input_undervoltage(uint8_t module_addr, uint8_t* status);
yfy_result_t yfy_get_input_overvoltage(uint8_t module_addr, uint8_t* status);
yfy_result_t yfy_get_module_pfc_fault(uint8_t module_addr, uint8_t* status);

// 模块状态表1读取
yfy_result_t yfy_get_module_dc_side_fault(uint8_t module_addr, uint8_t* status);
yfy_result_t yfy_get_module_fault(uint8_t module_addr, uint8_t* status);
yfy_result_t yfy_get_module_protection(uint8_t module_addr, uint8_t* status);
yfy_result_t yfy_get_fan_fault(uint8_t module_addr, uint8_t* status);
yfy_result_t yfy_get_over_temp(uint8_t module_addr, uint8_t* status);
yfy_result_t yfy_get_output_overvoltage(uint8_t module_addr, uint8_t* status);
yfy_result_t yfy_get_walk_in_enable(uint8_t module_addr, uint8_t* status);
yfy_result_t yfy_get_module_comm_interrupt(uint8_t module_addr, uint8_t* status);

// 模块状态表0读取
yfy_result_t yfy_get_output_short_circuit(uint8_t module_addr, uint8_t* status);
yfy_result_t yfy_get_module_internal_comm_fault(uint8_t module_addr, uint8_t* status);
yfy_result_t yfy_get_input_or_bus_abnormal(uint8_t module_addr, uint8_t* status);
yfy_result_t yfy_get_module_sleep(uint8_t module_addr, uint8_t* status);
yfy_result_t yfy_get_module_discharge_abnormal(uint8_t module_addr, uint8_t* status);

// 交流输入电压读取
yfy_result_t yfy_get_ac_input_voltage_ab(uint8_t module_addr, uint16_t* voltage);
yfy_result_t yfy_get_ac_input_voltage_bc(uint8_t module_addr, uint16_t* voltage);
yfy_result_t yfy_get_ac_input_voltage_ca(uint8_t module_addr, uint16_t* voltage);

// 最大最小值读取
yfy_result_t yfy_get_max_voltage(uint8_t module_addr, uint16_t* voltage);
yfy_result_t yfy_get_min_voltage(uint8_t module_addr, uint16_t* voltage);
yfy_result_t yfy_get_max_current(uint8_t module_addr, uint16_t* current);
yfy_result_t yfy_get_max_power(uint8_t module_addr, uint16_t* power);

// 其他参数读取
yfy_result_t yfy_get_external_voltage(uint8_t module_addr, uint16_t* voltage);
yfy_result_t yfy_get_max_output_current(uint8_t module_addr, uint16_t* current);

// ========== 第二类：读取模块组数据的函数 ==========
yfy_result_t yfy_get_group_voltage(uint8_t group_num, float* voltage);
yfy_result_t yfy_get_group_current(uint8_t group_num, float* current);
yfy_result_t yfy_get_group_module_num(uint8_t group_num, uint8_t* module_num);

// ========== 第三类：读取系统数据的函数 ==========
yfy_result_t yfy_get_sys_voltage(float* voltage);
yfy_result_t yfy_get_sys_current(float* current);
yfy_result_t yfy_get_sys_module_num(uint8_t* module_num);