#include "module_interface.h"
#include "can/module_can.h"
#include "log.h"

static bool power_interface_init(module_type_t module_type);

TaskHandle_t yfy_module_task_handle = NULL;

power_interface_t power_interface[eModuleTypeMax] = {
    {
        .xHandle = NULL,
        .module_task = NULL,
        .power_manage = NULL,
        .name = "yfy module",
        .module_type_query = yfy_module_check,
        .module_type_ok = yfy_module_ok,
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
 * @brief 发送模块数据接口
 */
bool module_data_send(uint32_t id, uint8_t* pdata)
{
    can_handle_t* handle = module_can_handle_get();
    uint32_t data[3] = {0};
    data[0] = id;
    data[1] = *(uint32_t*) pdata;
    data[2] = *(uint32_t*) (pdata + 4);
    if (handle->tx_ring_buffer->write((uint8_t*) data, sizeof(data), MODULE_CAN_TIMEOUT_MS) > 0)
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
    if (handle->rx_ring_buffer->read((uint8_t*) data, sizeof(data), MODULE_CAN_TIMEOUT_MS) > 0)
    {
        id = data[0];
        *(uint32_t*) pdata = data[1];
        *(uint32_t*) (pdata + 4) = data[2];
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
    power_interface[eModuleTypeYfy].power_manage = NULL; // TODO:添加控制策略
}

/**
 * @brief 功率控制任务
 */
void module_interface_init_task(void* pvParameters)
{
    // 等待硬件初始化完成
    vTaskDelay(pdMS_TO_TICKS(1000));
    power_interface_pre_init();
    while (1)
    {
        // 先初始化一个接口，并发送数据，如果没有回复，则说明接口不匹配，再初始化下一个接口。
        uint32_t count = 10;
        bool found = false;
        for (uint16_t i = 0; i < eModuleTypeMax && !found; i++)
        {
            xTaskCreate(power_interface[i].module_task, power_interface[i].name, 100, NULL, 5, &power_interface[i].xHandle);
            while (count-- && !found)
            {
                // 询问系统的模块数量
                power_interface[i].module_type_query();
                vTaskDelay(pdMS_TO_TICKS(100));
                uint8_t module_num = 0;
                if (power_interface[i].module_type_ok())
                {
                    Log_info("找到电源接口: %s\n", power_interface[i].name);
                    found = true; // 设置标志，两层循环都会退出
                    power_interface_init(i);
                    // TODO:创建功率调度任务
                    // xTaskCreate(power_interface[i].power_manage, "power_manage", 100, NULL, 5, NULL);
                }
            }
            count = 10;
            vTaskDelete(power_interface[i].xHandle);
        }

        if (!found)
        {
            // 没有找到匹配的接口，等待下次重试
            Log_error("没有找到电源接口，5s后重试\n");
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }

        // 已经找到匹配的接口，退出任务
        vTaskDelete(NULL);
    }
}