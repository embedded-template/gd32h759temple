#include "../yfy_interface.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

// 外部函数声明
extern module_data_t* get_module_data(void);
extern group_module_data_t* get_group_module_data(void);
extern sys_module_data_t* get_sys_module_data(void);

// 测试统计
static int test_count = 0;
static int pass_count = 0;

// 测试宏
#define TEST_ASSERT(condition, test_name) do { \
    test_count++; \
    if (condition) { \
        pass_count++; \
        printf("[PASS] %s\n", test_name); \
    } else { \
        printf("[FAIL] %s\n", test_name); \
    } \
} while(0)

// 清空测试数据
void clear_all_data() {
    module_data_t* module_data = get_module_data();
    group_module_data_t* group_data = get_group_module_data();
    sys_module_data_t* sys_data = get_sys_module_data();
    
    memset(module_data, 0, sizeof(module_data_t));
    memset(group_data, 0, sizeof(group_module_data_t));
    memset(sys_data, 0, sizeof(sys_module_data_t));
}

// 设置测试数据
void setup_test_data() {
    module_data_t* module_data = get_module_data();
    group_module_data_t* group_data = get_group_module_data();
    sys_module_data_t* sys_data = get_sys_module_data();
    
    // 设置模块数据
    for (int i = 0; i < 3; i++) {
        module_data->voltage[i] = 48.0f + i;
        module_data->current[i] = 10.0f + i;
        module_data->group[i] = i + 1;
        module_data->temp[i] = 25 + i;
        module_data->module_limit_power[i] = i % 2;
        module_data->ac_input_voltage_ab[i] = 380 + i * 10;
        module_data->max_voltage[i] = 50 + i;
        module_data->addr_modeL[i] = i % 2;
    }
    
    // 设置组数据
    for (int i = 0; i < 3; i++) {
        group_data->voltage[i] = 48.0f + i * 0.1f;
        group_data->current[i] = 100.0f + i * 10;
        group_data->module_num[i] = 4 + i;
    }
    
    // 设置系统数据
    sys_data->voltage = 480.0f;
    sys_data->current = 1000.0f;
    sys_data->module_num = 12;
}

// 测试1：基本数据读取
void test_basic_data_read() {
    printf("\n=== 测试1：基本数据读取 ===\n");
    
    float voltage, current;
    uint8_t group;
    int8_t temp;
    
    // 测试模块0
    TEST_ASSERT(yfy_get_module_voltage(0, &voltage) == YFY_OK && voltage == 48.0f, "模块0电压读取");
    TEST_ASSERT(yfy_get_module_current(0, &current) == YFY_OK && current == 10.0f, "模块0电流读取");
    TEST_ASSERT(yfy_get_module_group(0, &group) == YFY_OK && group == 1, "模块0组号读取");
    TEST_ASSERT(yfy_get_module_temp(0, &temp) == YFY_OK && temp == 25, "模块0温度读取");
    
    // 测试边界检查
    TEST_ASSERT(yfy_get_module_voltage(MODULE_NUM, &voltage) == YFY_ERROR_INVALID_ADDR, "无效地址检查");
    TEST_ASSERT(yfy_get_module_voltage(0, NULL) == YFY_ERROR_NULL_POINTER, "空指针检查");
}

// 测试2：状态位读取
void test_status_read() {
    printf("\n=== 测试2：状态位读取 ===\n");
    
    uint8_t status;
    
    // 测试状态表2
    TEST_ASSERT(yfy_get_module_limit_power(0, &status) == YFY_OK && status == 0, "限功率状态读取");
    TEST_ASSERT(yfy_get_module_limit_power(1, &status) == YFY_OK && status == 1, "限功率状态读取(模块1)");
    
    // 测试状态表1
    TEST_ASSERT(yfy_get_module_dc_side_fault(0, &status) == YFY_OK, "DC侧故障状态读取");
    TEST_ASSERT(yfy_get_fan_fault(0, &status) == YFY_OK, "风扇故障状态读取");
    
    // 测试状态表0
    TEST_ASSERT(yfy_get_output_short_circuit(0, &status) == YFY_OK, "输出短路状态读取");
    TEST_ASSERT(yfy_get_module_sleep(0, &status) == YFY_OK, "模块休眠状态读取");
}

// 测试3：电压电流读取
void test_voltage_current_read() {
    printf("\n=== 测试3：电压电流读取 ===\n");
    
    uint16_t voltage, current;
    
    // 测试交流输入电压
    TEST_ASSERT(yfy_get_ac_input_voltage_ab(0, &voltage) == YFY_OK && voltage == 380, "AB相电压读取");
    TEST_ASSERT(yfy_get_ac_input_voltage_ab(1, &voltage) == YFY_OK && voltage == 390, "AB相电压读取(模块1)");
    
    // 测试最大最小值
    TEST_ASSERT(yfy_get_max_voltage(0, &voltage) == YFY_OK && voltage == 50, "最大电压读取");
    TEST_ASSERT(yfy_get_max_voltage(1, &voltage) == YFY_OK && voltage == 51, "最大电压读取(模块1)");
    
    // 测试其他参数
    TEST_ASSERT(yfy_get_external_voltage(0, &voltage) == YFY_OK, "外部电压读取");
    TEST_ASSERT(yfy_get_max_output_current(0, &current) == YFY_OK, "最大输出电流读取");
    
    uint8_t addr_mode;
    TEST_ASSERT(yfy_get_module_addr_mode(0, &addr_mode) == YFY_OK && addr_mode == 0, "地址模式读取");
    TEST_ASSERT(yfy_get_module_addr_mode(1, &addr_mode) == YFY_OK && addr_mode == 1, "地址模式读取(模块1)");
}

