#include "module_interface.h"
#include "can/module_can.h"
#include "log.h"

static bool power_interface_init(module_type_t module_type);

TaskHandle_t yfy_module_task_handle = NULL;

power_interface_t power_interface[eModuleTypeMax] = {
    {
        .xHandle = NULL,
        .module_task = NULL,
        .name = "yfy module",
        // ========== 发送读取命令的函数 ==========
        .send_read_sys_output = yfy_send_read_sys_output,
        .send_read_sys_module_num = yfy_send_read_sys_module_num,
        .send_read_group_output = yfy_send_read_group_output,
        .send_read_group_module_num = yfy_send_read_group_module_num,
        .send_read_single_output = yfy_send_read_single_output,
        .send_read_single_output_by_group = yfy_send_read_single_output_by_group,
        .send_read_single_info_1 = yfy_send_read_single_info_1,
        .send_read_single_info_1_by_group = yfy_send_read_single_info_1_by_group,
        .send_read_single_info_2 = yfy_send_read_single_info_2,
        .send_read_single_info_2_by_group = yfy_send_read_single_info_2_by_group,
        .send_read_single_info_3 = yfy_send_read_single_info_3,
        .send_read_single_info_3_by_group = yfy_send_read_single_info_3_by_group,
        .send_read_single_info_4 = yfy_send_read_single_info_4,
        .send_read_single_info_4_by_group = yfy_send_read_single_info_4_by_group,

        // ========== 发送设置命令的函数 ==========
        .send_write_sys_WalkIn = yfy_send_write_sys_WalkIn,
        .send_write_WalkIn_by_group = yfy_send_write_WalkIn_by_group,
        .send_write_single_WalkIn = yfy_send_write_single_WalkIn,
        .send_write_sys_green_led_blink = yfy_send_write_sys_green_led_blink,
        .send_write_green_led_blink_by_group = yfy_send_write_green_led_blink_by_group,
        .send_write_single_green_led_blink = yfy_send_write_single_green_led_blink,
        .send_write_sys_group_num = yfy_send_write_sys_group_num,
        .send_write_group_num_by_group = yfy_send_write_group_num_by_group,
        .send_write_single_group_num = yfy_send_write_single_group_num,
        .send_write_sys_sleep = yfy_send_write_sys_sleep,
        .send_write_sleep_by_group = yfy_send_write_sleep_by_group,
        .send_write_single_sleep = yfy_send_write_single_sleep,
        .send_write_sys_work = yfy_send_write_sys_work,
        .send_write_work_by_group = yfy_send_write_work_by_group,
        .send_write_single_work = yfy_send_write_single_work,
        .send_write_sys_output_all = yfy_send_write_sys_output_all,
        .send_write_output_all_by_group = yfy_send_write_output_all_by_group,
        .send_write_sys_output = yfy_send_write_sys_output,
        .send_write_output_by_group = yfy_send_write_output_by_group,
        .send_write_single_output = yfy_send_write_single_output,
        .send_write_sys_addr_model = yfy_send_write_sys_addr_model,
        .send_write_addr_model_by_group = yfy_send_write_addr_model_by_group,
        .send_write_single_addr_model = yfy_send_write_single_addr_model,

        // ========== 数据获取函数 ==========
        // 基本数据读取
        .get_module_voltage = yfy_get_module_voltage,
        .get_module_current = yfy_get_module_current,
        .get_module_group = yfy_get_module_group,
        .get_module_temp = yfy_get_module_temp,

        // 模块状态表2读取
        .get_module_limit_power = yfy_get_module_limit_power,
        .get_module_id_duplicate = yfy_get_module_id_duplicate,
        .get_module_severe_unbalanced = yfy_get_module_severe_unbalanced,
        .get_three_phase_input_missing = yfy_get_three_phase_input_missing,
        .get_three_phase_input_unbalanced = yfy_get_three_phase_input_unbalanced,
        .get_input_undervoltage = yfy_get_input_undervoltage,
        .get_input_overvoltage = yfy_get_input_overvoltage,
        .get_module_pfc_fault = yfy_get_module_pfc_fault,

        // 模块状态表1读取
        .get_module_dc_side_fault = yfy_get_module_dc_side_fault,
        .get_module_fault = yfy_get_module_fault,
        .get_module_protection = yfy_get_module_protection,
        .get_fan_fault = yfy_get_fan_fault,
        .get_over_temp = yfy_get_over_temp,
        .get_output_overvoltage = yfy_get_output_overvoltage,
        .get_walk_in_enable = yfy_get_walk_in_enable,
        .get_module_comm_interrupt = yfy_get_module_comm_interrupt,

        // 模块状态表0读取
        .get_output_short_circuit = yfy_get_output_short_circuit,
        .get_module_internal_comm_fault = yfy_get_module_internal_comm_fault,
        .get_input_or_bus_abnormal = yfy_get_input_or_bus_abnormal,
        .get_module_sleep = yfy_get_module_sleep,
        .get_module_discharge_abnormal = yfy_get_module_discharge_abnormal,

        // 交流输入电压读取
        .get_ac_input_voltage_ab = yfy_get_ac_input_voltage_ab,
        .get_ac_input_voltage_bc = yfy_get_ac_input_voltage_bc,
        .get_ac_input_voltage_ca = yfy_get_ac_input_voltage_ca,

        // 最大最小值读取
        .get_max_voltage = yfy_get_max_voltage,
        .get_min_voltage = yfy_get_min_voltage,
        .get_max_current = yfy_get_max_current,
        .get_max_power = yfy_get_max_power,

        // 其他参数读取
        .get_external_voltage = yfy_get_external_voltage,
        .get_max_output_current = yfy_get_max_output_current,
        .get_module_addr_mode = yfy_get_module_addr_mode,

        // 组数据读取
        .get_group_voltage = yfy_get_group_voltage,
        .get_group_current = yfy_get_group_current,
        .get_group_module_num = yfy_get_group_module_num,

        // 系统数据读取
        .get_sys_voltage = yfy_get_sys_voltage,
        .get_sys_current = yfy_get_sys_current,
        .get_sys_module_num = yfy_get_sys_module_num,

        // ========== 状态查询函数 ==========
        .module_is_online = yfy_module_is_online,
    },
};

