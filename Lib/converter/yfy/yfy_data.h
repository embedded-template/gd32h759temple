#pragma once
#include "stdint.h"

#define MODULE_NUM 12
//存在一个空闲组，因此组号比模块数多1.空闲组号为0
#define GROUP_MODULE_NUM (MODULE_NUM + 1)

typedef struct module_data_t{
    float voltage[MODULE_NUM];
    float current[MODULE_NUM];
    uint8_t group[MODULE_NUM];
    int8_t temp[MODULE_NUM];

    // 模块状态表2
    uint8_t module_limit_power[MODULE_NUM];//限功率
    uint8_t module_id_duplicate[MODULE_NUM];//模块ID重复
    uint8_t module_severe_unbalanced[MODULE_NUM];//模块严重不均流
    uint8_t three_phase_input_missing[MODULE_NUM];//三相输入缺相告警
    uint8_t three_phase_input_unbalanced[MODULE_NUM];//三相输入不平衡告警
    uint8_t input_undervoltage[MODULE_NUM];//输入欠压告警
    uint8_t input_overvoltage[MODULE_NUM];//输入过压告警
    uint8_t module_pfc_fault[MODULE_NUM];//模块PFC侧处于关机状态
    
    // 模块状态表1
    uint8_t module_dc_side_fault[MODULE_NUM];//模块 DC 侧处于关机状态
    uint8_t module_fault[MODULE_NUM];//模块故障告警
    uint8_t module_protection[MODULE_NUM];//模块保护告警
    uint8_t fan_fault[MODULE_NUM];//风扇故障告警
    uint8_t over_temp[MODULE_NUM];//过温告警
    uint8_t output_overvoltage[MODULE_NUM];//输出过压告警
    uint8_t walk_in_enable[MODULE_NUM];//WALK-IN 使能
    uint8_t module_comm_interrupt[MODULE_NUM];//模块通信中断告警

    // 模块状态表0
    uint8_t output_short_circuit[MODULE_NUM];//输出短路
    uint8_t module_internal_comm_fault[MODULE_NUM];//模块内部通信故障
    uint8_t input_or_bus_abnormal[MODULE_NUM];//输入或母线异常
    uint8_t module_sleep[MODULE_NUM];//模块休眠
    uint8_t module_discharge_abnormal[MODULE_NUM];//模块放电异常

    uint16_t ac_input_voltage_ab[MODULE_NUM];// 交流输入电压AB相电压
    uint16_t ac_input_voltage_bc[MODULE_NUM];// 交流输入电压BC相电压
    uint16_t ac_input_voltage_ca[MODULE_NUM];// 交流输入电压CA相电压

    //最大电压，最小电压，最大电流，最大功率
    uint16_t max_voltage[MODULE_NUM];
    uint16_t min_voltage[MODULE_NUM];
    uint16_t max_current[MODULE_NUM];
    uint16_t max_power[MODULE_NUM];

}module_data_t;

typedef struct group_module_data_t{
    float voltage[GROUP_MODULE_NUM];
    float current[GROUP_MODULE_NUM];
    uint8_t module_num[GROUP_MODULE_NUM];
}group_module_data_t;

typedef struct sys_module_data_t{
    float voltage;
    float current;
    uint8_t module_num;
}sys_module_data_t;

typedef struct module_info_t{
    uint16_t cmd;
    uint16_t byte_start;
    uint16_t byte_end;
    uint16_t bit_start;
    uint16_t bit_end;
    void* pdata;
}module_info_t;

typedef struct group_module_info_t{
    uint16_t cmd;
    uint16_t byte_start;
    uint16_t byte_end;
    uint16_t bit_start;
    uint16_t bit_end;
    void* pdata;
}group_module_info_t;

typedef struct sys_module_inf_t{
    uint16_t cmd;
    uint16_t byte_start;
    uint16_t byte_end;
    uint16_t bit_start;
    uint16_t bit_end;
    void* pdata;
}sys_module_inf_t;

