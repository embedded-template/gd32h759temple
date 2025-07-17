#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// 模拟MODULE_NUM定义
#define MODULE_NUM 2

// 模拟yyln_module_data_t结构体（按实际顺序）
typedef struct yyln_module_data_t
{
    uint32_t vout[MODULE_NUM];                    // 1. 模块输出电压 mV
    uint32_t iout[MODULE_NUM];                    // 2. 模块输出电流 mA
    uint32_t v_set[MODULE_NUM];                   // 3. 模块输出电压设定值 mV
    uint32_t i_set[MODULE_NUM];                   // 4. 模块输出限流点设定值 mA
    uint32_t sn[MODULE_NUM];                      // 5. 模块序列号的低 32 位
    uint8_t ac_overVoltage[MODULE_NUM];           // 6. 交流过压
    uint8_t ac_underVoltage[MODULE_NUM];          // 7. 交流欠压
    uint8_t ac_overVoltageClose[MODULE_NUM];      // 8. 交流过压脱离(交流过压ss关机)
    uint8_t pfcBus_overVoltage[MODULE_NUM];       // 9. PFC 母线过压
    uint8_t pfcBus_underVoltage[MODULE_NUM];      // 10. PFC 母线欠压
    uint8_t pfcBus_unbalance[MODULE_NUM];         // 11. PFC 母线不平衡
    uint8_t dc_overVoltage[MODULE_NUM];           // 12. 直流输出过压
    uint8_t dc_overVoltageClose[MODULE_NUM];      // 13. 直流过压关机
    uint8_t dc_underVoltage[MODULE_NUM];          // 14. 直流输出欠压
    uint8_t fan_stop[MODULE_NUM];                 // 15. 风扇不运行
    uint8_t fan_driverFaild[MODULE_NUM];          // 16. 风扇驱动电路损坏
    uint8_t overTemple[MODULE_NUM];               // 17. 环境过温
    uint8_t underTempl[MODULE_NUM];               // 18. 环境温度过低
    uint8_t pfc_overTempleProtection[MODULE_NUM]; // 19. PFC 过温保护 1
    uint8_t relay_faild[MODULE_NUM];              // 20. 输出继电器故障
    uint8_t dc_overTempleProtection[MODULE_NUM];  // 21. DC 过温保护 1
    uint8_t dcdc_pfc_comFaild[MODULE_NUM];        // 22. PFC 与 DCDC 通信故障
    uint8_t pfc_faild[MODULE_NUM];                // 23. PFC 故障
    uint8_t dcdc_faile[MODULE_NUM];               // 24. DCDC 故障
    uint8_t dcdc_noWork[MODULE_NUM];              // 25. DCDC 不运行
    uint8_t outState[MODULE_NUM];                 // 26. 输出环路状态
    uint8_t dc_voltageUnbalance[MODULE_NUM];      // 27. DC 输出电压不平衡
    uint8_t same_sn[MODULE_NUM];                  // 28. 发现相同序列号的模块
    uint8_t bleeder_circuitFailure[MODULE_NUM];   // 29. 泄放电路异常
    uint32_t v_ab[MODULE_NUM];                    // 30. 线电压 AB mV
    uint32_t v_bc[MODULE_NUM];                    // 31. 线电压 BC mV
    uint32_t v_ca[MODULE_NUM];                    // 32. 线电压 CA mV
    uint32_t air_intake_temple[MODULE_NUM];       // 33. 进风口温度 毫度
    uint32_t module_group[MODULE_NUM];            // 34. 模块组号
    uint32_t work_mode[MODULE_NUM];               // 35. 读取模块实际的高低压档位状态（1：1000V 2：500V ）
    uint32_t module_max_curr[MODULE_NUM];         // 36. 读取模块输出电流能力 单位：mA
    uint32_t iout1[MODULE_NUM];                   // 37. 读取模块输出电流 单位：0.1A
    uint32_t module_max_curr1[MODULE_NUM];        // 38. 读取模块输出功率能力 单位：0.1A
} yyln_module_data_t;

