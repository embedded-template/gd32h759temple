#include "../yfy_interface.h"
#include <stdio.h>
#include <string.h>

// 外部函数声明
extern module_data_t* get_module_data(void);
extern group_module_data_t* get_group_module_data(void);
extern sys_module_data_t* get_sys_module_data(void);

// 简洁的测试宏
#define RUN_TEST(test_func)                                                                                                                                    \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
        printf("运行 %s... ", #test_func);                                                                                                                     \
        if (test_func())                                                                                                                                       \
        {                                                                                                                                                      \
            printf("✓ 通过\n");                                                                                                                                \
            passed++;                                                                                                                                          \
        }                                                                                                                                                      \
        else                                                                                                                                                   \
        {                                                                                                                                                      \
            printf("✗ 失败\n");                                                                                                                                \
            failed++;                                                                                                                                          \
        }                                                                                                                                                      \
        total++;                                                                                                                                               \
    } while (0)

static int total = 0, passed = 0, failed = 0;

// 初始化测试数据
void init_test_data()
{
    module_data_t* module_data = get_module_data();
    group_module_data_t* group_data = get_group_module_data();
    sys_module_data_t* sys_data = get_sys_module_data();

    // 清零
    memset(module_data, 0, sizeof(module_data_t));
    memset(group_data, 0, sizeof(group_module_data_t));
    memset(sys_data, 0, sizeof(sys_module_data_t));

    // 设置标准测试数据
    module_data->voltage[0] = 48.0f;
    module_data->current[0] = 10.0f;
    module_data->group[0] = 1;
    module_data->temp[0] = 25;
    module_data->module_limit_power[0] = 0;
    module_data->ac_input_voltage_ab[0] = 380;
    module_data->max_voltage[0] = 50;
    module_data->addr_modeL[0] = 0;

    group_data->voltage[0] = 48.0f;
    group_data->current[0] = 100.0f;
    group_data->module_num[0] = 4;

    sys_data->voltage = 480.0f;
    sys_data->current = 1000.0f;
    sys_data->module_num = 12;
}

// 测试1：基本功能
bool test_basic_functions()
{
    float voltage;
    uint8_t group;

    return (yfy_get_module_voltage(0, &voltage) == YFY_OK && voltage == 48.0f) && (yfy_get_module_group(0, &group) == YFY_OK && group == 1) &&
           (yfy_get_module_voltage(MODULE_NUM, &voltage) == YFY_ERROR_INVALID_ADDR) && (yfy_get_module_voltage(0, NULL) == YFY_ERROR_NULL_POINTER);
}

// 测试2：状态读取
bool test_status_functions()
{
    uint8_t status;

    return (yfy_get_module_limit_power(0, &status) == YFY_OK) && (yfy_get_module_dc_side_fault(0, &status) == YFY_OK) &&
           (yfy_get_output_short_circuit(0, &status) == YFY_OK) && (yfy_get_module_limit_power(MODULE_NUM, &status) == YFY_ERROR_INVALID_ADDR);
}

// 测试3：电压电流
bool test_voltage_current()
{
    uint16_t voltage;
    uint8_t addr_mode;

    return (yfy_get_ac_input_voltage_ab(0, &voltage) == YFY_OK && voltage == 380) && (yfy_get_max_voltage(0, &voltage) == YFY_OK && voltage == 50) &&
           (yfy_get_module_addr_mode(0, &addr_mode) == YFY_OK && addr_mode == 0) &&
           (yfy_get_ac_input_voltage_ab(MODULE_NUM, &voltage) == YFY_ERROR_INVALID_ADDR);
}

// 测试4：组数据
bool test_group_functions()
{
    float voltage, current;
    uint8_t module_num;

    return (yfy_get_group_voltage(1, &voltage) == YFY_OK && voltage == 48.0f) && (yfy_get_group_current(1, &current) == YFY_OK && current == 100.0f) &&
           (yfy_get_group_module_num(1, &module_num) == YFY_OK && module_num == 4) && (yfy_get_group_voltage(0, &voltage) == YFY_ERROR_INVALID_GROUP) &&
           (yfy_get_group_voltage(GROUP_MODULE_NUM + 1, &voltage) == YFY_ERROR_INVALID_GROUP);
}

// 测试5：系统数据
bool test_system_functions()
{
    float voltage, current;
    uint8_t module_num;

    return (yfy_get_sys_voltage(&voltage) == YFY_OK && voltage == 480.0f) && (yfy_get_sys_current(&current) == YFY_OK && current == 1000.0f) &&
           (yfy_get_sys_module_num(&module_num) == YFY_OK && module_num == 12) && (yfy_get_sys_voltage(NULL) == YFY_ERROR_NULL_POINTER);
}

