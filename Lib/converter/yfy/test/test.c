#include "yfy_data.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>


// 外部声明全局数据结构
extern module_data_t stModuleData;
extern group_module_data_t stGroupModuleData;
extern sys_module_data_t stSysModuleData;

// 测试辅助函数
void print_test_result(const char* test_name, bool result) {
    printf("[%s] %s: %s\n", result ? "PASS" : "FAIL", test_name, result ? "SUCCESS" : "FAILED");
}

void clear_test_data() {
    memset(&stModuleData, 0, sizeof(stModuleData));
    memset(&stGroupModuleData, 0, sizeof(stGroupModuleData));
    memset(&stSysModuleData, 0, sizeof(stSysModuleData));
}

// 测试1: 单个模块电压数据解析 (cmd=0x03)
void test_module_voltage_parse() {
    printf("\n=== 测试1: 单个模块电压数据解析 ===\n");
    clear_test_data();

    // 模拟电压数据: 48.5V (IEEE 754 float格式)
    // 48.5 = 0x42420000 (big endian) = {0x42, 0x42, 0x00, 0x00}
    uint8_t voltage_data[8] = {0x42, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    bool result = yfy_data_parse(DEVICE_ID, 0x03, 0, voltage_data);

    // 检查解析结果
    float expected_voltage = 48.5f;
    float actual_voltage = stModuleData.voltage[0];

    printf("期望电压: %.2f V\n", expected_voltage);
    printf("实际电压: %.2f V\n", actual_voltage);

    bool test_passed = (actual_voltage > 48.4f && actual_voltage < 48.6f);
    print_test_result("模块电压解析", test_passed);
}

// 测试2: 单个模块电流数据解析 (cmd=0x03)
void test_module_current_parse() {
    printf("\n=== 测试2: 单个模块电流数据解析 ===\n");
    clear_test_data();

    // 模拟电流数据: 前4字节为电压(忽略), 后4字节为电流 12.5A
    // 12.5 = 0x41480000 (big endian)
    uint8_t current_data[8] = {0x00, 0x00, 0x00, 0x00, 0x41, 0x48, 0x00, 0x00};

    bool result = yfy_data_parse(DEVICE_ID, 0x03, 1, current_data);

    float expected_current = 12.5f;
    float actual_current = stModuleData.current[1];

    printf("期望电流: %.2f A\n", expected_current);
    printf("实际电流: %.2f A\n", actual_current);

    bool test_passed = (actual_current > 12.4f && actual_current < 12.6f);
    print_test_result("模块电流解析", test_passed);
}

// 测试3: 模块状态数据解析 (cmd=0x04)
void test_module_status_parse() {
    printf("\n=== 测试3: 模块状态数据解析 ===\n");
    clear_test_data();

    // 模拟状态数据
    uint8_t status_data[8] = {
        0x00, 0x00,     // 前2字节
        0x02,           // byte2: 组号 = 2
        0x00,           // byte3
        0x1E,           // byte4: 温度 = 30°C
        0x85,           // byte5: 状态位 10000101 (bit0=1限功率, bit2=1严重不均流, bit7=1 PFC故障)
        0x42,           // byte6: 状态位 01000010 (bit1=1模块故障, bit6=1 WALK-IN使能)
        0x11            // byte7: 状态位 00010001 (bit0=1输出短路, bit4=1模块放电异常)
    };

    bool result = yfy_data_parse(DEVICE_ID, 0x04, 2, status_data);

    // 检查解析结果
    printf("模块2组号: %d (期望: 2)\n", stModuleData.group[2]);
    printf("模块2温度: %d°C (期望: 30)\n", stModuleData.temp[2]);
    printf("模块2限功率状态: %d (期望: 1)\n", stModuleData.module_limit_power[2]);
    printf("模块2严重不均流: %d (期望: 1)\n", stModuleData.module_severe_unbalanced[2]);
    printf("模块2 PFC故障: %d (期望: 1)\n", stModuleData.module_pfc_fault[2]);
    printf("模块2模块故障: %d (期望: 1)\n", stModuleData.module_fault[2]);
    printf("模块2输出短路: %d (期望: 1)\n", stModuleData.output_short_circuit[2]);

    bool test_passed = (stModuleData.group[2] == 2) &&
                      (stModuleData.temp[2] == 30) &&
                      (stModuleData.module_limit_power[2] == 1) &&
                      (stModuleData.module_severe_unbalanced[2] == 1) &&
                      (stModuleData.module_pfc_fault[2] == 1);

    print_test_result("模块状态解析", test_passed);
}

// 测试4: 组数据解析 (cmd=0x01)
void test_group_data_parse() {
    printf("\n=== 测试4: 组数据解析 ===\n");
    clear_test_data();

    // 模拟组电压数据: 48.0V
    // 48.0 = 0x42400000
    uint8_t group_voltage_data[8] = {0x42, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    bool result = yfy_data_parse(GROUP_DEVICE_ID, 0x01, 3, group_voltage_data);

    float expected_voltage = 48.0f;
    float actual_voltage = stGroupModuleData.voltage[2]; // 组号从1开始，所以组3对应索引2

    printf("组3电压: %.2f V (期望: %.2f V)\n", actual_voltage, expected_voltage);

    bool test_passed = (actual_voltage > 47.9f && actual_voltage < 48.1f);
    print_test_result("组数据解析", test_passed);
}

// 测试5: 系统数据解析 (cmd=0x01, 广播地址)
void test_system_data_parse() {
    printf("\n=== 测试5: 系统数据解析 ===\n");
    clear_test_data();

    // 模拟系统电压数据: 480.0V
    // 480.0 = 0x43F00000
    uint8_t sys_voltage_data[8] = {0x43, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    bool result = yfy_data_parse(DEVICE_ID, 0x01, BROADCAST_ADDR, sys_voltage_data);

    float expected_voltage = 480.0f;
    float actual_voltage = stSysModuleData.voltage;

    printf("系统电压: %.2f V (期望: %.2f V)\n", actual_voltage, expected_voltage);

    bool test_passed = (actual_voltage > 479.9f && actual_voltage < 480.1f);
    print_test_result("系统数据解析", test_passed);
}

// 测试6: 错误情况测试
void test_error_cases() {
    printf("\n=== 测试6: 错误情况测试 ===\n");

    // 测试无效组号
    uint8_t dummy_data[8] = {0};
    bool result1 = yfy_data_parse(GROUP_DEVICE_ID, 0x01, GROUP_MODULE_NUM + 5, dummy_data);
    print_test_result("无效组号处理", !result1); // 应该返回false

    // 测试空指针
    bool result2 = yfy_data_parse(DEVICE_ID, 0x03, 0, NULL);
    print_test_result("空指针处理", !result2); // 应该返回false或崩溃保护
}

int main() {
    printf("=== YFY数据解析函数测试程序 ===\n");
    printf("模块数量: %d\n", MODULE_NUM);
    printf("组数量: %d\n", GROUP_MODULE_NUM);
    printf("广播地址: 0x%02X\n", BROADCAST_ADDR);
    printf("设备ID: 0x%02X\n", DEVICE_ID);
    printf("组设备ID: 0x%02X\n", GROUP_DEVICE_ID);

    // 运行所有测试
    test_module_voltage_parse();
    test_module_current_parse();
    test_module_status_parse();
    test_group_data_parse();
    test_system_data_parse();
    test_error_cases();

    printf("\n=== 测试完成 ===\n");
    return 0;
}