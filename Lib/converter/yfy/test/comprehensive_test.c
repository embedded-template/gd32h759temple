#include "../yfy_interface.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 外部函数声明
extern module_data_t* get_module_data(void);
extern group_module_data_t* get_group_module_data(void);
extern sys_module_data_t* get_sys_module_data(void);

// 测试统计
typedef struct
{
    int total;
    int passed;
    int failed;
    double duration_ms;
} test_stats_t;

static test_stats_t stats = {0};

// 高精度计时
static clock_t start_time;

#define TEST_START()                                                                                                                                           \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
        start_time = clock();                                                                                                                                  \
    } while (0)

#define TEST_END()                                                                                                                                             \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
        clock_t end_time = clock();                                                                                                                            \
        stats.duration_ms += ((double) (end_time - start_time) / CLOCKS_PER_SEC) * 1000;                                                                       \
    } while (0)

#define ASSERT_TEST(condition, name)                                                                                                                           \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
        TEST_START();                                                                                                                                          \
        stats.total++;                                                                                                                                         \
        if (condition)                                                                                                                                         \
        {                                                                                                                                                      \
            stats.passed++;                                                                                                                                    \
            printf("✓ %s\n", name);                                                                                                                            \
        }                                                                                                                                                      \
        else                                                                                                                                                   \
        {                                                                                                                                                      \
            stats.failed++;                                                                                                                                    \
            printf("✗ %s\n", name);                                                                                                                            \
        }                                                                                                                                                      \
        TEST_END();                                                                                                                                            \
    } while (0)

// 随机数据生成
void generate_random_data()
{
    module_data_t* module_data = get_module_data();
    group_module_data_t* group_data = get_group_module_data();
    sys_module_data_t* sys_data = get_sys_module_data();

    srand((unsigned int) time(NULL));

    // 生成随机模块数据
    for (int i = 0; i < MODULE_NUM; i++)
    {
        module_data->voltage[i] = 40.0f + (rand() % 200) / 10.0f; // 40-60V
        module_data->current[i] = (rand() % 500) / 10.0f;         // 0-50A
        module_data->group[i] = (rand() % GROUP_MODULE_NUM) + 1;  // 1-13
        module_data->temp[i] = 20 + (rand() % 60);                // 20-80°C

        // 随机状态位
        module_data->module_limit_power[i] = rand() % 2;
        module_data->module_id_duplicate[i] = rand() % 2;
        module_data->module_dc_side_fault[i] = rand() % 2;
        module_data->output_short_circuit[i] = rand() % 2;

        // 随机电压值
        module_data->ac_input_voltage_ab[i] = 350 + rand() % 100; // 350-450V
        module_data->max_voltage[i] = 45 + rand() % 20;           // 45-65V
        module_data->min_voltage[i] = 35 + rand() % 10;           // 35-45V
        module_data->addr_modeL[i] = rand() % 2;
    }

    // 生成随机组数据
    for (int i = 0; i < GROUP_MODULE_NUM; i++)
    {
        group_data->voltage[i] = 45.0f + (rand() % 100) / 100.0f;
        group_data->current[i] = 50.0f + (rand() % 1000) / 10.0f;
        group_data->module_num[i] = 1 + rand() % 8;
    }

    // 生成随机系统数据
    sys_data->voltage = 450.0f + (rand() % 100) / 10.0f;
    sys_data->current = 500.0f + (rand() % 2000) / 10.0f;
    sys_data->module_num = 8 + rand() % 8;
}