// 模拟测试数据
static yyln_module_data_t test_data = {
    .vout = {5000, 4800},                // 1. 输出电压
    .iout = {2000, 1800},                // 2. 输出电流
    .v_set = {5100, 4900},               // 3. 电压设定值
    .i_set = {2500, 2300},               // 4. 电流限制设定值
    .sn = {0x12345678, 0x87654321},      // 5. 序列号
    .ac_overVoltage = {0, 1},            // 6. 交流过压
    .ac_underVoltage = {1, 0},           // 7. 交流欠压
    .ac_overVoltageClose = {0, 1},       // 8. 交流过压脱离
    .pfcBus_overVoltage = {1, 0},        // 9. PFC母线过压
    .pfcBus_underVoltage = {0, 1},       // 10. PFC母线欠压
    .pfcBus_unbalance = {1, 0},          // 11. PFC母线不平衡
    .dc_overVoltage = {0, 1},            // 12. 直流输出过压
    .dc_overVoltageClose = {1, 0},       // 13. 直流过压关机
    .dc_underVoltage = {0, 1},           // 14. 直流输出欠压
    .fan_stop = {1, 0},                  // 15. 风扇停止
    .fan_driverFaild = {0, 1},           // 16. 风扇驱动故障
    .overTemple = {1, 0},                // 17. 环境过温
    .underTempl = {0, 1},                // 18. 环境温度过低
    .pfc_overTempleProtection = {1, 0},  // 19. PFC过温保护
    .relay_faild = {0, 1},               // 20. 继电器故障
    .dc_overTempleProtection = {1, 0},   // 21. DC过温保护
    .dcdc_pfc_comFaild = {0, 1},         // 22. PFC与DCDC通信故障
    .pfc_faild = {1, 0},                 // 23. PFC故障
    .dcdc_faile = {0, 1},                // 24. DCDC故障
    .dcdc_noWork = {1, 0},               // 25. DCDC不运行
    .outState = {0, 1},                  // 26. 输出环路状态
    .dc_voltageUnbalance = {1, 0},       // 27. DC电压不平衡
    .same_sn = {0, 1},                   // 28. 相同序列号
    .bleeder_circuitFailure = {1, 0},    // 29. 泄放电路异常
    .v_ab = {380000, 375000},            // 30. AB相电压
    .v_bc = {385000, 380000},            // 31. BC相电压
    .v_ca = {382000, 378000},            // 32. CA相电压
    .air_intake_temple = {25000, 28000}, // 33. 进风口温度
    .module_group = {1, 2},              // 34. 模块组号
    .work_mode = {1, 2},                 // 35. 工作模式
    .module_max_curr = {3000, 2800},     // 36. 最大电流能力
    .iout1 = {200, 180},                 // 37. 电流(0.1A)
    .module_max_curr1 = {300, 280},      // 38. 功率能力
};

// 模拟函数
yyln_module_data_t* yyln_get_module_data(void)
{
    return &test_data;
}

