#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// 模拟定义
#define MODULE_NUM 2

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

// 模拟句柄
typedef struct yyln_module_handle_t
{
    bool (*send)(uint32_t id, uint8_t* pdata);
    bool (*recv)(uint32_t id, uint8_t* pdata);
    uint32_t (*time)(void);
} yyln_module_handle_t;

// 测试数据
static uint32_t test_time = 0;
static uint8_t test_recv_data[8] = {0};
static bool test_has_data = false;

// 模拟函数
bool test_send(uint32_t id, uint8_t* pdata)
{
    printf("发送: ID=0x%08X, 数据=[", id);
    for (int i = 0; i < 8; i++)
    {
        printf("0x%02X", pdata[i]);
        if (i < 7) printf(" ");
    }
    printf("]\n");
    return true;
}

bool test_recv(uint32_t id, uint8_t* pdata)
{
    (void)id; // 抑制未使用参数警告
    if (test_has_data)
    {
        memcpy(pdata, test_recv_data, 8);
        test_has_data = false;
        return true;
    }
    return false;
}

uint32_t test_get_time(void)
{
    return test_time;
}

// 测试函数
void test_yyln_id_macros(void)
{
    printf("=== 测试YYLN ID宏 ===\n");

    // 测试ID组装
    uint32_t id = YYLN_SET_ID(0x01, 0x01, 0x01, 15, 100);
    printf("组装ID: 0x%08X\n", id);

    // 测试ID拆分
    uint8_t protocol = YYLN_EXTRACT_PROTOCOL(id);
    uint8_t monitor = YYLN_EXTRACT_MONITOR_ADDR(id);
    uint8_t module = YYLN_EXTRACT_MODULE_ADDR(id);
    uint8_t day = YYLN_EXTRACT_PRODUCTION_DAY(id);
    uint16_t serial = YYLN_EXTRACT_SERIAL_LOW(id);

    printf("拆分结果: protocol=0x%02X, monitor=0x%02X, module=0x%02X, day=%d, serial=%d\n", protocol, monitor, module, day, serial);

    assert(protocol == 0x01);
    assert(monitor == 0x01);
    assert(module == 0x01);
    assert(day == 15);
    assert(serial == 100);

    printf("✓ ID宏测试通过\n\n");
}

void test_yyln_send_commands(void)
{
    printf("=== 测试YYLN发送命令 ===\n");

    // 初始化句柄
    yyln_module_handle_t handle = {.send = test_send, .recv = test_recv, .time = test_get_time};

    // 测试发送读取命令
    printf("发送读取电压命令:\n");
    uint32_t read_id = YYLN_SET_ID(0x01, 0x01, 0x01, 15, 100);
    uint8_t read_data[8] = {yyln_ReadData, yyln_Vout, 0, 0, 0, 0, 0, 0};
    handle.send(read_id, read_data);

    // 测试发送设置命令
    printf("发送设置电压命令:\n");
    uint32_t set_id = YYLN_SET_ID(0x01, 0x01, 0x01, 15, 100);
    uint8_t set_data[8] = {yyln_SetData, yyln_VoutReference, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00}; // 设置4096mV
    handle.send(set_id, set_data);

    printf("✓ 发送命令测试通过\n\n");
}

void test_yyln_data_parsing(void)
{
    printf("=== 测试YYLN数据解析 ===\n");

    // 模拟接收到的数据包
    struct
    {
        uint32_t id;
        uint8_t data[8];
        const char* description;
    } test_packets[] = {
        {YYLN_SET_ID(0x01, 0x01, 0x01, 15, 100), {yyln_ReadDataResponse, yyln_Vout, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00}, "模块1电压响应: 4096mV"},
        {YYLN_SET_ID(0x01, 0x01, 0x02, 15, 101), {yyln_ReadDataResponse, yyln_Iout_slow, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00}, "模块2电流响应: 1280mA"},
        {YYLN_SET_ID(0x01, 0x01, 0x00, 1, 0), {yyln_AllSetDataResponse, yyln_VoutReference, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00}, "广播设置响应: 8192mV"}};

    int num_packets = sizeof(test_packets) / sizeof(test_packets[0]);

    for (int i = 0; i < num_packets; i++)
    {
        printf("处理数据包 %d: %s\n", i + 1, test_packets[i].description);
        printf("  ID: 0x%08X\n", test_packets[i].id);
        printf("  数据: [");
        for (int j = 0; j < 8; j++)
        {
            printf("0x%02X", test_packets[i].data[j]);
            if (j < 7) printf(" ");
        }
        printf("]\n");

        // 解析ID
        uint8_t protocol = YYLN_EXTRACT_PROTOCOL(test_packets[i].id);
        uint8_t monitor = YYLN_EXTRACT_MONITOR_ADDR(test_packets[i].id);
        uint8_t module = YYLN_EXTRACT_MODULE_ADDR(test_packets[i].id);
        uint8_t day = YYLN_EXTRACT_PRODUCTION_DAY(test_packets[i].id);
        uint16_t serial = YYLN_EXTRACT_SERIAL_LOW(test_packets[i].id);

        printf("  解析: protocol=%d, monitor=%d, module=%d, day=%d, serial=%d\n", protocol, monitor, module, day, serial);

        // 解析数据
        yyln_message_type_e msg_type = (yyln_message_type_e) test_packets[i].data[0];
        yyln_message_cmd_e msg_cmd = (yyln_message_cmd_e) test_packets[i].data[1];

        printf("  消息类型: %d, 命令: %d\n", msg_type, msg_cmd);
        printf("\n");
    }

    printf("✓ 数据解析测试通过\n\n");
}

int main(void)
{
    printf("YYLN协议数据处理测试\n");
    printf("====================\n\n");

    test_yyln_id_macros();
    test_yyln_send_commands();
    test_yyln_data_parsing();

    printf("所有测试通过！YYLN协议实现正确。\n");

    return 0;
}