// 压力测试：大量读取操作
void stress_test_read_operations()
{
    printf("\n=== 压力测试：读取操作 ===\n");

    const int iterations = 10000;
    float voltage, current;
    uint8_t status, group;
    int8_t temp;
    uint16_t ac_voltage;

    TEST_START();

    // 大量基本数据读取
    for (int i = 0; i < iterations; i++)
    {
        uint8_t addr = i % MODULE_NUM;
        yfy_get_module_voltage(addr, &voltage);
        yfy_get_module_current(addr, &current);
        yfy_get_module_group(addr, &group);
        yfy_get_module_temp(addr, &temp);
    }

    // 大量状态读取
    for (int i = 0; i < iterations; i++)
    {
        uint8_t addr = i % MODULE_NUM;
        yfy_get_module_limit_power(addr, &status);
        yfy_get_module_dc_side_fault(addr, &status);
        yfy_get_output_short_circuit(addr, &status);
    }

    // 大量电压读取
    for (int i = 0; i < iterations; i++)
    {
        uint8_t addr = i % MODULE_NUM;
        yfy_get_ac_input_voltage_ab(addr, &ac_voltage);
        yfy_get_max_voltage(addr, &ac_voltage);
    }

    TEST_END();

    printf("完成 %d 次读取操作，耗时: %.2f ms\n", iterations * 9, stats.duration_ms);
    printf("平均每次读取: %.4f ms\n", stats.duration_ms / (iterations * 9));

    stats.total++;
    stats.passed++;
}

// 边界测试：测试所有边界条件
void boundary_test()
{
    printf("\n=== 边界测试 ===\n");

    float voltage;
    uint8_t status;

    // 测试模块地址边界
    ASSERT_TEST(yfy_get_module_voltage(0, &voltage) == true, "最小模块地址(0)");
    ASSERT_TEST(yfy_get_module_voltage(MODULE_NUM - 1, &voltage) == true, "最大模块地址");
    ASSERT_TEST(yfy_get_module_voltage(MODULE_NUM, &voltage) == false, "超出模块地址");
    ASSERT_TEST(yfy_get_module_voltage(255, &voltage) == false, "最大uint8地址");

    // 测试组号边界
    ASSERT_TEST(yfy_get_group_voltage(1, &voltage) == true, "最小组号(1)");
    ASSERT_TEST(yfy_get_group_voltage(GROUP_MODULE_NUM, &voltage) == true, "最大组号");
    ASSERT_TEST(yfy_get_group_voltage(0, &voltage) == false, "无效组号(0)");
    ASSERT_TEST(yfy_get_group_voltage(GROUP_MODULE_NUM + 1, &voltage) == false, "超出组号");

    // 测试空指针
    ASSERT_TEST(yfy_get_module_voltage(0, NULL) == false, "模块数据空指针");
    ASSERT_TEST(yfy_get_group_voltage(1, NULL) == false, "组数据空指针");
    ASSERT_TEST(yfy_get_sys_voltage(NULL) == false, "系统数据空指针");
}

// 数据一致性测试
void consistency_test()
{
    printf("\n=== 数据一致性测试 ===\n");

    // 设置已知数据
    module_data_t* module_data = get_module_data();
    module_data->voltage[0] = 48.5f;
    module_data->current[0] = 12.3f;
    module_data->group[0] = 5;

    float voltage1, voltage2, current1, current2;
    uint8_t group1, group2;

    // 多次读取同一数据，应该一致
    yfy_get_module_voltage(0, &voltage1);
    yfy_get_module_voltage(0, &voltage2);
    yfy_get_module_current(0, &current1);
    yfy_get_module_current(0, &current2);
    yfy_get_module_group(0, &group1);
    yfy_get_module_group(0, &group2);

    ASSERT_TEST(voltage1 == voltage2, "电压读取一致性");
    ASSERT_TEST(current1 == current2, "电流读取一致性");
    ASSERT_TEST(group1 == group2, "组号读取一致性");

    // 验证数据正确性
    ASSERT_TEST(voltage1 == 48.5f, "电压数据正确性");
    ASSERT_TEST(current1 == 12.3f, "电流数据正确性");
    ASSERT_TEST(group1 == 5, "组号数据正确性");
}

// 32位宏性能测试
void macro_performance_test()
{
    printf("\n=== 32位宏性能测试 ===\n");

    const int iterations = 100000;
    uint32_t data;
    uint8_t extracted;

    TEST_START();

    // 大量组装操作
    for (int i = 0; i < iterations; i++)
    {
        data = YFY_SET_ID(i % 8, i % 16, i % 64, i % 256, i % 256);
    }

    // 大量提取操作
    for (int i = 0; i < iterations; i++)
    {
        extracted = YFY_EXTRACT_ERROR_CODE(data);
        extracted = YFY_EXTRACT_DEVICE_ID(data);
        extracted = YFY_EXTRACT_CMD(data);
        extracted = YFY_EXTRACT_DEST_ADDR(data);
        extracted = YFY_EXTRACT_SRC_ADDR(data);
    }

    TEST_END();

    printf("完成 %d 次宏操作，耗时: %.2f ms\n", iterations * 6, stats.duration_ms);
    printf("平均每次宏操作: %.6f ms\n", stats.duration_ms / (iterations * 6));

    stats.total++;
    stats.passed++;
}

