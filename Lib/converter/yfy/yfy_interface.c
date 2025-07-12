#include "yfy_interface.h"

// 内部辅助函数：检查模块地址是否有效
static bool is_valid_module_addr(uint8_t module_addr) {
    return (module_addr < MODULE_NUM);
}

// 内部辅助函数：检查组号是否有效（组号从1开始）
static bool is_valid_group_num(uint8_t group_num) {
    return (group_num >= 1 && group_num <= GROUP_MODULE_NUM);
}

// ========== 第一类：读取单个模块数据的函数 ==========

// 基本数据读取
yfy_result_t yfy_get_module_voltage(uint8_t module_addr, float* voltage) {
    if (voltage == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *voltage = module_data->voltage[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_module_current(uint8_t module_addr, float* current) {
    if (current == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *current = module_data->current[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_module_group(uint8_t module_addr, uint8_t* group) {
    if (group == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *group = module_data->group[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_module_temp(uint8_t module_addr, int8_t* temp) {
    if (temp == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *temp = module_data->temp[module_addr];
    return YFY_OK;
}

// 模块状态表2读取
yfy_result_t yfy_get_module_limit_power(uint8_t module_addr, uint8_t* status) {
    if (status == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->module_limit_power[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_module_id_duplicate(uint8_t module_addr, uint8_t* status) {
    if (status == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->module_id_duplicate[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_module_severe_unbalanced(uint8_t module_addr, uint8_t* status) {
    if (status == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->module_severe_unbalanced[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_three_phase_input_missing(uint8_t module_addr, uint8_t* status) {
    if (status == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->three_phase_input_missing[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_three_phase_input_unbalanced(uint8_t module_addr, uint8_t* status) {
    if (status == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->three_phase_input_unbalanced[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_input_undervoltage(uint8_t module_addr, uint8_t* status) {
    if (status == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->input_undervoltage[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_input_overvoltage(uint8_t module_addr, uint8_t* status) {
    if (status == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->input_overvoltage[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_module_pfc_fault(uint8_t module_addr, uint8_t* status) {
    if (status == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->module_pfc_fault[module_addr];
    return YFY_OK;
}

// 模块状态表1读取
yfy_result_t yfy_get_module_dc_side_fault(uint8_t module_addr, uint8_t* status) {
    if (status == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->module_dc_side_fault[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_module_fault(uint8_t module_addr, uint8_t* status) {
    if (status == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->module_fault[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_module_protection(uint8_t module_addr, uint8_t* status) {
    if (status == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->module_protection[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_fan_fault(uint8_t module_addr, uint8_t* status) {
    if (status == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->fan_fault[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_over_temp(uint8_t module_addr, uint8_t* status) {
    if (status == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->over_temp[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_output_overvoltage(uint8_t module_addr, uint8_t* status) {
    if (status == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->output_overvoltage[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_walk_in_enable(uint8_t module_addr, uint8_t* status) {
    if (status == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->walk_in_enable[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_module_comm_interrupt(uint8_t module_addr, uint8_t* status) {
    if (status == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->module_comm_interrupt[module_addr];
    return YFY_OK;
}

// 模块状态表0读取
yfy_result_t yfy_get_output_short_circuit(uint8_t module_addr, uint8_t* status) {
    if (status == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->output_short_circuit[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_module_internal_comm_fault(uint8_t module_addr, uint8_t* status) {
    if (status == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->module_internal_comm_fault[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_input_or_bus_abnormal(uint8_t module_addr, uint8_t* status) {
    if (status == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->input_or_bus_abnormal[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_module_sleep(uint8_t module_addr, uint8_t* status) {
    if (status == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->module_sleep[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_module_discharge_abnormal(uint8_t module_addr, uint8_t* status) {
    if (status == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->module_discharge_abnormal[module_addr];
    return YFY_OK;
}

// 交流输入电压读取
yfy_result_t yfy_get_ac_input_voltage_ab(uint8_t module_addr, uint16_t* voltage) {
    if (voltage == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *voltage = module_data->ac_input_voltage_ab[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_ac_input_voltage_bc(uint8_t module_addr, uint16_t* voltage) {
    if (voltage == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *voltage = module_data->ac_input_voltage_bc[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_ac_input_voltage_ca(uint8_t module_addr, uint16_t* voltage) {
    if (voltage == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *voltage = module_data->ac_input_voltage_ca[module_addr];
    return YFY_OK;
}

// 最大最小值读取
yfy_result_t yfy_get_max_voltage(uint8_t module_addr, uint16_t* voltage) {
    if (voltage == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *voltage = module_data->max_voltage[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_min_voltage(uint8_t module_addr, uint16_t* voltage) {
    if (voltage == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *voltage = module_data->min_voltage[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_max_current(uint8_t module_addr, uint16_t* current) {
    if (current == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *current = module_data->max_current[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_max_power(uint8_t module_addr, uint16_t* power) {
    if (power == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *power = module_data->max_power[module_addr];
    return YFY_OK;
}

// 其他参数读取
yfy_result_t yfy_get_external_voltage(uint8_t module_addr, uint16_t* voltage) {
    if (voltage == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *voltage = module_data->external_voltage[module_addr];
    return YFY_OK;
}

yfy_result_t yfy_get_max_output_current(uint8_t module_addr, uint16_t* current) {
    if (current == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr)) {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *current = module_data->max_output_current[module_addr];
    return YFY_OK;
}

// ========== 第二类：读取模块组数据的函数 ==========
yfy_result_t yfy_get_group_voltage(uint8_t group_num, float* voltage) {
    if (voltage == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_group_num(group_num)) {
        return YFY_ERROR_INVALID_GROUP;
    }

    group_module_data_t* group_data = get_group_module_data();
    // 组号从1开始，所以访问数组时需要减1
    *voltage = group_data->voltage[group_num - 1];
    return YFY_OK;
}

yfy_result_t yfy_get_group_current(uint8_t group_num, float* current) {
    if (current == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_group_num(group_num)) {
        return YFY_ERROR_INVALID_GROUP;
    }

    group_module_data_t* group_data = get_group_module_data();
    // 组号从1开始，所以访问数组时需要减1
    *current = group_data->current[group_num - 1];
    return YFY_OK;
}

yfy_result_t yfy_get_group_module_num(uint8_t group_num, uint8_t* module_num) {
    if (module_num == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_group_num(group_num)) {
        return YFY_ERROR_INVALID_GROUP;
    }

    group_module_data_t* group_data = get_group_module_data();
    // 组号从1开始，所以访问数组时需要减1
    *module_num = group_data->module_num[group_num - 1];
    return YFY_OK;
}

// ========== 第三类：读取系统数据的函数 ==========
yfy_result_t yfy_get_sys_voltage(float* voltage) {
    if (voltage == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }

    sys_module_data_t* sys_data = get_sys_module_data();
    *voltage = sys_data->voltage;
    return YFY_OK;
}

yfy_result_t yfy_get_sys_current(float* current) {
    if (current == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }

    sys_module_data_t* sys_data = get_sys_module_data();
    *current = sys_data->current;
    return YFY_OK;
}

yfy_result_t yfy_get_sys_module_num(uint8_t* module_num) {
    if (module_num == NULL) {
        return YFY_ERROR_NULL_POINTER;
    }

    sys_module_data_t* sys_data = get_sys_module_data();
    *module_num = sys_data->module_num;
    return YFY_OK;
}