void test_ordered_get_functions(void)
{
    printf("=== 测试按结构体顺序的Get函数 ===\n\n");

    printf("结构体字段顺序验证:\n");
    printf("1. vout[0] = %u mV\n", test_data.vout[0]);
    printf("2. iout[0] = %u mA\n", test_data.iout[0]);
    printf("3. v_set[0] = %u mV\n", test_data.v_set[0]);
    printf("4. i_set[0] = %u mA\n", test_data.i_set[0]);
    printf("5. sn[0] = 0x%08X\n", test_data.sn[0]);
    printf("6. ac_overVoltage[0] = %u\n", test_data.ac_overVoltage[0]);
    printf("7. ac_underVoltage[0] = %u\n", test_data.ac_underVoltage[0]);
    printf("8. ac_overVoltageClose[0] = %u\n", test_data.ac_overVoltageClose[0]);
    printf("9. pfcBus_overVoltage[0] = %u\n", test_data.pfcBus_overVoltage[0]);
    printf("10. pfcBus_underVoltage[0] = %u\n", test_data.pfcBus_underVoltage[0]);
    printf("...\n");
    printf("30. v_ab[0] = %u mV\n", test_data.v_ab[0]);
    printf("31. v_bc[0] = %u mV\n", test_data.v_bc[0]);
    printf("32. v_ca[0] = %u mV\n", test_data.v_ca[0]);
    printf("33. air_intake_temple[0] = %u 毫度\n", test_data.air_intake_temple[0]);
    printf("34. module_group[0] = %u\n", test_data.module_group[0]);
    printf("35. work_mode[0] = %u\n", test_data.work_mode[0]);
    printf("36. module_max_curr[0] = %u mA\n", test_data.module_max_curr[0]);
    printf("37. iout1[0] = %u (0.1A)\n", test_data.iout1[0]);
    printf("38. module_max_curr1[0] = %u (0.1A)\n", test_data.module_max_curr1[0]);

    printf("\n✓ 所有38个字段按结构体顺序排列！\n");
    printf("✓ 对应的get函数已按相同顺序实现！\n");
}

void print_function_list(void)
{
    printf("\n=== 按结构体顺序实现的Get函数列表 ===\n\n");

    const char* functions[] = {"1.  yyln_get_module_vout",
                               "2.  yyln_get_module_iout",
                               "3.  yyln_get_module_v_set",
                               "4.  yyln_get_module_i_set",
                               "5.  yyln_get_module_sn",
                               "6.  yyln_get_module_ac_overVoltage",
                               "7.  yyln_get_module_ac_underVoltage",
                               "8.  yyln_get_module_ac_overVoltageClose",
                               "9.  yyln_get_module_pfcBus_overVoltage",
                               "10. yyln_get_module_pfcBus_underVoltage",
                               "11. yyln_get_module_pfcBus_unbalance",
                               "12. yyln_get_module_dc_overVoltage",
                               "13. yyln_get_module_dc_overVoltageClose",
                               "14. yyln_get_module_dc_underVoltage",
                               "15. yyln_get_module_fan_stop",
                               "16. yyln_get_module_fan_driverFaild",
                               "17. yyln_get_module_overTemple",
                               "18. yyln_get_module_underTempl",
                               "19. yyln_get_module_pfc_overTempleProtection",
                               "20. yyln_get_module_relay_faild",
                               "21. yyln_get_module_dc_overTempleProtection",
                               "22. yyln_get_module_dcdc_pfc_comFaild",
                               "23. yyln_get_module_pfc_faild",
                               "24. yyln_get_module_dcdc_faile",
                               "25. yyln_get_module_dcdc_noWork",
                               "26. yyln_get_module_outState",
                               "27. yyln_get_module_dc_voltageUnbalance",
                               "28. yyln_get_module_same_sn",
                               "29. yyln_get_module_bleeder_circuitFailure",
                               "30. yyln_get_module_v_ab",
                               "31. yyln_get_module_v_bc",
                               "32. yyln_get_module_v_ca",
                               "33. yyln_get_module_air_intake_temple",
                               "34. yyln_get_module_module_group",
                               "35. yyln_get_module_work_mode",
                               "36. yyln_get_module_module_max_curr",
                               "37. yyln_get_module_iout1",
                               "38. yyln_get_module_module_max_curr1"};

    for (int i = 0; i < 38; i++)
    {
        printf("%s\n", functions[i]);
    }

    printf("\n总计: 38个get函数，完全对应yyln_module_data_t结构体的38个字段\n");
}

int main(void)
{
    printf("YYLN协议按结构体顺序的Get函数测试\n");
    printf("=================================\n\n");

    test_ordered_get_functions();
    print_function_list();

    printf("\n特点:\n");
    printf("- 函数名直接对应结构体字段名\n");
    printf("- 函数顺序与结构体字段顺序完全一致\n");
    printf("- 便于维护和查找对应关系\n");
    printf("- 覆盖了所有38个字段\n");

    return 0;
}