// 全覆盖功能测试
void full_coverage_test()
{
    printf("\n=== 全覆盖功能测试 ===\n");

    float f_val;
    uint8_t u8_val;
    uint16_t u16_val;
    int8_t i8_val;

    // 测试所有第一类函数（采样测试）
    ASSERT_TEST(yfy_get_module_voltage(0, &f_val) == true, "模块电压");
    ASSERT_TEST(yfy_get_module_current(0, &f_val) == true, "模块电流");
    ASSERT_TEST(yfy_get_module_group(0, &u8_val) == true, "模块组号");
    ASSERT_TEST(yfy_get_module_temp(0, &i8_val) == true, "模块温度");

    // 状态表2采样
    ASSERT_TEST(yfy_get_module_limit_power(0, &u8_val) == true, "限功率状态");
    ASSERT_TEST(yfy_get_module_id_duplicate(0, &u8_val) == true, "ID重复状态");
    ASSERT_TEST(yfy_get_three_phase_input_missing(0, &u8_val) == true, "三相缺相状态");

    // 状态表1采样
    ASSERT_TEST(yfy_get_module_dc_side_fault(0, &u8_val) == true, "DC侧故障");
    ASSERT_TEST(yfy_get_fan_fault(0, &u8_val) == true, "风扇故障");
    ASSERT_TEST(yfy_get_over_temp(0, &u8_val) == true, "过温状态");

    // 状态表0采样
    ASSERT_TEST(yfy_get_output_short_circuit(0, &u8_val) == true, "输出短路");
    ASSERT_TEST(yfy_get_module_sleep(0, &u8_val) == true, "模块休眠");

    // 电压电流采样
    ASSERT_TEST(yfy_get_ac_input_voltage_ab(0, &u16_val) == true, "AB相电压");
    ASSERT_TEST(yfy_get_max_voltage(0, &u16_val) == true, "最大电压");
    ASSERT_TEST(yfy_get_external_voltage(0, &u16_val) == true, "外部电压");
    ASSERT_TEST(yfy_get_module_addr_mode(0, &u8_val) == true, "地址模式");

    // 第二类函数
    ASSERT_TEST(yfy_get_group_voltage(1, &f_val) == true, "组电压");
    ASSERT_TEST(yfy_get_group_current(1, &f_val) == true, "组电流");
    ASSERT_TEST(yfy_get_group_module_num(1, &u8_val) == true, "组模块数");

    // 第三类函数
    ASSERT_TEST(yfy_get_sys_voltage(&f_val) == true, "系统电压");
    ASSERT_TEST(yfy_get_sys_current(&f_val) == true, "系统电流");
    ASSERT_TEST(yfy_get_sys_module_num(&u8_val) == true, "系统模块数");
}

int main()
{
    printf("=== YFY模块综合测试套件 ===\n");
    printf("开始时间: %s", ctime(&(time_t) {time(NULL)}));

    // 初始化随机数据
    generate_random_data();

    // 运行所有测试
    full_coverage_test();
    boundary_test();
    consistency_test();
    stress_test_read_operations();
    macro_performance_test();

    // 输出最终统计
    printf("\n=== 最终测试报告 ===\n");
    printf("总测试项: %d\n", stats.total);
    printf("通过: %d\n", stats.passed);
    printf("失败: %d\n", stats.failed);
    printf("成功率: %.1f%%\n", (float) stats.passed / stats.total * 100);
    printf("总耗时: %.2f ms\n", stats.duration_ms);

    if (stats.failed == 0)
    {
        printf("\n🎉 所有测试通过！系统运行正常。\n");
        return 0;
    }
    else
    {
        printf("\n❌ 发现 %d 个问题，请检查！\n", stats.failed);
        return 1;
    }
}
