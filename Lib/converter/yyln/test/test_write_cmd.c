#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// 模拟YYLN相关定义
typedef enum yyln_message_type_e
{
    yyln_SetData = 0u,
    yyln_SetDataResponse = 1u,
    yyln_ReadData = 2u,
    yyln_ReadDataResponse = 3u,
} yyln_message_type_e;

typedef enum yyln_message_cmd_e
{
    yyln_Vout = 0u,
    yyln_VoutReference = 2u,
    yyln_IoutLimit = 3u,
} yyln_message_cmd_e;

// ID宏定义
#define YYLN_SET_ID(protocol, monitor_addr, module_addr, production_day, serial_low)                                                                           \
    ((uint32_t) (((protocol) & 0x0F) << 25) | (uint32_t) (((monitor_addr) & 0x0F) << 21) | (uint32_t) (((module_addr) & 0x7F) << 14) |                         \
     (uint32_t) (((production_day) & 0x1F) << 9) | (uint32_t) ((serial_low) & 0x1FF))

// 模拟发送函数
bool test_send(uint32_t id, uint8_t* pdata)
{
    printf("发送CAN消息:\n");
    printf("  ID: 0x%08X\n", id);
    printf("  数据: [");
    for (int i = 0; i < 8; i++)
    {
        printf("0x%02X", pdata[i]);
        if (i < 7) printf(" ");
    }
    printf("]\n");

    // 解析数据包结构
    printf("  解析:\n");
    printf("    byte0 (消息类型): 0x%02X\n", pdata[0]);
    printf("    byte1 (消息命令): 0x%02X\n", pdata[1]);
    printf("    byte2-3 (保留): 0x%02X 0x%02X\n", pdata[2], pdata[3]);
    printf("    byte4-7 (用户数据): 0x%02X 0x%02X 0x%02X 0x%02X\n", pdata[4], pdata[5], pdata[6], pdata[7]);

    // 将byte4-7组合成32位数据（Big Endian）
    uint32_t user_data = ((uint32_t) pdata[4] << 24) | ((uint32_t) pdata[5] << 16) | ((uint32_t) pdata[6] << 8) | ((uint32_t) pdata[7]);
    printf("    32位用户数据: 0x%08X (%u)\n", user_data, user_data);
    printf("\n");

    return true;
}

// 模拟yyln_send_write_cmd函数
void yyln_send_write_cmd(uint8_t monitor_addr, uint8_t module_addr, yyln_message_type_e message_type, yyln_message_cmd_e message_cmd, uint8_t production_day,
                         uint16_t serial_low, uint8_t* pdata)
{
    // 组装CAN ID
    uint32_t can_id = YYLN_SET_ID(0x01, monitor_addr, module_addr, production_day, serial_low);

    uint8_t send_data[8] = {0};
    send_data[0] = (uint8_t) message_type;
    send_data[1] = (uint8_t) message_cmd;
    // byte2和byte3保留为0
    send_data[2] = 0x00;
    send_data[3] = 0x00;

    // 复制用户数据到byte4~byte7（4字节用户设置数据，高位在低字节）
    if (pdata != NULL)
    {
        // 直接复制4字节数据到byte4~byte7，保持Big Endian格式（高位在低字节）
        // 例如：32位数据0x12345678 -> byte4=0x12, byte5=0x34, byte6=0x56, byte7=0x78
        memcpy(&send_data[4], pdata, 4);
    }

    // 调用发送函数
    test_send(can_id, send_data);
}

void test_write_commands(void)
{
    printf("=== 测试YYLN写入命令格式 ===\n\n");

    // 测试1: 设置电压参考值为5000mV (0x1388)
    printf("测试1: 设置模块1电压参考值为5000mV\n");
    uint8_t voltage_data[4] = {0x00, 0x00, 0x13, 0x88}; // Big Endian: 5000
    yyln_send_write_cmd(0x01, 0x01, yyln_SetData, yyln_VoutReference, 15, 100, voltage_data);

    // 测试2: 设置电流限制为2000mA (0x07D0)
    printf("测试2: 设置模块2电流限制为2000mA\n");
    uint8_t current_data[4] = {0x00, 0x00, 0x07, 0xD0}; // Big Endian: 2000
    yyln_send_write_cmd(0x01, 0x02, yyln_SetData, yyln_IoutLimit, 15, 101, current_data);

    // 测试3: 设置大数值 100000 (0x186A0)
    printf("测试3: 设置大数值100000\n");
    uint8_t big_data[4] = {0x00, 0x01, 0x86, 0xA0}; // Big Endian: 100000
    yyln_send_write_cmd(0x01, 0x01, yyln_SetData, yyln_VoutReference, 15, 100, big_data);
}

void test_data_format_explanation(void)
{
    printf("=== 数据格式说明 ===\n\n");

    printf("YYLN写入命令数据包格式:\n");
    printf("  byte0: 消息类型 (message_type)\n");
    printf("  byte1: 消息命令 (message_cmd)\n");
    printf("  byte2: 保留 (0x00)\n");
    printf("  byte3: 保留 (0x00)\n");
    printf("  byte4: 用户数据高字节 (MSB)\n");
    printf("  byte5: 用户数据次高字节\n");
    printf("  byte6: 用户数据次低字节\n");
    printf("  byte7: 用户数据低字节 (LSB)\n\n");

    printf("Big Endian格式示例:\n");
    printf("  32位数据: 0x12345678\n");
    printf("  byte4 = 0x12 (最高字节)\n");
    printf("  byte5 = 0x34\n");
    printf("  byte6 = 0x56\n");
    printf("  byte7 = 0x78 (最低字节)\n\n");
}

int main(void)
{
    printf("YYLN协议写入命令测试\n");
    printf("====================\n\n");

    test_data_format_explanation();
    test_write_commands();

    printf("✓ 所有测试完成！\n");
    printf("注意: 用户数据位于byte4~byte7，采用Big Endian格式（高位在低字节）\n");

    return 0;
}
