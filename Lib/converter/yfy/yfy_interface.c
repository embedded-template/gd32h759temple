#include "yfy_interface.h"
#include <string.h>

extern module_data_t* get_module_data(void);
extern group_module_data_t* get_group_module_data(void);
extern sys_module_data_t* get_sys_module_data(void);
extern void yfy_send_read_cmd(uint8_t dev_id, uint8_t cmd, uint8_t module_addr);
extern void yfy_send_write_cmd(uint8_t dev_id, uint8_t cmd, uint8_t module_addr, uint8_t* pdata);

// 内部辅助函数：检查模块地址是否有效
static bool is_valid_module_addr(uint8_t module_addr)
{
    return (module_addr < MODULE_NUM);
}

// 内部辅助函数：检查组号是否有效（组号从1开始）
static bool is_valid_group_num(uint8_t group_num)
{
    return (group_num >= 1 && group_num <= GROUP_MODULE_NUM);
}

// ========== 第一类：读取单个模块数据的函数 ==========

// 基本数据读取
/**
 * @brief 读取指定模块的输出电压
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param voltage 输出电压值指针，单位：V (伏特)
 * @return yfy_result_t 操作结果
 * @note 读取模块的实时输出电压，通常为直流电压值
 */
yfy_result_t yfy_get_module_voltage(uint8_t module_addr, float* voltage)
{
    if (voltage == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *voltage = module_data->voltage[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取指定模块的输出电流
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param current 输出电流值指针，单位：A (安培)
 * @return yfy_result_t 操作结果
 * @note 读取模块的实时输出电流
 */
yfy_result_t yfy_get_module_current(uint8_t module_addr, float* current)
{
    if (current == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *current = module_data->current[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取指定模块所属的组号
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param group 组号指针，范围：1 ~ GROUP_MODULE_NUM
 * @return yfy_result_t 操作结果
 * @note 读取模块当前配置的组号，用于并联组管理
 */
yfy_result_t yfy_get_module_group(uint8_t module_addr, uint8_t* group)
{
    if (group == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *group = module_data->group[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取指定模块的温度
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param temp 温度值指针，单位：°C (摄氏度)
 * @return yfy_result_t 操作结果
 * @note 读取模块内部温度，用于过温保护和散热监控
 */
yfy_result_t yfy_get_module_temp(uint8_t module_addr, int8_t* temp)
{
    if (temp == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *temp = module_data->temp[module_addr];
    return YFY_OK;
}

// 模块状态表2读取 - 输入侧和功率相关状态
/**
 * @brief 读取模块限功率状态
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param status 状态值指针，0=正常，1=限功率运行
 * @return yfy_result_t 操作结果
 * @note 指示模块是否处于功率限制状态，通常由温度或其他保护触发
 */
yfy_result_t yfy_get_module_limit_power(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->module_limit_power[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取模块ID重复状态
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param status 状态值指针，0=正常，1=ID重复
 * @return yfy_result_t 操作结果
 * @note 检测系统中是否存在相同地址的模块，避免通信冲突
 */
yfy_result_t yfy_get_module_id_duplicate(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->module_id_duplicate[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取模块严重不均流状态
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param status 状态值指针，0=正常，1=严重不均流
 * @return yfy_result_t 操作结果
 * @note 检测并联模块间电流分配是否严重不均，影响系统稳定性
 */
yfy_result_t yfy_get_module_severe_unbalanced(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->module_severe_unbalanced[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取三相输入缺相告警状态
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param status 状态值指针，0=正常，1=缺相告警
 * @return yfy_result_t 操作结果
 * @note 检测交流输入是否缺少某一相，缺相会影响模块正常工作
 */
yfy_result_t yfy_get_three_phase_input_missing(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->three_phase_input_missing[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取三相输入不平衡告警状态
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param status 状态值指针，0=正常，1=不平衡告警
 * @return yfy_result_t 操作结果
 * @note 检测三相输入电压是否平衡，不平衡会导致效率降低
 */
yfy_result_t yfy_get_three_phase_input_unbalanced(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->three_phase_input_unbalanced[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取输入欠压告警状态
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param status 状态值指针，0=正常，1=欠压告警
 * @return yfy_result_t 操作结果
 * @note 检测交流输入电压是否低于正常工作范围
 */
yfy_result_t yfy_get_input_undervoltage(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->input_undervoltage[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取输入过压告警状态
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param status 状态值指针，0=正常，1=过压告警
 * @return yfy_result_t 操作结果
 * @note 检测交流输入电压是否超过正常工作范围
 */
yfy_result_t yfy_get_input_overvoltage(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->input_overvoltage[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取模块PFC故障状态
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param status 状态值指针，0=正常，1=PFC故障
 * @return yfy_result_t 操作结果
 * @note 检测功率因数校正(PFC)电路是否故障，PFC故障会影响输入特性
 */
yfy_result_t yfy_get_module_pfc_fault(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->module_pfc_fault[module_addr];
    return YFY_OK;
}

// 模块状态表1读取 - 输出侧和系统状态
/**
 * @brief 读取模块DC侧故障状态
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param status 状态值指针，0=正常，1=DC侧故障
 * @return yfy_result_t 操作结果
 * @note 检测直流输出侧是否存在故障，如输出短路、过流等
 */
yfy_result_t yfy_get_module_dc_side_fault(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->module_dc_side_fault[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取模块故障告警状态
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param status 状态值指针，0=正常，1=模块故障
 * @return yfy_result_t 操作结果
 * @note 通用模块故障指示，包含各种内部故障情况
 */
yfy_result_t yfy_get_module_fault(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->module_fault[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取模块保护告警状态
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param status 状态值指针，0=正常，1=保护激活
 * @return yfy_result_t 操作结果
 * @note 指示模块保护功能是否激活，如过压、过流、过温保护等
 */
yfy_result_t yfy_get_module_protection(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->module_protection[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取风扇故障告警状态
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param status 状态值指针，0=正常，1=风扇故障
 * @return yfy_result_t 操作结果
 * @note 检测散热风扇是否正常工作，风扇故障会影响散热效果
 */
yfy_result_t yfy_get_fan_fault(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->fan_fault[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取过温告警状态
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param status 状态值指针，0=正常，1=过温告警
 * @return yfy_result_t 操作结果
 * @note 检测模块温度是否超过安全阈值，过温会触发保护或降额
 */
yfy_result_t yfy_get_over_temp(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->over_temp[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取输出过压告警状态
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param status 状态值指针，0=正常，1=输出过压
 * @return yfy_result_t 操作结果
 * @note 检测直流输出电压是否超过设定的安全范围
 */
yfy_result_t yfy_get_output_overvoltage(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->output_overvoltage[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取WALK-IN使能状态
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param status 状态值指针，0=禁用，1=使能
 * @return yfy_result_t 操作结果
 * @note 检测软启动功能是否使能，用于平滑启动减少冲击
 */
yfy_result_t yfy_get_walk_in_enable(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->walk_in_enable[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取模块通信中断告警状态
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param status 状态值指针，0=正常，1=通信中断
 * @return yfy_result_t 操作结果
 * @note 检测与模块的通信是否正常，通信中断会影响监控和控制
 */
yfy_result_t yfy_get_module_comm_interrupt(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->module_comm_interrupt[module_addr];
    return YFY_OK;
}

// 模块状态表0读取 - 基础运行状态
/**
 * @brief 读取输出短路状态
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param status 状态值指针，0=正常，1=输出短路
 * @return yfy_result_t 操作结果
 * @note 检测直流输出端是否发生短路，短路会触发保护关断
 */
yfy_result_t yfy_get_output_short_circuit(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->output_short_circuit[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取模块内部通信故障状态
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param status 状态值指针，0=正常，1=内部通信故障
 * @return yfy_result_t 操作结果
 * @note 检测模块内部控制器间通信是否正常
 */
yfy_result_t yfy_get_module_internal_comm_fault(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->module_internal_comm_fault[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取输入或母线异常状态
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param status 状态值指针，0=正常，1=输入或母线异常
 * @return yfy_result_t 操作结果
 * @note 检测交流输入或直流母线是否存在异常情况
 */
yfy_result_t yfy_get_input_or_bus_abnormal(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->input_or_bus_abnormal[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取模块休眠状态
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param status 状态值指针，0=正常运行，1=休眠状态
 * @return yfy_result_t 操作结果
 * @note 检测模块是否处于休眠/待机状态，用于节能管理
 */
yfy_result_t yfy_get_module_sleep(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->module_sleep[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取模块放电异常状态
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param status 状态值指针，0=正常，1=放电异常
 * @return yfy_result_t 操作结果
 * @note 检测模块放电过程是否异常，可能影响输出稳定性
 */
yfy_result_t yfy_get_module_discharge_abnormal(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *status = module_data->module_discharge_abnormal[module_addr];
    return YFY_OK;
}

// 交流输入电压读取
/**
 * @brief 读取交流输入AB相间电压
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param voltage 电压值指针，单位：V (伏特)
 * @return yfy_result_t 操作结果
 * @note 读取三相交流输入中A相与B相之间的线电压有效值
 */
yfy_result_t yfy_get_ac_input_voltage_ab(uint8_t module_addr, uint16_t* voltage)
{
    if (voltage == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *voltage = module_data->ac_input_voltage_ab[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取交流输入BC相间电压
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param voltage 电压值指针，单位：V (伏特)
 * @return yfy_result_t 操作结果
 * @note 读取三相交流输入中B相与C相之间的线电压有效值
 */
yfy_result_t yfy_get_ac_input_voltage_bc(uint8_t module_addr, uint16_t* voltage)
{
    if (voltage == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *voltage = module_data->ac_input_voltage_bc[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取交流输入CA相间电压
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param voltage 电压值指针，单位：V (伏特)
 * @return yfy_result_t 操作结果
 * @note 读取三相交流输入中C相与A相之间的线电压有效值
 */
yfy_result_t yfy_get_ac_input_voltage_ca(uint8_t module_addr, uint16_t* voltage)
{
    if (voltage == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *voltage = module_data->ac_input_voltage_ca[module_addr];
    return YFY_OK;
}

// 最大最小值读取 - 历史极值记录
/**
 * @brief 读取模块记录的最大电压值
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param voltage 最大电压值指针，单位：V (伏特)
 * @return yfy_result_t 操作结果
 * @note 读取模块运行期间记录的输出电压最大值，用于监控电压波动
 */
yfy_result_t yfy_get_max_voltage(uint8_t module_addr, uint16_t* voltage)
{
    if (voltage == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *voltage = module_data->max_voltage[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取模块记录的最小电压值
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param voltage 最小电压值指针，单位：V (伏特)
 * @return yfy_result_t 操作结果
 * @note 读取模块运行期间记录的输出电压最小值，用于监控电压波动
 */
yfy_result_t yfy_get_min_voltage(uint8_t module_addr, uint16_t* voltage)
{
    if (voltage == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *voltage = module_data->min_voltage[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取模块记录的最大电流值
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param current 最大电流值指针，单位：A (安培)
 * @return yfy_result_t 操作结果
 * @note 读取模块运行期间记录的输出电流最大值，用于监控负载变化
 */
yfy_result_t yfy_get_max_current(uint8_t module_addr, uint16_t* current)
{
    if (current == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *current = module_data->max_current[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取模块记录的最大功率值
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param power 最大功率值指针，单位：W (瓦特)
 * @return yfy_result_t 操作结果
 * @note 读取模块运行期间记录的输出功率最大值，用于功率管理
 */
yfy_result_t yfy_get_max_power(uint8_t module_addr, uint16_t* power)
{
    if (power == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *power = module_data->max_power[module_addr];
    return YFY_OK;
}

// 其他参数读取
/**
 * @brief 读取外部电压检测值
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param voltage 外部电压值指针，单位：V (伏特)
 * @return yfy_result_t 操作结果
 * @note 读取模块外部电压检测端口的电压值，用于外部信号监控
 */
yfy_result_t yfy_get_external_voltage(uint8_t module_addr, uint16_t* voltage)
{
    if (voltage == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *voltage = module_data->external_voltage[module_addr];
    return YFY_OK;
}

/**
 * @brief 读取当前最大输出电流限制值
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param current 最大输出电流限制值指针，单位：A (安培)
 * @return yfy_result_t 操作结果
 * @note 读取模块当前设定的最大输出电流限制，用于过流保护
 */
yfy_result_t yfy_get_max_output_current(uint8_t module_addr, uint16_t* current)
{
    if (current == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *current = module_data->max_output_current[module_addr];
    return YFY_OK;
}

// ========== 第二类：读取模块组数据的函数 ==========
/**
 * @brief 读取指定组的总电压
 * @param group_num 组号 (1 ~ GROUP_MODULE_NUM)
 * @param voltage 组电压值指针，单位：V (伏特)
 * @return yfy_result_t 操作结果
 * @note 读取并联组的总输出电压，所有组内模块共享相同电压
 */
yfy_result_t yfy_get_group_voltage(uint8_t group_num, float* voltage)
{
    if (voltage == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_group_num(group_num))
    {
        return YFY_ERROR_INVALID_GROUP;
    }

    group_module_data_t* group_data = get_group_module_data();
    // 组号从1开始，所以访问数组时需要减1
    *voltage = group_data->voltage[group_num - 1];
    return YFY_OK;
}

/**
 * @brief 读取指定组的总电流
 * @param group_num 组号 (1 ~ GROUP_MODULE_NUM)
 * @param current 组电流值指针，单位：A (安培)
 * @return yfy_result_t 操作结果
 * @note 读取并联组的总输出电流，为组内所有模块电流之和
 */
yfy_result_t yfy_get_group_current(uint8_t group_num, float* current)
{
    if (current == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_group_num(group_num))
    {
        return YFY_ERROR_INVALID_GROUP;
    }

    group_module_data_t* group_data = get_group_module_data();
    // 组号从1开始，所以访问数组时需要减1
    *current = group_data->current[group_num - 1];
    return YFY_OK;
}

/**
 * @brief 读取指定组内的模块数量
 * @param group_num 组号 (1 ~ GROUP_MODULE_NUM)
 * @param module_num 组内模块数量指针
 * @return yfy_result_t 操作结果
 * @note 读取当前组内实际工作的模块数量，用于负载均衡计算
 */
yfy_result_t yfy_get_group_module_num(uint8_t group_num, uint8_t* module_num)
{
    if (module_num == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_group_num(group_num))
    {
        return YFY_ERROR_INVALID_GROUP;
    }

    group_module_data_t* group_data = get_group_module_data();
    // 组号从1开始，所以访问数组时需要减1
    *module_num = group_data->module_num[group_num - 1];
    return YFY_OK;
}

/**
 * @brief 读取指定模块的地址模式
 * @param module_addr 模块地址 (0 ~ MODULE_NUM-1)
 * @param addr_mode 地址模式指针，1=地址拨码，组号命令控制，0=地址自动分配，组号拨码控制
 * @return yfy_result_t 操作结果
 * @note 读取模块当前配置的地址模式，用于通讯管理
 */
yfy_result_t yfy_get_module_addr_mode(uint8_t module_addr, uint8_t* addr_mode)
{
    if (addr_mode == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }
    if (!is_valid_module_addr(module_addr))
    {
        return YFY_ERROR_INVALID_ADDR;
    }

    module_data_t* module_data = get_module_data();
    *addr_mode = module_data->addr_modeL[module_addr];
    return YFY_OK;
}

// ========== 第三类：读取系统数据的函数 ==========
/**
 * @brief 读取整个系统的总电压
 * @param voltage 系统电压值指针，单位：V (伏特)
 * @return yfy_result_t 操作结果
 * @note 读取整个电源系统的输出电压，通常为所有组的平均值或代表值
 */
yfy_result_t yfy_get_sys_voltage(float* voltage)
{
    if (voltage == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }

    sys_module_data_t* sys_data = get_sys_module_data();
    *voltage = sys_data->voltage;
    return YFY_OK;
}

/**
 * @brief 读取整个系统的总电流
 * @param current 系统电流值指针，单位：A (安培)
 * @return yfy_result_t 操作结果
 * @note 读取整个电源系统的总输出电流，为所有模块电流之和
 */
yfy_result_t yfy_get_sys_current(float* current)
{
    if (current == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }

    sys_module_data_t* sys_data = get_sys_module_data();
    *current = sys_data->current;
    return YFY_OK;
}

/**
 * @brief 读取系统中的总模块数量
 * @param module_num 系统模块数量指针
 * @return yfy_result_t 操作结果
 * @note 读取系统中实际工作的模块总数，用于系统状态监控
 */
yfy_result_t yfy_get_sys_module_num(uint8_t* module_num)
{
    if (module_num == NULL)
    {
        return YFY_ERROR_NULL_POINTER;
    }

    sys_module_data_t* sys_data = get_sys_module_data();
    *module_num = sys_data->module_num;
    return YFY_OK;
}

// ========== 发送读取命令的函数 ==========
/**
 * @brief 读取系统输出
 *
 */
void yfy_send_read_sys_output(void)
{
    yfy_send_read_cmd(DEVICE_ID, 0x01, BROADCAST_ADDR);
}

/**
 * @brief 读取系统模块数量
 *
 */
void yfy_send_read_sys_module_num(void)
{
    yfy_send_read_cmd(DEVICE_ID, 0x02, BROADCAST_ADDR);
}

/**
 * @brief 读取组输出
 *
 */
void yfy_send_read_group_output(uint8_t module_addr)
{
    yfy_send_read_cmd(GROUP_DEVICE_ID, 0x01, module_addr);
}

/**
 * @brief 读取组模块数量
 *
 */
void yfy_send_read_group_module_num(uint8_t module_addr)
{
    yfy_send_read_cmd(GROUP_DEVICE_ID, 0x02, module_addr);
}

/**
 * @brief 读取单个模块输出
 *
 */
void yfy_send_read_single_output(uint8_t module_addr)
{
    yfy_send_read_cmd(DEVICE_ID, 0x03, module_addr);
}

/**
 * @brief 读取组内模块输出
 *
 */
void yfy_send_read_single_output_by_group(uint8_t group_num)
{
    yfy_send_read_cmd(GROUP_DEVICE_ID, 0x03, group_num);
}

/**
 * @brief 读取单个模块信息1：模块组号、温度、状态表2，状态表1，状态表0
 *
 */
void yfy_send_read_single_info_1(uint8_t module_addr)
{
    yfy_send_read_cmd(DEVICE_ID, 0x04, module_addr);
}

/**
 * @brief 读取组内模块信息1：模块组号、温度、状态表2，状态表1，状态表0
 *
 */
void yfy_send_read_single_info_1_by_group(uint8_t group_num)
{
    yfy_send_read_cmd(GROUP_DEVICE_ID, 0x04, group_num);
}

/**
 * @brief 读取单个模块信息2：交流输入电压AB、BC、CA相电压
 *
 */
void yfy_send_read_single_info_2(uint8_t module_addr)
{
    yfy_send_read_cmd(DEVICE_ID, 0x06, module_addr);
}

/**
 * @brief 读取组内模块信息2：交流输入电压AB、BC、CA相电压
 *
 */
void yfy_send_read_single_info_2_by_group(uint8_t group_num)
{
    yfy_send_read_cmd(GROUP_DEVICE_ID, 0x06, group_num);
}

/**
 * @brief 读取单个模块信息3：最大电压、最小电压、最大电流、额定功率
 *
 */
void yfy_send_read_single_info_3(uint8_t module_addr)
{
    yfy_send_read_cmd(DEVICE_ID, 0x0A, module_addr);
}

/**
 * @brief 读取组内模块信息3：最大电压、最小电压、最大电流、额定功率
 *
 */
void yfy_send_read_single_info_3_by_group(uint8_t group_num)
{
    yfy_send_read_cmd(GROUP_DEVICE_ID, 0x0A, group_num);
}

/**
 * @brief 读取单个模块信息4：条形码
 *
 */
void yfy_send_read_single_info_4(uint8_t module_addr)
{
    yfy_send_read_cmd(DEVICE_ID, 0x0B, module_addr);
}

/**
 * @brief 读取组内模块信息4：条形码
 *
 */
void yfy_send_read_single_info_4_by_group(uint8_t group_num)
{
    yfy_send_read_cmd(GROUP_DEVICE_ID, 0x0B, group_num);
}

/**
 * @brief 读取单个模块信息4：外部电压、当前最大输出电流
 *
 */
void yfy_send_read_single_info_5(uint8_t module_addr)
{
    yfy_send_read_cmd(DEVICE_ID, 0x0C, module_addr);
}

/**
 * @brief 读取组内模块信息4：外部电压、当前最大输出电流
 *
 */
void yfy_send_read_single_info_5_by_group(uint8_t group_num)
{
    yfy_send_read_cmd(GROUP_DEVICE_ID, 0x0C, group_num);
}

// ========== 发送设置命令的函数 ==========
/**
 * @brief 设置系统WALK-IN使能状态
 * @param enable 使能状态，0=禁用，1=使能
 *
 */
void yfy_send_write_sys_WalkIn(bool enable)
{
    uint8_t pdata[8] = {0};
    pdata[0] = (uint8_t) enable;
    yfy_send_write_cmd(DEVICE_ID, 0x13, BROADCAST_ADDR, pdata);
}

/**
 * @brief 设置组WALK-IN使能状态
 * @param group_num 组号
 * @param enable 使能状态，0=禁用，1=使能
 */
void yfy_send_write_WalkIn_by_group(uint8_t group_num, bool enable)
{
    uint8_t pdata[8] = {0};
    pdata[0] = (uint8_t) enable;
    yfy_send_write_cmd(GROUP_DEVICE_ID, 0x13, group_num, pdata);
}

/**
 * @brief 设置单个模块WALK-IN使能状态
 * @param module_addr 模块地址
 * @param enable 使能状态，0=禁用，1=使能
 */
void yfy_send_write_single_WalkIn(uint8_t module_addr, bool enable)
{

    uint8_t pdata[8] = {0};
    pdata[0] = (uint8_t) enable;
    yfy_send_write_cmd(DEVICE_ID, 0x13, module_addr, pdata);
}

/**
 * @brief 设置系统绿灯闪烁使能状态
 * @param enable 使能状态，0=禁用，1=使能
 */
void yfy_send_write_sys_green_led_blink(bool enable)
{
    uint8_t pdata[8] = {0};
    pdata[0] = (uint8_t) enable;
    yfy_send_write_cmd(DEVICE_ID, 0x14, BROADCAST_ADDR, pdata);
}

/**
 * @brief 设置组绿灯闪烁使能状态
 * @param group_num 组号
 * @param enable 使能状态，0=禁用，1=使能
 */
void yfy_send_write_green_led_blink_by_group(uint8_t group_num, bool enable)
{
    uint8_t pdata[8] = {0};
    pdata[0] = (uint8_t) enable;
    yfy_send_write_cmd(GROUP_DEVICE_ID, 0x14, group_num, pdata);
}

/**
 * @brief 设置单个模块绿灯闪烁使能状态
 * @param module_addr 模块地址
 * @param enable 使能状态，0=禁用，1=使能
 */
void yfy_send_write_single_green_led_blink(uint8_t module_addr, bool enable)
{
    uint8_t pdata[8] = {0};
    pdata[0] = (uint8_t) enable;
    yfy_send_write_cmd(DEVICE_ID, 0x14, module_addr, pdata);
}

/**
 * @brief 设置系统组号
 * @param group_num 组号
 */
void yfy_send_write_sys_group_num(uint8_t group_num)
{
    uint8_t pdata[8] = {0};
    pdata[0] = group_num;
    yfy_send_write_cmd(DEVICE_ID, 0x16, BROADCAST_ADDR, pdata);
}

/**
 * @brief 设置组内模块组号
 * @param group_num 组号
 * @param new_group_num 新组号
 */
void yfy_send_write_group_num_by_group(uint8_t group_num, uint8_t new_group_num)
{
    uint8_t pdata[8] = {0};
    pdata[0] = new_group_num;
    yfy_send_write_cmd(GROUP_DEVICE_ID, 0x16, group_num, pdata);
}

/**
 * @brief 设置单个模块组号
 * @param module_addr 模块地址
 * @param group_num 组号
 */
void yfy_send_write_single_group_num(uint8_t module_addr, uint8_t group_num)
{
    uint8_t pdata[8] = {0};
    pdata[0] = group_num;
    yfy_send_write_cmd(DEVICE_ID, 0x16, module_addr, pdata);
}

/**
 * @brief 设置系统休眠使能状态
 * @param enable 使能状态，0=禁用，1=使能
 */
void yfy_send_write_sys_sleep(bool enable)
{
    uint8_t pdata[8] = {0};
    pdata[0] = (uint8_t) enable;
    yfy_send_write_cmd(DEVICE_ID, 0x19, BROADCAST_ADDR, pdata);
}

/**
 * @brief 设置组休眠使能状态
 * @param group_num 组号
 * @param enable 使能状态，0=禁用，1=使能
 */
void yfy_send_write_sleep_by_group(uint8_t group_num, bool enable)
{
    uint8_t pdata[8] = {0};
    pdata[0] = (uint8_t) enable;
    yfy_send_write_cmd(GROUP_DEVICE_ID, 0x19, group_num, pdata);
}

/**
 * @brief 设置单个模块休眠使能状态
 * @param module_addr 模块地址
 * @param enable 使能状态，0=禁用，1=使能
 */
void yfy_send_write_single_sleep(uint8_t module_addr, bool enable)
{
    uint8_t pdata[8] = {0};
    pdata[0] = (uint8_t) enable;
    yfy_send_write_cmd(DEVICE_ID, 0x19, module_addr, pdata);
}

/**
 * @brief 设置系统工作使能状态
 * @param enable 使能状态，0=禁用，1=使能
 */
void yfy_send_write_sys_work(bool enable)
{
    uint8_t pdata[8] = {0};
    pdata[0] = (uint8_t) enable;
    yfy_send_write_cmd(DEVICE_ID, 0x1A, BROADCAST_ADDR, pdata);
}

/**
 * @brief 设置组工作使能状态
 * @param group_num 组号
 * @param enable 使能状态，0=禁用，1=使能
 */
void yfy_send_write_work_by_group(uint8_t group_num, bool enable)
{
    uint8_t pdata[8] = {0};
    pdata[0] = (uint8_t) enable;
    yfy_send_write_cmd(GROUP_DEVICE_ID, 0x1A, group_num, pdata);
}

/**
 * @brief 设置单个模块工作使能状态
 * @param module_addr 模块地址
 * @param enable 使能状态，0=禁用，1=使能
 */
void yfy_send_write_single_work(uint8_t module_addr, bool enable)
{
    uint8_t pdata[8] = {0};
    pdata[0] = (uint8_t) enable;
    yfy_send_write_cmd(DEVICE_ID, 0x1A, module_addr, pdata);
}

/**
 * @brief 设置系统输出电压和电流
 * @param volt 输出电压，单位：mV (毫伏)
 * @param current 输出电流，单位：mA (毫安)
 */
void yfy_send_write_sys_output_all(uint32_t volt, uint32_t current)
{
    uint8_t pdata[8] = {0};
    memcpy(pdata, &volt, 4);
    memcpy(pdata + 4, &current, 4);
    yfy_send_write_cmd(DEVICE_ID, 0x1B, BROADCAST_ADDR, pdata);
}

/**
 * @brief 设置组输出电压和电流
 * @param group_num 组号
 * @param volt 输出电压，单位：mV (毫伏)
 * @param current 输出电流，单位：mA (毫安)
 */
void yfy_send_write_output_all_by_group(uint8_t group_num, uint32_t volt, uint32_t current)
{
    uint8_t pdata[8] = {0};
    memcpy(pdata, &volt, 4);
    memcpy(pdata + 4, &current, 4);
    yfy_send_write_cmd(GROUP_DEVICE_ID, 0x1B, group_num, pdata);
}

/**
 * @brief 设置单个模块输出电压和电流
 * @param module_addr 模块地址
 * @param volt 输出电压，单位：mV (毫伏)
 * @param current 输出电流，单位：mA (毫安)
 */
void yfy_send_write_sys_output(uint32_t volt, uint32_t current)
{
    uint8_t pdata[8] = {0};
    memcpy(pdata, &volt, 4);
    memcpy(pdata + 4, &current, 4);
    yfy_send_write_cmd(DEVICE_ID, 0x1C, BROADCAST_ADDR, pdata);
}

/**
 * @brief 设置组内模块输出电压和电流
 * @param group_num 组号
 * @param volt 输出电压，单位：mV (毫伏)
 * @param current 输出电流，单位：mA (毫安)
 */
void yfy_send_write_output_by_group(uint8_t group_num, uint32_t volt, uint32_t current)
{
    uint8_t pdata[8] = {0};
    memcpy(pdata, &volt, 4);
    memcpy(pdata + 4, &current, 4);
    yfy_send_write_cmd(GROUP_DEVICE_ID, 0x1C, group_num, pdata);
}

/**
 * @brief 设置单个模块输出电压和电流
 * @param module_addr 模块地址
 * @param volt 输出电压，单位：mV (毫伏)
 * @param current 输出电流，单位：mA (毫安)
 */
void yfy_send_write_single_output(uint8_t module_addr, uint32_t volt, uint32_t current)
{
    uint8_t pdata[8] = {0};
    memcpy(pdata, &volt, 4);
    memcpy(pdata + 4, &current, 4);
    yfy_send_write_cmd(DEVICE_ID, 0x1C, module_addr, pdata);
}

/**
 * @brief 设置系统地址模式
 * @param model 地址模式，0=地址自动分配，组号拨码控制，1=地址拨码，组号命令控制
 */
void yfy_send_write_sys_addr_model(uint8_t model)
{
    uint8_t pdata[8] = {0};
    pdata[0] = model;
    yfy_send_write_cmd(DEVICE_ID, 0x1F, BROADCAST_ADDR, pdata);
}

/**
 * @brief 设置组地址模式
 * @param group_num 组号
 * @param model 地址模式，0=地址自动分配，组号拨码控制，1=地址拨码，组号命令控制
 */
void yfy_send_write_addr_model_by_group(uint8_t group_num, uint8_t model)
{
    uint8_t pdata[8] = {0};
    pdata[0] = model;
    yfy_send_write_cmd(GROUP_DEVICE_ID, 0x1F, group_num, pdata);
}

/**
 * @brief 设置单个模块地址模式
 * @param module_addr 模块地址
 * @param model 地址模式，0=地址自动分配，组号拨码控制，1=地址拨码，组号命令控制
 */
void yfy_send_write_single_addr_model(uint8_t module_addr, uint8_t model)
{
    uint8_t pdata[8] = {0};
    pdata[0] = model;
    yfy_send_write_cmd(DEVICE_ID, 0x1F, module_addr, pdata);
}
