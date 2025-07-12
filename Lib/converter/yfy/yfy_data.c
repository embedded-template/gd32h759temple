#include "yfy_data.h"
#include "macro.h"
#include "string.h"
#include <stdint.h>

static bool yfy_data_parse(uint8_t dev_id, uint8_t cmd, uint8_t module_addr, uint8_t* pdata);
static bool yfy_data_unpack(module_info_type_t type, uint8_t cmd, uint8_t module_addr, uint8_t* p_data);
static void yfy_data_store(uint8_t byte_start, uint8_t byte_end, uint8_t bit_start, uint8_t bit_endd, uint8_t addr, uint8_t* pdata, void* pstore_data);

module_online_info_t stModuleOnlineInfo[MODULE_NUM] = {0};

module_online_info_t* get_module_online_info(void)
{
    return stModuleOnlineInfo;
}

yfy_module_handle_t module_handle = {0};

void yfy_module_handle_init(bool (*send)(uint32_t id, uint8_t* pdata), bool (*recv)(uint8_t id, uint8_t* pdata), uint32_t (*time)(void))
{
    module_handle.recv = recv;
    module_handle.send = send;
    module_handle.time = time;
}

yfy_module_handle_t* yfy_module_handle_get(void)
{
    return &module_handle;
}

/**
 * @defgroup 英飞源模块接收数据定义
 * @{
 */
module_data_t stModuleData = {0};

module_data_t* get_module_data(void)
{
    return &stModuleData;
}

group_module_data_t stGroupModuleData = {0};

group_module_data_t* get_group_module_data(void)
{
    return &stGroupModuleData;
}

sys_module_data_t stSysModuleData = {0};

sys_module_data_t* get_sys_module_data(void)
{
    return &stSysModuleData;
}

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

    {.cmd = 0x0C, .byte_start = 0, .byte_end = 2, .bit_start = 0, .bit_end = 0, .pdata = &stModuleData.external_voltage[0]},
    {.cmd = 0x0C, .byte_start = 2, .byte_end = 4, .bit_start = 0, .bit_end = 0, .pdata = &stModuleData.max_output_current[0]},

    {.cmd = 0x1ff, .byte_start = 0, .byte_end = 1, .bit_start = 0, .bit_end = 0, .pdata = &stModuleData.addr_modeL[0]},

};

group_module_info_t stGroupModuleInfo[] = {
    {.cmd = 0x01, .byte_start = 0, .byte_end = 4, .bit_start = 0, .bit_end = 0, .pdata = &stGroupModuleData.voltage[0]},
    {.cmd = 0x01, .byte_start = 4, .byte_end = 8, .bit_start = 0, .bit_end = 0, .pdata = &stGroupModuleData.current[0]},
    {.cmd = 0x02, .byte_start = 2, .byte_end = 3, .bit_start = 0, .bit_end = 0, .pdata = &stGroupModuleData.module_num[0]}};

sys_module_inf_t stSysModuleInf[] = {{.cmd = 0x01, .byte_start = 0, .byte_end = 4, .bit_start = 0, .bit_end = 0, .pdata = &stSysModuleData.voltage},
                                     {.cmd = 0x01, .byte_start = 4, .byte_end = 8, .bit_start = 0, .bit_end = 0, .pdata = &stSysModuleData.current},
                                     {.cmd = 0x02, .byte_start = 2, .byte_end = 3, .bit_start = 0, .bit_end = 0, .pdata = &stSysModuleData.module_num}};

set_moduole_inf_t stSetModuleInf[] = {
    {.cmd = 0x13, .byte_start = 0, .byte_end = 1}, {.cmd = 0x14, .byte_start = 0, .byte_end = 1}, {.cmd = 0x16, .byte_start = 0, .byte_end = 1},
    {.cmd = 0x19, .byte_start = 0, .byte_end = 1}, {.cmd = 0x1A, .byte_start = 0, .byte_end = 1}, {.cmd = 0x1B, .byte_start = 0, .byte_end = 1},
    {.cmd = 0x1C, .byte_start = 0, .byte_end = 4}, {.cmd = 0x1C, .byte_start = 4, .byte_end = 8}, {.cmd = 0x1F, .byte_start = 0, .byte_end = 1}};

/**
 * @}
 */

/**
 * @brief 接收数据主任务
 */