// 测试4：组数据读取
void test_group_data_read() {
    printf("\n=== 测试4：组数据读取 ===\n");
    
    float voltage, current;
    uint8_t module_num;
    
    // 测试组1数据
    TEST_ASSERT(yfy_get_group_voltage(1, &voltage) == YFY_OK && voltage == 48.0f, "组1电压读取");
    TEST_ASSERT(yfy_get_group_current(1, &current) == YFY_OK && current == 100.0f, "组1电流读取");
    TEST_ASSERT(yfy_get_group_module_num(1, &module_num) == YFY_OK && module_num == 4, "组1模块数读取");
    
    // 测试组2数据
    TEST_ASSERT(yfy_get_group_voltage(2, &voltage) == YFY_OK && voltage == 48.1f, "组2电压读取");
    TEST_ASSERT(yfy_get_group_current(2, &current) == YFY_OK && current == 110.0f, "组2电流读取");
    
    // 测试边界检查
    TEST_ASSERT(yfy_get_group_voltage(0, &voltage) == YFY_ERROR_INVALID_GROUP, "无效组号检查(0)");
    TEST_ASSERT(yfy_get_group_voltage(GROUP_MODULE_NUM + 1, &voltage) == YFY_ERROR_INVALID_GROUP, "无效组号检查(超限)");
    TEST_ASSERT(yfy_get_group_voltage(1, NULL) == YFY_ERROR_NULL_POINTER, "组数据空指针检查");
}

// 测试5：系统数据读取
void test_system_data_read() {
    printf("\n=== 测试5：系统数据读取 ===\n");
    
    float voltage, current;
    uint8_t module_num;
    
    TEST_ASSERT(yfy_get_sys_voltage(&voltage) == YFY_OK && voltage == 480.0f, "系统电压读取");
    TEST_ASSERT(yfy_get_sys_current(&current) == YFY_OK && current == 1000.0f, "系统电流读取");
    TEST_ASSERT(yfy_get_sys_module_num(&module_num) == YFY_OK && module_num == 12, "系统模块数读取");
    
    // 测试空指针检查
    TEST_ASSERT(yfy_get_sys_voltage(NULL) == YFY_ERROR_NULL_POINTER, "系统数据空指针检查");
}

// 测试6：32位数据组装宏
void test_32bit_macro() {
    printf("\n=== 测试6：32位数据组装宏 ===\n");
    
    // 测试基本组装
    uint32_t data = YFY_SET_ID(0x02, 0x0A, 0x03, 0x05, 0xF0);
    uint32_t expected = 0x0A8305F0;
    TEST_ASSERT(data == expected, "32位数据组装");
    
    // 测试数据提取
    TEST_ASSERT(YFY_EXTRACT_ERROR_CODE(data) == 0x02, "错误码提取");
    TEST_ASSERT(YFY_EXTRACT_DEVICE_ID(data) == 0x0A, "设备号提取");
    TEST_ASSERT(YFY_EXTRACT_CMD(data) == 0x03, "命令号提取");
    TEST_ASSERT(YFY_EXTRACT_DEST_ADDR(data) == 0x05, "目的地址提取");
    TEST_ASSERT(YFY_EXTRACT_SRC_ADDR(data) == 0xF0, "源地址提取");
    
    // 测试边界值
    uint32_t max_data = YFY_SET_ID(0x07, 0x0F, 0x3F, 0xFF, 0xFF);
    TEST_ASSERT(max_data == 0x1FFFFFFF, "最大值组装");
    
    uint32_t min_data = YFY_SET_ID(0x00, 0x00, 0x00, 0x00, 0x00);
    TEST_ASSERT(min_data == 0x00000000, "最小值组装");
}

// 主测试函数
int main() {
    printf("=== YFY模块全面单元测试 ===\n");
    printf("模块数量: %d, 组数量: %d\n", MODULE_NUM, GROUP_MODULE_NUM);
    
    // 初始化测试数据
    clear_all_data();
    setup_test_data();
    
    // 运行所有测试
    test_basic_data_read();
    test_status_read();
    test_voltage_current_read();
    test_group_data_read();
    test_system_data_read();
    test_32bit_macro();
    
    // 输出测试结果
    printf("\n=== 测试结果统计 ===\n");
    printf("总测试数: %d\n", test_count);
    printf("通过数: %d\n", pass_count);
    printf("失败数: %d\n", test_count - pass_count);
    printf("通过率: %.1f%%\n", (float)pass_count / test_count * 100);
    
    if (pass_count == test_count) {
        printf("\n🎉 所有测试通过！\n");
        return 0;
    } else {
        printf("\n❌ 有测试失败！\n");
        return 1;
    }
}
