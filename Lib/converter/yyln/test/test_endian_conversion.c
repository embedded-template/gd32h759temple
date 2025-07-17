#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// 模拟YYLN相关定义
#define YYLN_MONITOR_ADDR 0x01

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

// 字节序转换宏
#define CONVERT_ENDIANNESS(src_data, dst_data, bytes)                                                                                                          \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
        if ((bytes) > 0)                                                                                                                                       \
        {                                                                                                                                                      \
            for (uint8_t i = 0; i < (bytes); i++)                                                                                                              \
            {                                                                                                                                                  \
                (dst_data)[i] = (src_data)[(bytes) - 1 - i];                                                                                                   \
            }                                                                                                                                                  \
        }                                                                                                                                                      \
    } while (0)

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
    printf("    byte0 (group|type): 0x%02X (group=%d, type=%d)\n", pdata[0], (pdata[0] >> 4) & 0x0F, pdata[0] & 0x0F);
    printf("    byte1 (命令): 0x%02X\n", pdata[1]);
    printf("    byte2-3 (保留): 0x%02X 0x%02X\n", pdata[2], pdata[3]);
    printf("    byte4-7 (用户数据): 0x%02X 0x%02X 0x%02X 0x%02X\n", pdata[4], pdata[5], pdata[6], pdata[7]);

    // 将byte4-7组合成32位数据（Big Endian）
    uint32_t user_data = ((uint32_t) pdata[4] << 24) | ((uint32_t) pdata[5] << 16) | ((uint32_t) pdata[6] << 8) | ((uint32_t) pdata[7]);
    printf("    32位用户数据: 0x%08X (%u)\n", user_data, user_data);
    printf("\n");

    return true;
}

// 模拟yyln_send_write_cmd函数
void yyln_send_write_cmd(uint8_t module_addr, uint8_t group, yyln_message_type_e message_type, yyln_message_cmd_e message_cmd, uint32_t data)
{
    // 组装CAN ID
    uint32_t can_id = YYLN_SET_ID(0x01, YYLN_MONITOR_ADDR, module_addr, 0u, 0u);

    uint8_t send_data[8] = {0};
    // group占7:4, message_type占3:0
    send_data[0] = (group << 4) | (message_type & 0x0F);
    send_data[1] = (uint8_t) message_cmd;
    // byte2和byte3保留为0
    send_data[2] = 0x00;
    send_data[3] = 0x00;

    // 将32位数据转换为Big Endian格式并复制到byte4~byte7
    // 使用字节序转换宏将本地字节序转换为Big Endian（高位在低字节）
    uint8_t temp_data[4];
    CONVERT_ENDIANNESS((uint8_t*) &data, temp_data, 4);
    memcpy(&send_data[4], temp_data, 4);

    // 调用发送函数
    test_send(can_id, send_data);
}

void test_endian_conversion(void)
{
    printf("=== 测试字节序转换功能 ===\n\n");

    // 测试1: 设置电压参考值为5000mV
    printf("测试1: 设置模块1电压参考值为5000mV\n");
    printf("原始数据: 5000 (0x1388)\n");
    yyln_send_write_cmd(0x01, 0x00, yyln_SetData, yyln_VoutReference, 5000);

    // 测试2: 设置电流限制为2000mA
    printf("测试2: 设置模块2电流限制为2000mA\n");
    printf("原始数据: 2000 (0x07D0)\n");
    yyln_send_write_cmd(0x02, 0x01, yyln_SetData, yyln_IoutLimit, 2000);

    // 测试3: 设置大数值 100000
    printf("测试3: 设置大数值100000\n");
    printf("原始数据: 100000 (0x186A0)\n");
    yyln_send_write_cmd(0x01, 0x02, yyln_SetData, yyln_VoutReference, 100000);

    // 测试4: 测试字节序转换的正确性
    printf("测试4: 验证字节序转换\n");
    uint32_t test_value = 0x12345678;
    uint8_t converted[4];
    CONVERT_ENDIANNESS((uint8_t*) &test_value, converted, 4);

    printf("原始32位数据: 0x%08X\n", test_value);
    printf("本地字节序: [0x%02X 0x%02X 0x%02X 0x%02X]\n", ((uint8_t*) &test_value)[0], ((uint8_t*) &test_value)[1], ((uint8_t*) &test_value)[2],
           ((uint8_t*) &test_value)[3]);
    printf("转换后(Big Endian): [0x%02X 0x%02X 0x%02X 0x%02X]\n", converted[0], converted[1], converted[2], converted[3]);

    // 验证转换结果
    if (converted[0] == 0x12 && converted[1] == 0x34 && converted[2] == 0x56 && converted[3] == 0x78)
    {
        printf("✓ 字节序转换正确！\n");
    }
    else
    {
        printf("✗ 字节序转换错误！\n");
    }
}

void test_usage_examples(void)
{
    printf("\n=== 使用示例 ===\n\n");

    printf("现在调用者可以直接传入原始数值，无需手动转换字节序:\n\n");

    printf("// 设置模块1电压为5000mV\n");
    printf("yyln_send_write_cmd(1, 0, yyln_SetData, yyln_VoutReference, 5000);\n\n");

    printf("// 设置模块2电流限制为3000mA\n");
    printf("yyln_send_write_cmd(2, 1, yyln_SetData, yyln_IoutLimit, 3000);\n\n");

    printf("优势:\n");
    printf("1. 调用者无需关心字节序问题\n");
    printf("2. 代码更简洁，直接传入数值\n");
    printf("3. 减少出错可能性\n");
    printf("4. 统一在底层函数处理字节序转换\n");
}

int main(void)
{
    printf("YYLN协议字节序转换测试\n");
    printf("======================\n\n");

    test_endian_conversion();
    test_usage_examples();

    printf("\n✓ 所有测试完成！\n");
    printf("字节序转换已集成到yyln_send_write_cmd函数中\n");

    return 0;
}