void yfy_process_data(void)
{
    yfy_module_handle_t* handle = yfy_module_handle_get();
    if (handle->recv == NULL)
    {
        return;
    }
    uint8_t id = 0;
    uint8_t pdata[8] = {0};

    module_online_check();

    for (int max_handle_count = 10; max_handle_count > 0; max_handle_count--)
    {
        if (handle->recv(id, pdata))
        {
            uint8_t dev_id = YFY_EXTRACT_DEVICE_ID(id);
            uint8_t cmd = YFY_EXTRACT_CMD(id);
            uint8_t module_addr = YFY_EXTRACT_SRC_ADDR(id);
            yfy_data_parse(dev_id, cmd, module_addr, pdata);
        }
        else
        {
            break;
        }
    }
}

/**
 * @brief 解析收到的数据
 *
 */
bool yfy_data_parse(uint8_t dev_id, uint8_t cmd, uint8_t module_addr, uint8_t* pdata)
{
    // 通过设备号与命令号判断数据属于单个模块数据、组数据、系统数据中的哪个
    if (cmd == 0x01 | cmd == 0x02)
    {
        // 数据为系统或者组
        if (module_addr == BROADCAST_ADDR)
        {
            // 数据为系统
            return yfy_data_unpack(eSysModuleInfo, cmd, module_addr, pdata);
        }
        else if (dev_id == GROUP_DEVICE_ID)
        {
            // 数据为组
            if (module_addr > GROUP_MODULE_NUM)
            {
                return false;
            }
            // 组编号从1开始
            return yfy_data_unpack(eGroupModuleInfo, cmd, (module_addr - 1), pdata);
        }
    }
    else
    {
        // 数据为单个模块
        if (module_addr >= MODULE_NUM)
        {
            return false;
        }
        // 记录模块通讯时间
        yfy_module_handle_t* handle = yfy_module_handle_get();
        module_online_info_t* online_info = get_module_online_info();
        online_info[module_addr].module_addr = module_addr;
        online_info[module_addr].last_online_time = handle->time();
        online_info[module_addr].is_online = true;
        
        return yfy_data_unpack(eModuleInfo, cmd, module_addr, pdata);
    }
}

/**
 * @brief 将数据发送到
 */
