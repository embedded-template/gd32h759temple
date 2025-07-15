#include "power_interface.h"
#include "can/module_can.h"
#include "log.h"

static bool power_interface_init(module_type_t module_type);

TaskHandle_t yfy_module_task_handle = NULL;

power_interface_t power_interface[eModuleTypeMax] = {
    {
        .xHandle = NULL,
        .module_task = NULL,
        .send_set_sys_addr_model = yfy_send_write_sys_addr_model,
        .send_set_group_output = yfy_send_write_output_all_by_group,
        .send_set_single_output = yfy_send_write_single_output,
        .get_sys_module_num = yfy_get_sys_module_num,
        .get_single_voltage = yfy_get_module_voltage,
        .get_single_current = yfy_get_module_current,
        .get_single_is_online = yfy_module_is_online,
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
void power_control_task(void* pvParameters)
{
    power_interface_pre_init();
    power_interface_init(eModuleTypeYfy);
    while (1)
    {
        // TODO:增加接口判断机制。先初始化一个接口，并发送数据，如果没有回复，则说明接口不匹配，再初始化下一个接口。
        
        
        // 匹配到接口后定期发送读取指令:至少包括：
        // 1. 读取系统模块数量。可选，暂时无用。因为可以根据模块的通讯超时数据确定
        // 2. 读取所有模块当前输出电压和电流
        // 3. 读取所有模块当前组号

        vTaskDelay(pdMS_TO_TICKS(*(uint32_t*) pvParameters));
    }
}