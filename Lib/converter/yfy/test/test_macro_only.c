#include "../yfy_data.h"
#include <stdio.h>

// 测试32位数据组装宏
void test_32bit_data_assembly()
{
    printf("=== 32位数据组装宏测试 ===\n");

    // 测试数据组装
    uint8_t error_code = 0x02; // 3 bits: 010
    uint8_t device_id = 0x0A;  // 4 bits: 1010 (DEVICE_ID)
    uint8_t cmd = 0x03;        // 6 bits: 000011
    uint8_t dest_addr = 0x05;  // 8 bits: 00000101
    uint8_t src_addr = 0xF0;   // 8 bits: 11110000 (MONITOR_ADDR)

    // 组装32位数据
    uint32_t assembled_data = YFY_SET_ID(error_code, device_id, cmd, dest_addr, src_addr);

    // 期望结果计算：
    // [28:26] = 010 (0x02) -> 0x08000000
    // [25:22] = 1010 (0x0A) -> 0x02800000
    // [21:16] = 000011 (0x03) -> 0x00030000
    // [15:8] = 00000101 (0x05) -> 0x00000500
    // [7:0] = 11110000 (0xF0) -> 0x000000F0
    uint32_t expected = 0x08000000 | 0x02800000 | 0x00030000 | 0x00000500 | 0x000000F0;

    printf("组装数据: 0x%08X (期望: 0x%08X)\n", assembled_data, expected);
    printf("测试结果: %s\n", (assembled_data == expected) ? "PASS" : "FAIL");

    // 测试数据提取
    uint8_t extracted_error = YFY_EXTRACT_ERROR_CODE(assembled_data);
    uint8_t extracted_device = YFY_EXTRACT_DEVICE_ID(assembled_data);
    uint8_t extracted_cmd = YFY_EXTRACT_CMD(assembled_data);
    uint8_t extracted_dest = YFY_EXTRACT_DEST_ADDR(assembled_data);
    uint8_t extracted_src = YFY_EXTRACT_SRC_ADDR(assembled_data);

    printf("\n--- 数据提取测试 ---\n");
    printf("提取错误码: 0x%02X (期望: 0x%02X) %s\n", extracted_error, error_code, (extracted_error == error_code) ? "PASS" : "FAIL");
    printf("提取设备号: 0x%02X (期望: 0x%02X) %s\n", extracted_device, device_id, (extracted_device == device_id) ? "PASS" : "FAIL");
    printf("提取命令号: 0x%02X (期望: 0x%02X) %s\n", extracted_cmd, cmd, (extracted_cmd == cmd) ? "PASS" : "FAIL");
    printf("提取目的地址: 0x%02X (期望: 0x%02X) %s\n", extracted_dest, dest_addr, (extracted_dest == dest_addr) ? "PASS" : "FAIL");
    printf("提取源地址: 0x%02X (期望: 0x%02X) %s\n", extracted_src, src_addr, (extracted_src == src_addr) ? "PASS" : "FAIL");

    // 测试边界值
    printf("\n--- 边界值测试 ---\n");

    // 测试最大值
    uint32_t max_data = YFY_SET_ID(0x07, 0x0F, 0x3F, 0xFF, 0xFF);
    uint32_t expected_max = 0x1FFFFFFF; // 所有位都是1（除了高3位）
    printf("最大值测试: 0x%08X (期望: 0x%08X) %s\n", max_data, expected_max, (max_data == expected_max) ? "PASS" : "FAIL");

    // 测试最小值
    uint32_t min_data = YFY_SET_ID(0x00, 0x00, 0x00, 0x00, 0x00);
    printf("最小值测试: 0x%08X (期望: 0x00000000) %s\n", min_data, (min_data == 0x00000000) ? "PASS" : "FAIL");

    // 测试实际应用场景
    printf("\n--- 实际应用场景测试 ---\n");

    // 模拟读取模块0电压命令
    uint32_t read_voltage_cmd = YFY_SET_ID(0, DEVICE_ID, 0x03, 0, MONITOR_ADDR);
    printf("读取模块0电压命令: 0x%08X\n", read_voltage_cmd);

    // 模拟读取组1数据命令
    uint32_t read_group_cmd = YFY_SET_ID(0, GROUP_DEVICE_ID, 0x01, 1, MONITOR_ADDR);
    printf("读取组1数据命令: 0x%08X\n", read_group_cmd);

    // 模拟广播命令
    uint32_t broadcast_cmd = YFY_SET_ID(0, DEVICE_ID, 0x01, BROADCAST_ADDR, MONITOR_ADDR);
    printf("广播命令: 0x%08X\n", broadcast_cmd);

    printf("\n=== 测试完成 ===\n");
}

int main()
{
    test_32bit_data_assembly();
    return 0;
}