static bool yfy_data_unpack(module_info_type_t type, uint8_t cmd, uint8_t module_addr, uint8_t* pdata)
{
    uint8_t start_parse = 1;
    uint8_t on_parse = 2;
    uint8_t end_parse = 3;
    uint8_t parse_state = start_parse;
    uint8_t module_group = module_addr;
    uint8_t sys_addr = 0u;
    if (type == eModuleInfo)
    {
        for (uint8_t i = 0; i < sizeof(stModuleInfo) / sizeof(stModuleInfo[0]); i++)
        {
            if (stModuleInfo[i].cmd == cmd)
            {
                yfy_data_store(stModuleInfo[i].byte_start, stModuleInfo[i].byte_end, stModuleInfo[i].bit_start, stModuleInfo[i].bit_end, module_addr, pdata,
                               stModuleInfo[i].pdata);
                if (parse_state == start_parse)
                {
                    parse_state = on_parse;
                }
            }
            else if (parse_state == on_parse)
            {
                parse_state = end_parse;
                break;
            }
        }
    }
    else if (type == eGroupModuleInfo)
    {
        for (uint8_t i = 0; i < sizeof(stGroupModuleInfo) / sizeof(stGroupModuleInfo[0]); i++)
        {
            if (stGroupModuleInfo[i].cmd == cmd)
            {
                yfy_data_store(stGroupModuleInfo[i].byte_start, stGroupModuleInfo[i].byte_end, stGroupModuleInfo[i].bit_start, stGroupModuleInfo[i].bit_end,
                               module_group, pdata, stGroupModuleInfo[i].pdata);
                if (parse_state == start_parse)
                {
                    parse_state = on_parse;
                }
            }
            else if (parse_state == on_parse)
            {
                parse_state = end_parse;
                break;
            }
        }
    }
    else if (type == eSysModuleInfo)
    {
        for (uint8_t i = 0; i < sizeof(stSysModuleInf) / sizeof(stSysModuleInf[0]); i++)
        {
            if (stSysModuleInf[i].cmd == cmd)
            {
                yfy_data_store(stSysModuleInf[i].byte_start, stSysModuleInf[i].byte_end, stSysModuleInf[i].bit_start, stSysModuleInf[i].bit_end, sys_addr,
                               pdata, stSysModuleInf[i].pdata);
                if (parse_state == start_parse)
                {
                    parse_state = on_parse;
                }
            }
            else if (parse_state == on_parse)
            {
                parse_state = end_parse;
                break;
            }
        }
    }

    if (parse_state == end_parse)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief 存储数据
 */
static void yfy_data_store(uint8_t byte_start, uint8_t byte_end, uint8_t bit_start, uint8_t bit_endd, uint8_t addr, uint8_t* pdata, void* pstore_data)
{
    if (byte_start == byte_end)
    {
        // 数据为一个bit,从pstore_data中取出数据，将特定的bit存入
        uint8_t data = pdata[byte_start];
        uint8_t need_data = (data >> bit_start) & 0x01;
        ((uint8_t*) pstore_data)[addr] = need_data;
    }
    else
    {
        uint8_t bytes = byte_end - byte_start;
        uint8_t* p_data = (uint8_t*) pstore_data + (addr * bytes);
        // 当字节数大于1时，进行字节序转换（Big Endian -> Little Endian）
        if (bytes > 1)
        {
            // 创建临时缓冲区进行字节序转换（最大支持8字节）
            uint8_t temp_data[8];
            CONVERT_ENDIANNESS(pdata + byte_start, temp_data, bytes);
            memcpy(p_data, temp_data, bytes);
        }
        else
        {
            // 单字节数据不需要转换
            memcpy(p_data, pdata + byte_start, bytes);
        }
    }
}

/**
 * @brief 发送读取命令
 */
void yfy_send_read_cmd(uint8_t dev_id, uint8_t cmd, uint8_t module_addr)
{
    // 组装32位命令数据
    // 错误码设为0（正常），目的地址为模块地址，源地址为监控地址
    uint32_t cmd_data = YFY_SET_ID(0, dev_id, cmd, module_addr, MONITOR_ADDR);

    // 获取发送句柄
    yfy_module_handle_t* handle = yfy_module_handle_get();

    uint8_t send_data[8] = {0};
    // 检查发送函数是否已初始化
    if (handle != NULL && handle->send != NULL)
    {
        // 将32位数据转换为字节数组发送

        // 调用发送函数
        handle->send(cmd_data, send_data);
    }
}

/**
 * @brief 发送设置命令
 *
 */
void yfy_send_write_cmd(uint8_t dev_id, uint8_t cmd, uint8_t module_addr, uint8_t* pdata)
{
    // 组装32位命令数据
    // 错误码设为0（正常），目的地址为模块地址，源地址为监控地址
    uint32_t cmd_data = YFY_SET_ID(0, dev_id, cmd, module_addr, MONITOR_ADDR);

    // 获取发送句柄
    yfy_module_handle_t* handle = yfy_module_handle_get();

    // 检查发送函数是否已初始化
    if (handle != NULL && handle->send != NULL)
    {
        // 将32位数据转换为字节数组发送

        // 调用发送函数
        handle->send(cmd_data, pdata);
    }
}

/**
 * @brief 检查模块在线状态
 */
void module_online_check(void)
{
    static uint32_t last_check_time = 0;
    uint32_t current_time = module_handle.time();
    uint32_t check_interval_diff;

    // 计算距离上次检查的时间差（处理溢出）
    if (current_time >= last_check_time) {
        check_interval_diff = current_time - last_check_time;
    } else {
        // 处理32位时间溢出
        check_interval_diff = (UINT32_MAX - last_check_time) + current_time + 1;
    }

    // 每1000ms执行一次主要检查逻辑
    if (check_interval_diff >= 1000) {
        last_check_time = current_time;

        module_online_info_t* online_info = get_module_online_info();
        for (uint8_t i = 0; i < MODULE_NUM; i++)
        {
            if (online_info[i].is_online)
            {
                uint32_t last_time = online_info[i].last_online_time;
                uint32_t time_diff;

                // 处理32位时间溢出情况
                if (current_time >= last_time)
                {
                    // 正常情况：当前时间大于等于上次在线时间
                    time_diff = current_time - last_time;
                }
                else
                {
                    // 溢出情况：当前时间小于上次在线时间（时间戳已溢出）
                    // 计算跨越溢出点的时间差
                    time_diff = (UINT32_MAX - last_time) + current_time + 1;
                }

                // 检查是否超过离线超时时间
                if (time_diff >= MODULE_OFFLINE_TIME)
                {
                    online_info[i].is_online = false;
                }
            }
        }
    }
}