#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// 模拟定义
#define MODULE_NUM 2
#define MODULE_OFFLINE_TIME 5000

// 包含YYLN相关定义
typedef enum yyln_message_type_e
{
    yyln_SetData = 0u,
    yyln_SetDataResponse = 1u,
    yyln_ReadData = 2u,
    yyln_ReadDataResponse = 3u,
    yyln_ReadSerialNumberResponse = 4u,
    yyln_AllSetData = 5u,
    yyln_AllSetDataResponse = 6u,
} yyln_message_type_e;

typedef enum yyln_message_cmd_e
{
    yyln_Vout = 0u,
    yyln_Iout_slow = 1u,
    yyln_VoutReference = 2u,
    yyln_IoutLimit = 3u,
    yyln_ShutDownDCDC = 4u,
    yyln_ReadSN = 5u,
    yyln_ModuleStatus = 8u,
} yyln_message_cmd_e;

// ID宏定义
#define YYLN_SET_ID(protocol, monitor_addr, module_addr, production_day, serial_low)                                                                           \
    ((uint32_t) (((protocol) & 0x0F) << 25) | (uint32_t) (((monitor_addr) & 0x0F) << 21) | (uint32_t) (((module_addr) & 0x7F) << 14) |                         \
     (uint32_t) (((production_day) & 0x1F) << 9) | (uint32_t) ((serial_low) & 0x1FF))

#define YYLN_EXTRACT_PROTOCOL(id) (((id) >> 25) & 0x0F)
#define YYLN_EXTRACT_MONITOR_ADDR(id) (((id) >> 21) & 0x0F)
#define YYLN_EXTRACT_MODULE_ADDR(id) (((id) >> 14) & 0x7F)
#define YYLN_EXTRACT_PRODUCTION_DAY(id) (((id) >> 9) & 0x1F)
#define YYLN_EXTRACT_SERIAL_LOW(id) ((id) & 0x1FF)

// 模块在线信息
typedef struct yyln_module_online_info_t
{
    uint8_t module_addr;
    uint32_t last_online_time;
    bool is_online;
} yyln_module_online_info_t;

// 模拟全局变量
static yyln_module_online_info_t yyln_module_online_info[MODULE_NUM] = {0};
static uint32_t test_time = 0;

// 模拟数据解析函数的在线状态更新逻辑
bool simulate_data_parse(uint32_t id, uint8_t* pdata)
{
    // 从CAN ID中提取字段
    uint8_t protocol = YYLN_EXTRACT_PROTOCOL(id);
    uint8_t module_addr = YYLN_EXTRACT_MODULE_ADDR(id);

    // 检查协议号是否正确
    if (protocol != 0x01)
    {
        return false;
    }

    // 检查模块地址范围
    if (module_addr >= MODULE_NUM)
    {
        return false;
    }

    // 从数据包中提取消息类型
    yyln_message_type_e message_type = (yyln_message_type_e) pdata[0];

    // 抑制未使用变量警告
    (void)pdata[1]; // message_cmd 在此测试中不需要使用

    // 只有收到模块的响应消息才更新在线状态（不包括发送的命令）
    if (module_addr > 0 && module_addr < MODULE_NUM &&
        (message_type == yyln_SetDataResponse || message_type == yyln_ReadDataResponse || message_type == yyln_ReadSerialNumberResponse ||
         message_type == yyln_AllSetDataResponse))
    {
        printf("  -> 模块%d发送响应消息，更新在线状态\n", module_addr);
        yyln_module_online_info[module_addr].module_addr = module_addr;
        yyln_module_online_info[module_addr].last_online_time = test_time;
        yyln_module_online_info[module_addr].is_online = true;
    }
    else if (message_type == yyln_SetData || message_type == yyln_ReadData || message_type == yyln_AllSetData)
    {
        printf("  -> 收到命令消息，不更新在线状态\n");
    }

    return true;
}