// 测试6：32位宏
bool test_32bit_macros()
{
    uint32_t data = YFY_SET_ID(0x02, 0x0A, 0x03, 0x05, 0xF0);

    return (data == 0x0A8305F0) && (YFY_EXTRACT_ERROR_CODE(data) == 0x02) && (YFY_EXTRACT_DEVICE_ID(data) == 0x0A) && (YFY_EXTRACT_CMD(data) == 0x03) &&
           (YFY_EXTRACT_DEST_ADDR(data) == 0x05) && (YFY_EXTRACT_SRC_ADDR(data) == 0xF0);
}

// 测试7：边界条件
bool test_boundary_conditions()
{
    float voltage;
    uint8_t status;

    // 测试所有模块地址
    for (int i = 0; i < MODULE_NUM; i++)
    {
        if (yfy_get_module_voltage(i, &voltage) != YFY_OK) return false;
    }

    // 测试所有组号
    for (int i = 1; i <= GROUP_MODULE_NUM; i++)
    {
        if (yfy_get_group_voltage(i, &voltage) != YFY_OK) return false;
    }

    // 测试所有状态函数（采样）
    return (yfy_get_module_limit_power(0, &status) == YFY_OK) && (yfy_get_module_id_duplicate(0, &status) == YFY_OK) &&
           (yfy_get_module_severe_unbalanced(0, &status) == YFY_OK) && (yfy_get_three_phase_input_missing(0, &status) == YFY_OK) &&
           (yfy_get_three_phase_input_unbalanced(0, &status) == YFY_OK) && (yfy_get_input_undervoltage(0, &status) == YFY_OK) &&
           (yfy_get_input_overvoltage(0, &status) == YFY_OK) && (yfy_get_module_pfc_fault(0, &status) == YFY_OK);
}

// 测试8：数据类型覆盖
bool test_data_types()
{
    float f_val;
    uint8_t u8_val;
    uint16_t u16_val;
    int8_t i8_val;

    return (yfy_get_module_voltage(0, &f_val) == YFY_OK) &&        // float
           (yfy_get_module_current(0, &f_val) == YFY_OK) &&        // float
           (yfy_get_module_group(0, &u8_val) == YFY_OK) &&         // uint8_t
           (yfy_get_module_temp(0, &i8_val) == YFY_OK) &&          // int8_t
           (yfy_get_ac_input_voltage_ab(0, &u16_val) == YFY_OK) && // uint16_t
           (yfy_get_max_voltage(0, &u16_val) == YFY_OK) &&         // uint16_t
           (yfy_get_group_voltage(1, &f_val) == YFY_OK) &&         // float
           (yfy_get_sys_voltage(&f_val) == YFY_OK);                // float
}

// 快速冒烟测试
bool quick_smoke_test()
{
    float voltage;
    uint8_t status;

    // 只测试最关键的几个函数
    return (yfy_get_module_voltage(0, &voltage) == YFY_OK) && (yfy_get_group_voltage(1, &voltage) == YFY_OK) && (yfy_get_sys_voltage(&voltage) == YFY_OK) &&
           (yfy_get_module_limit_power(0, &status) == YFY_OK) && (YFY_SET_ID(0, 1, 2, 3, 4) != 0);
}

int main(int argc, char* argv[])
{
    printf("=== YFY模块测试套件 ===\n");

    // 初始化测试数据
    init_test_data();

    // 检查是否是快速测试模式
    if (argc > 1 && strcmp(argv[1], "quick") == 0)
    {
        printf("运行快速冒烟测试...\n");
        RUN_TEST(quick_smoke_test);
    }
    else
    {
        printf("运行完整测试套件...\n");

        RUN_TEST(test_basic_functions);
        RUN_TEST(test_status_functions);
        RUN_TEST(test_voltage_current);
        RUN_TEST(test_group_functions);
        RUN_TEST(test_system_functions);
        RUN_TEST(test_32bit_macros);
        RUN_TEST(test_boundary_conditions);
        RUN_TEST(test_data_types);
    }

    // 输出结果
    printf("\n=== 测试结果 ===\n");
    printf("总计: %d, 通过: %d, 失败: %d\n", total, passed, failed);
    printf("成功率: %.1f%%\n", (float) passed / total * 100);

    if (failed == 0)
    {
        printf("🎉 所有测试通过！\n");
        return 0;
    }
    else
    {
        printf("❌ 有 %d 个测试失败！\n", failed);
        return 1;
    }
}
