#include "yfy_data.h"

module_data_t stModuleData = {0};

group_module_data_t stGroupModuleData = {0};

sys_module_data_t stSysModuleData = {0};

module_info_t stModuleInfo[] = {
    {.cmd = 0x03, .byte_start = 0, .byte_end = 4, .bit_start = 0, .bit_end = 0, .pdata = &stModuleData.voltage[0]},
    {.cmd = 0x03, .byte_start = 4, .byte_end = 8, .bit_start = 0, .bit_end = 0, .pdata = &stModuleData.current[0]},
    {.cmd = 0x04, .byte_start = 2, .byte_end = 3, .bit_start = 0, .bit_end = 0, .pdata = &stModuleData.group[0]},
    {.cmd = 0x04, .byte_start = 4, .byte_end = 5, .bit_start = 0, .bit_end = 0, .pdata = &stModuleData.temp[0]},

    // 模块状态表2 (byte 5)
    {.cmd = 0x04, .byte_start = 5, .byte_end = 5, .bit_start = 0, .bit_end = 1, .pdata = &stModuleData.module_limit_power[0]},
    {.cmd = 0x04, .byte_start = 5, .byte_end = 5, .bit_start = 1, .bit_end = 2, .pdata = &stModuleData.module_id_duplicate[0]},
    {.cmd = 0x04, .byte_start = 5, .byte_end = 5, .bit_start = 2, .bit_end = 3, .pdata = &stModuleData.module_severe_unbalanced[0]},
    {.cmd = 0x04, .byte_start = 5, .byte_end = 5, .bit_start = 3, .bit_end = 4, .pdata = &stModuleData.three_phase_input_missing[0]},
    {.cmd = 0x04, .byte_start = 5, .byte_end = 5, .bit_start = 4, .bit_end = 5, .pdata = &stModuleData.three_phase_input_unbalanced[0]},
    {.cmd = 0x04, .byte_start = 5, .byte_end = 5, .bit_start = 5, .bit_end = 6, .pdata = &stModuleData.input_undervoltage[0]},
    {.cmd = 0x04, .byte_start = 5, .byte_end = 5, .bit_start = 6, .bit_end = 7, .pdata = &stModuleData.input_overvoltage[0]},
    {.cmd = 0x04, .byte_start = 5, .byte_end = 5, .bit_start = 7, .bit_end = 8, .pdata = &stModuleData.module_pfc_fault[0]},

    // 模块状态表1 (byte 6)
    {.cmd = 0x04, .byte_start = 6, .byte_end = 6, .bit_start = 0, .bit_end = 1, .pdata = &stModuleData.module_dc_side_fault[0]},
    {.cmd = 0x04, .byte_start = 6, .byte_end = 6, .bit_start = 1, .bit_end = 2, .pdata = &stModuleData.module_fault[0]},
    {.cmd = 0x04, .byte_start = 6, .byte_end = 6, .bit_start = 2, .bit_end = 3, .pdata = &stModuleData.module_protection[0]},
    {.cmd = 0x04, .byte_start = 6, .byte_end = 6, .bit_start = 3, .bit_end = 4, .pdata = &stModuleData.fan_fault[0]},
    {.cmd = 0x04, .byte_start = 6, .byte_end = 6, .bit_start = 4, .bit_end = 5, .pdata = &stModuleData.over_temp[0]},
    {.cmd = 0x04, .byte_start = 6, .byte_end = 6, .bit_start = 5, .bit_end = 6, .pdata = &stModuleData.output_overvoltage[0]},
    {.cmd = 0x04, .byte_start = 6, .byte_end = 6, .bit_start = 6, .bit_end = 7, .pdata = &stModuleData.walk_in_enable[0]},
    {.cmd = 0x04, .byte_start = 6, .byte_end = 6, .bit_start = 7, .bit_end = 8, .pdata = &stModuleData.module_comm_interrupt[0]},

    // 模块状态表0 (byte 7)
    {.cmd = 0x04, .byte_start = 7, .byte_end = 7, .bit_start = 0, .bit_end = 1, .pdata = &stModuleData.output_short_circuit[0]},
    {.cmd = 0x04, .byte_start = 7, .byte_end = 7, .bit_start = 2, .bit_end = 3, .pdata = &stModuleData.module_internal_comm_fault[0]},
    {.cmd = 0x04, .byte_start = 7, .byte_end = 7, .bit_start = 3, .bit_end = 4, .pdata = &stModuleData.input_or_bus_abnormal[0]},
    {.cmd = 0x04, .byte_start = 7, .byte_end = 7, .bit_start = 4, .bit_end = 5, .pdata = &stModuleData.module_sleep[0]},
    {.cmd = 0x04, .byte_start = 7, .byte_end = 7, .bit_start = 5, .bit_end = 6, .pdata = &stModuleData.module_discharge_abnormal[0]},

    {.cmd = 0x06, .byte_start = 0, .byte_end = 2, .bit_start = 0, .bit_end = 0, .pdata = &stModuleData.ac_input_voltage_ab[0]},
    {.cmd = 0x06, .byte_start = 2, .byte_end = 4, .bit_start = 0, .bit_end = 0, .pdata = &stModuleData.ac_input_voltage_bc[0]},
    {.cmd = 0x06, .byte_start = 4, .byte_end = 6, .bit_start = 0, .bit_end = 0, .pdata = &stModuleData.ac_input_voltage_ca[0]},

    {.cmd = 0x0A, .byte_start = 0, .byte_end = 2, .bit_start = 0, .bit_end = 0, .pdata = &stModuleData.max_voltage[0]},
    {.cmd = 0x0A, .byte_start = 2, .byte_end = 4, .bit_start = 0, .bit_end = 0, .pdata = &stModuleData.min_voltage[0]},
    {.cmd = 0x0A, .byte_start = 4, .byte_end = 6, .bit_start = 0, .bit_end = 0, .pdata = &stModuleData.max_current[0]},
    {.cmd = 0x0A, .byte_start = 6, .byte_end = 8, .bit_start = 0, .bit_end = 0, .pdata = &stModuleData.max_power[0]},
};

group_module_info_t stGroupModuleInfo[] = {
    {.cmd = 0x01, .byte_start = 0, .byte_end = 4, .bit_start = 0, .bit_end = 0, .pdata = &stGroupModuleData.voltage[0]},
    {.cmd = 0x01, .byte_start = 4, .byte_end = 8, .bit_start = 0, .bit_end = 0, .pdata = &stGroupModuleData.current[0]},
    {.cmd = 0x02, .byte_start = 2, .byte_end = 3, .bit_start = 0, .bit_end = 0, .pdata = &stGroupModuleData.module_num[0]}
};

sys_module_inf_t stSysModuleInf[] = {
    {.cmd = 0x01, .byte_start = 0, .byte_end = 4, .bit_start = 0, .bit_end = 0, .pdata = &stSysModuleData.voltage},
    {.cmd = 0x01, .byte_start = 4, .byte_end = 8, .bit_start = 0, .bit_end = 0, .pdata = &stSysModuleData.current},
    {.cmd = 0x02, .byte_start = 2, .byte_end = 3, .bit_start = 0, .bit_end = 0, .pdata = &stSysModuleData.module_num}
};