power_interface_t* power_control = NULL;

/**
 * @brief 初始化电源控制接口,可多次初始化。
 *
 * @param module_type 模块类型
 */
static bool power_interface_init(module_type_t module_type)
{
    if (module_type < eModuleTypeMax)
    {
        power_control = &power_interface[module_type];
        power_control->found = true;
        return true;
    }
    return false;
}

/**
 * @brief 获取电源控制接口
 *
 * @return power_interface_t* 电源控制接口指针
 */
power_interface_t* power_interface_get(void)
{
    return power_control;
}

/**
 * @brief 发送模块数据接口
 */
bool module_data_send(uint32_t id, uint8_t* pdata)
{
    can_handle_t* handle = module_can_handle_get();
    uint32_t data[3] = {0};
    data[0] = id;
    data[1] = *(uint32_t*)pdata;
    data[2] = *(uint32_t*)(pdata + 4);
    if(handle->tx_ring_buffer->write((uint8_t*)data, sizeof(data), MODULE_CAN_TIMEOUT_MS) > 0)
    {
        return true;
    }
    return false;
}

/**
 * @brief 接收模块数据接口
 */
bool module_data_recv(uint32_t id, uint8_t* pdata)
{
    can_handle_t* handle = module_can_handle_get();
    uint32_t data[3] = {0};
    if(handle->rx_ring_buffer->read((uint8_t*)data, sizeof(data), MODULE_CAN_TIMEOUT_MS) > 0)
    {
        id = data[0];
        *(uint32_t*)pdata = data[1];
        *(uint32_t*)(pdata + 4) = data[2];
        return true;
    }
    return false;
}

/**
 * @brief 获取系统时间
 */
uint32_t module_data_time(void)
{
    return xTaskGetTickCount();
}

/**
 * @brief 模块数据处理任务
 */
void yfy_module_task(void* pvParameters)
{

    yfy_module_handle_init(module_data_send, module_data_recv, module_data_time);
    while (1)
    {
        yfy_process_data();
        vTaskDelay(pdMS_TO_TICKS(*(uint32_t*) pvParameters));
    }
}

/**
 * @brief 只初始化一次
 *
 */
void power_interface_pre_init(void)
{
    // yfy 电源模块
    power_interface[eModuleTypeYfy].xHandle = yfy_module_task_handle;
    power_interface[eModuleTypeYfy].module_task = yfy_module_task;
}

/**
 * @brief 功率控制任务
 */
void module_interface_init_task(void* pvParameters)
{
    //等待硬件初始化完成
    vTaskDelay(pdMS_TO_TICKS(1000));
    power_interface_pre_init();
    while (1)
    {
        // TODO:增加接口判断机制。先初始化一个接口，并发送数据，如果没有回复，则说明接口不匹配，再初始化下一个接口。
        uint32_t count = 10;
        bool found = false;
        for(uint16_t i = 0; i < eModuleTypeMax && !found; i++)
        {
            xTaskCreate(power_interface[i].module_task, power_interface[i].name, 100, NULL, 5, &power_interface[i].xHandle);
            while (count-- && !found)
            {
                //询问系统的模块数量
                power_interface[i].send_read_sys_module_num();
                vTaskDelay(pdMS_TO_TICKS(100));
                uint8_t module_num = 0;
                power_interface[i].get_sys_module_num(&module_num);
                if (module_num > 0)
                {
                    Log_info("找到电源接口: %s\n", power_interface[i].name);
                    power_interface_init(i);
                    found = true; // 设置标志，两层循环都会退出
                }
            }
            count = 10;
        }

        if(!found)
        {
            //没有找到匹配的接口，等待下次重试
            Log_debug("没有找到电源接口，5s后重试\n");
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }

        //已经找到匹配的接口，退出任务
        vTaskDelete(NULL);
    }
}