// 模拟在线检查函数
void simulate_online_check(void)
{
    static uint32_t last_check_time = 0;
    uint32_t current_time = test_time;
    uint32_t check_interval_diff;

    // 计算距离上次检查的时间差
    if (current_time >= last_check_time)
    {
        check_interval_diff = current_time - last_check_time;
    }
    else
    {
        check_interval_diff = (UINT32_MAX - last_check_time) + current_time + 1;
    }

    // 每1000ms执行一次超时检查
    if (check_interval_diff >= 1000)
    {
        last_check_time = current_time;
        printf("  -> 执行超时检查 (时间: %dms)\n", current_time);

        // 只检查已标记为在线的模块是否超时
        for (uint8_t i = 0; i < MODULE_NUM; i++)
        {
            if (yyln_module_online_info[i].is_online)
            {
                uint32_t last_time = yyln_module_online_info[i].last_online_time;
                uint32_t time_diff = current_time - last_time;

                printf("    模块%d: 上次在线时间=%dms, 时间差=%dms\n", i, last_time, time_diff);

                // 检查是否超过离线超时时间
                if (time_diff >= MODULE_OFFLINE_TIME)
                {
                    printf("    模块%d超时离线！\n", i);
                    yyln_module_online_info[i].is_online = false;
                }
            }
        }
    }
}

void print_online_status(void)
{
    printf("当前在线状态:\n");
    for (int i = 0; i < MODULE_NUM; i++)
    {
        printf("  模块%d: %s (上次在线: %dms)\n", i, yyln_module_online_info[i].is_online ? "在线" : "离线", yyln_module_online_info[i].last_online_time);
    }
    printf("\n");
}

void test_online_logic(void)
{
    printf("=== 测试YYLN在线状态逻辑 ===\n\n");

    // 初始状态
    printf("1. 初始状态 (时间: %dms)\n", test_time);
    print_online_status();

    // 模拟接收到模块1的响应消息
    printf("2. 接收模块1响应消息 (时间: %dms)\n", test_time);
    uint32_t response_id = YYLN_SET_ID(0x01, 0x01, 0x01, 15, 100);
    uint8_t response_data[8] = {yyln_ReadDataResponse, yyln_Vout, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
    simulate_data_parse(response_id, response_data);
    print_online_status();

    // 模拟接收到发送给模块的命令（不应该更新在线状态）
    printf("3. 接收发送命令消息 (时间: %dms)\n", test_time);
    uint32_t command_id = YYLN_SET_ID(0x01, 0x01, 0x01, 15, 100);
    uint8_t command_data[8] = {yyln_ReadData, yyln_Vout, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    simulate_data_parse(command_id, command_data);
    print_online_status();

    // 时间推进1秒，执行超时检查
    test_time += 1000;
    printf("4. 时间推进1秒，执行超时检查 (时间: %dms)\n", test_time);
    simulate_online_check();
    print_online_status();

    // 模拟接收到模块2的响应消息
    test_time += 500;
    printf("5. 接收模块2响应消息 (时间: %dms)\n", test_time);
    uint32_t response_id2 = YYLN_SET_ID(0x01, 0x01, 0x02, 15, 101);
    uint8_t response_data2[8] = {yyln_SetDataResponse, yyln_VoutReference, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00};
    simulate_data_parse(response_id2, response_data2);
    print_online_status();

    // 时间推进到超时时间
    test_time += MODULE_OFFLINE_TIME;
    printf("6. 时间推进到超时 (时间: %dms)\n", test_time);
    simulate_online_check();
    print_online_status();

    // 模块1重新上线
    test_time += 1000;
    printf("7. 模块1重新发送响应 (时间: %dms)\n", test_time);
    simulate_data_parse(response_id, response_data);
    print_online_status();

    printf("✓ 在线状态逻辑测试完成\n");
}

int main(void)
{
    printf("YYLN协议在线状态管理测试\n");
    printf("========================\n\n");

    test_online_logic();

    printf("\n总结:\n");
    printf("- 只有响应消息才会更新模块在线状态\n");
    printf("- 命令消息不会影响在线状态\n");
    printf("- 超时检查只会将在线模块标记为离线\n");
    printf("- 模块可以通过发送响应重新上线\n");

    return 0;
}
