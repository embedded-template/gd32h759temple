#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// 模拟MODULE_NUM定义
#define MODULE_NUM 3
#define YYLN_BROADCAST_ADDR 0xFF

// 模拟yyln_module_data_t结构体
typedef struct yyln_module_data_t
{
    uint32_t vout[MODULE_NUM];                    // 模块输出电压 mV
    uint32_t iout[MODULE_NUM];                    // 模块输出电流 mA
    uint32_t v_set[MODULE_NUM];                   // 模块输出电压设定值 mV
    uint32_t i_set[MODULE_NUM];                   // 模块输出限流点设定值 mA
    uint32_t sn[MODULE_NUM];                      // 模块序列号的低 32 位
    uint8_t ac_overVoltage[MODULE_NUM];           // 交流过压
    uint8_t ac_underVoltage[MODULE_NUM];          // 交流欠压
    uint8_t ac_overVoltageClose[MODULE_NUM];      // 交流过压脱离
    uint8_t pfcBus_overVoltage[MODULE_NUM];       // PFC 母线过压
    uint8_t pfcBus_underVoltage[MODULE_NUM];      // PFC 母线欠压
    uint8_t pfcBus_unbalance[MODULE_NUM];         // PFC 母线不平衡
    uint8_t dc_overVoltage[MODULE_NUM];           // 直流输出过压
    uint8_t dc_overVoltageClose[MODULE_NUM];      // 直流过压关机
    uint8_t dc_underVoltage[MODULE_NUM];          // 直流输出欠压
    uint8_t fan_stop[MODULE_NUM];                 // 风扇不运行
    uint8_t fan_driverFaild[MODULE_NUM];          // 风扇驱动电路损坏
    uint8_t overTemple[MODULE_NUM];               // 环境过温
    uint8_t underTempl[MODULE_NUM];               // 环境温度过低
    uint8_t pfc_overTempleProtection[MODULE_NUM]; // PFC 过温保护
    uint8_t relay_faild[MODULE_NUM];              // 输出继电器故障
    uint8_t dc_overTempleProtection[MODULE_NUM];  // DC 过温保护
    uint8_t dcdc_pfc_comFaild[MODULE_NUM];        // PFC 与 DCDC 通信故障
    uint8_t pfc_faild[MODULE_NUM];                // PFC 故障
    uint8_t dcdc_faile[MODULE_NUM];               // DCDC 故障
    uint8_t dcdc_noWork[MODULE_NUM];              // DCDC 不运行
    uint8_t outState[MODULE_NUM];                 // 输出环路状态
    uint8_t dc_voltageUnbalance[MODULE_NUM];      // DC 输出电压不平衡
    uint8_t same_sn[MODULE_NUM];                  // 发现相同序列号的模块
    uint8_t bleeder_circuitFailure[MODULE_NUM];   // 泄放电路异常
    uint32_t v_ab[MODULE_NUM];                    // 线电压 AB mV
    uint32_t v_bc[MODULE_NUM];                    // 线电压 BC mV
    uint32_t v_ca[MODULE_NUM];                    // 线电压 CA mV
    uint32_t air_intake_temple[MODULE_NUM];       // 进风口温度 毫度
    uint32_t module_group[MODULE_NUM];            // 模块组号
    uint32_t work_mode[MODULE_NUM];               // 工作模式
    uint32_t module_max_curr[MODULE_NUM];         // 模块输出电流能力
    uint32_t iout1[MODULE_NUM];                   // 模块输出电流 0.1A
    uint32_t module_max_curr1[MODULE_NUM];        // 模块输出功率能力
} yyln_module_data_t;

// 模拟数据
static yyln_module_data_t test_module_data = {
    .vout = {5000, 4800, 5200},
    .iout = {2000, 1800, 2200},
    .v_set = {5100, 4900, 5300},
    .i_set = {2500, 2300, 2700},
    .sn = {0x12345678, 0x87654321, 0xABCDEF00},
    .ac_overVoltage = {0, 1, 0},
    .ac_underVoltage = {1, 0, 0},
    .dc_overVoltage = {0, 0, 1},
    .fan_stop = {0, 1, 0},
    .overTemple = {1, 0, 0},
    .v_ab = {380000, 375000, 385000},
    .v_bc = {385000, 380000, 390000},
    .v_ca = {382000, 378000, 387000},
    .air_intake_temple = {25000, 28000, 23000},
    .module_group = {1, 2, 1},
    .work_mode = {1, 2, 1},
    .module_max_curr = {3000, 2800, 3200},
    .iout1 = {200, 180, 220},
    .module_max_curr1 = {300, 280, 320},
};

// 模拟命令记录
typedef struct
{
    uint8_t module_addr;
    uint8_t group;
    uint8_t cmd_type;
    uint8_t data_id;
    uint32_t data;
} command_record_t;

static command_record_t last_write_cmd = {0};
static command_record_t last_read_cmd = {0};

// 模拟枚举
typedef enum
{
    yyln_SetData = 0x01,
    yyln_ReadData = 0x02
} yyln_cmd_type_t;

typedef enum
{
    yyln_VoutReference = 0x10,
    yyln_IoutLimit = 0x11,
    yyln_GroupAddress = 0x12,
    yyln_HiMode_LoMode_Selection = 0x13,
    yyln_Vout = 0x20,
    yyln_Iout_slow = 0x21,
    yyln_ReadSN = 0x22,
    yyln_ModuleStatus = 0x23,
    yyln_Vab = 0x24,
    yyln_Vbc = 0x25,
    yyln_Vca = 0x26,
    yyln_Tin = 0x27,
    yyln_TrueHiLo_Status = 0x28,
    yyln_CurrentCapability = 0x29,
    yyln_CurrentAndCapability = 0x2A
} yyln_data_id_t;

// 模拟底层函数
yyln_module_data_t* yyln_get_module_data(void)
{
    return &test_module_data;
}

void yyln_send_write_cmd(uint8_t module_addr, uint8_t group, uint8_t cmd_type, uint8_t data_id, uint32_t data)
{
    last_write_cmd.module_addr = module_addr;
    last_write_cmd.group = group;
    last_write_cmd.cmd_type = cmd_type;
    last_write_cmd.data_id = data_id;
    last_write_cmd.data = data;
    printf("    发送写命令: 模块%d, 组%d, 类型0x%02X, ID0x%02X, 数据%u\n", module_addr, group, cmd_type, data_id, data);
}

void yyln_send_read_cmd(uint8_t module_addr, uint8_t group, uint8_t cmd_type, uint8_t data_id)
{
    last_read_cmd.module_addr = module_addr;
    last_read_cmd.group = group;
    last_read_cmd.cmd_type = cmd_type;
    last_read_cmd.data_id = data_id;
    printf("    发送读命令: 模块%d, 组%d, 类型0x%02X, ID0x%02X\n", module_addr, group, cmd_type, data_id);
}

// 手动包含接口函数的实现（避免依赖问题）
// 内部辅助函数：检查模块地址是否有效
static bool yyln_module_addr_is_available(uint8_t module_addr)
{
    if (module_addr == 0 || module_addr > MODULE_NUM)
    {
        return false;
    }
    return true;
}

static bool yyln_group_addr_is_available(uint8_t group_addr)
{
    if (group_addr > 15)
    {
        return false;
    }
    return true;
}

// 写入命令接口实现
bool yyln_send_write_module_vout_reference(uint8_t module_addr, uint8_t group, uint32_t voltage)
{
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    if (!yyln_group_addr_is_available(group))
    {
        return false;
    }

    yyln_send_write_cmd(module_addr, group, yyln_SetData, yyln_VoutReference, voltage);
    return true;
}

bool yyln_send_write_module_iout_limit(uint8_t module_addr, uint8_t group, uint32_t current)
{
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    if (!yyln_group_addr_is_available(group))
    {
        return false;
    }

    yyln_send_write_cmd(module_addr, group, yyln_SetData, yyln_IoutLimit, current);
    return true;
}

bool yyln_send_write_group_num(uint8_t module_addr, uint8_t new_group)
{
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    if (!yyln_group_addr_is_available(new_group))
    {
        return false;
    }

    yyln_send_write_cmd(module_addr, YYLN_BROADCAST_ADDR, yyln_SetData, yyln_GroupAddress, new_group);
    return true;
}

bool yyln_send_write_module_himode_lomode_selection(uint8_t module_addr, uint8_t group, uint32_t mode)
{
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    if (!yyln_group_addr_is_available(group))
    {
        return false;
    }

    yyln_send_write_cmd(module_addr, group, yyln_SetData, yyln_HiMode_LoMode_Selection, mode);
    return true;
}

// 读取命令接口实现
bool yyln_send_read_vout(uint8_t module_addr)
{
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    yyln_send_read_cmd(module_addr, YYLN_BROADCAST_ADDR, yyln_ReadData, yyln_Vout);
    return true;
}

bool yyln_send_read_vout_by_group(uint8_t group)
{
    if (!yyln_group_addr_is_available(group))
    {
        return false;
    }

    yyln_send_read_cmd(YYLN_BROADCAST_ADDR, group, yyln_ReadData, yyln_Vout);
    return true;
}

bool yyln_send_read_iout(uint8_t module_addr)
{
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    yyln_send_read_cmd(module_addr, YYLN_BROADCAST_ADDR, yyln_ReadData, yyln_Iout_slow);
    return true;
}

bool yyln_send_read_iout_by_group(uint8_t group)
{
    if (!yyln_group_addr_is_available(group))
    {
        return false;
    }

    yyln_send_read_cmd(YYLN_BROADCAST_ADDR, group, yyln_ReadData, yyln_Iout_slow);
    return true;
}

bool yyln_send_read_sn(uint8_t module_addr)
{
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    yyln_send_read_cmd(module_addr, YYLN_BROADCAST_ADDR, yyln_ReadData, yyln_ReadSN);
    return true;
}

bool yyln_send_read_sn_by_group(uint8_t group)
{
    if (!yyln_group_addr_is_available(group))
    {
        return false;
    }

    yyln_send_read_cmd(YYLN_BROADCAST_ADDR, group, yyln_ReadData, yyln_ReadSN);
    return true;
}

// 数据获取函数实现
bool yyln_get_module_vout(uint8_t module_addr, uint32_t* voltage)
{
    if (voltage == NULL)
    {
        return false;
    }
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    yyln_module_data_t* module_data = yyln_get_module_data();
    *voltage = module_data->vout[module_addr - 1];
    return true;
}

bool yyln_get_module_iout(uint8_t module_addr, uint32_t* current)
{
    if (current == NULL)
    {
        return false;
    }
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    yyln_module_data_t* module_data = yyln_get_module_data();
    *current = module_data->iout[module_addr - 1];
    return true;
}

bool yyln_get_module_v_set(uint8_t module_addr, uint32_t* voltage_set)
{
    if (voltage_set == NULL)
    {
        return false;
    }
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    yyln_module_data_t* module_data = yyln_get_module_data();
    *voltage_set = module_data->v_set[module_addr - 1];
    return true;
}

bool yyln_get_module_i_set(uint8_t module_addr, uint32_t* current_limit)
{
    if (current_limit == NULL)
    {
        return false;
    }
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    yyln_module_data_t* module_data = yyln_get_module_data();
    *current_limit = module_data->i_set[module_addr - 1];
    return true;
}

bool yyln_get_module_sn(uint8_t module_addr, uint32_t* serial_number)
{
    if (serial_number == NULL)
    {
        return false;
    }
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    yyln_module_data_t* module_data = yyln_get_module_data();
    *serial_number = module_data->sn[module_addr - 1];
    return true;
}

bool yyln_get_module_ac_overVoltage(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return false;
    }
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    yyln_module_data_t* module_data = yyln_get_module_data();
    *status = module_data->ac_overVoltage[module_addr - 1];
    return true;
}

bool yyln_get_module_ac_underVoltage(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return false;
    }
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    yyln_module_data_t* module_data = yyln_get_module_data();
    *status = module_data->ac_underVoltage[module_addr - 1];
    return true;
}

bool yyln_get_module_dc_overVoltage(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return false;
    }
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    yyln_module_data_t* module_data = yyln_get_module_data();
    *status = module_data->dc_overVoltage[module_addr - 1];
    return true;
}

bool yyln_get_module_fan_stop(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return false;
    }
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    yyln_module_data_t* module_data = yyln_get_module_data();
    *status = module_data->fan_stop[module_addr - 1];
    return true;
}

bool yyln_get_module_overTemple(uint8_t module_addr, uint8_t* status)
{
    if (status == NULL)
    {
        return false;
    }
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    yyln_module_data_t* module_data = yyln_get_module_data();
    *status = module_data->overTemple[module_addr - 1];
    return true;
}

bool yyln_get_module_v_ab(uint8_t module_addr, uint32_t* voltage)
{
    if (voltage == NULL)
    {
        return false;
    }
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    yyln_module_data_t* module_data = yyln_get_module_data();
    *voltage = module_data->v_ab[module_addr - 1];
    return true;
}

bool yyln_get_module_v_bc(uint8_t module_addr, uint32_t* voltage)
{
    if (voltage == NULL)
    {
        return false;
    }
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    yyln_module_data_t* module_data = yyln_get_module_data();
    *voltage = module_data->v_bc[module_addr - 1];
    return true;
}

bool yyln_get_module_v_ca(uint8_t module_addr, uint32_t* voltage)
{
    if (voltage == NULL)
    {
        return false;
    }
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    yyln_module_data_t* module_data = yyln_get_module_data();
    *voltage = module_data->v_ca[module_addr - 1];
    return true;
}

bool yyln_get_module_air_intake_temple(uint8_t module_addr, uint32_t* temperature)
{
    if (temperature == NULL)
    {
        return false;
    }
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    yyln_module_data_t* module_data = yyln_get_module_data();
    *temperature = module_data->air_intake_temple[module_addr - 1];
    return true;
}

bool yyln_get_module_module_group(uint8_t module_addr, uint32_t* group)
{
    if (group == NULL)
    {
        return false;
    }
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    yyln_module_data_t* module_data = yyln_get_module_data();
    *group = module_data->module_group[module_addr - 1];
    return true;
}

bool yyln_get_module_work_mode(uint8_t module_addr, uint32_t* work_mode)
{
    if (work_mode == NULL)
    {
        return false;
    }
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    yyln_module_data_t* module_data = yyln_get_module_data();
    *work_mode = module_data->work_mode[module_addr - 1];
    return true;
}

bool yyln_get_module_module_max_curr(uint8_t module_addr, uint32_t* max_current)
{
    if (max_current == NULL)
    {
        return false;
    }
    if (!yyln_module_addr_is_available(module_addr))
    {
        return false;
    }

    yyln_module_data_t* module_data = yyln_get_module_data();
    *max_current = module_data->module_max_curr[module_addr - 1];
    return true;
}

// 测试函数
void test_write_commands(void)
{
    printf("=== 测试写入命令接口 ===\n");

    // 测试设置输出电压
    printf("1. 测试设置模块输出电压:\n");
    assert(yyln_send_write_module_vout_reference(1, 1, 5000) == true);
    assert(last_write_cmd.module_addr == 1);
    assert(last_write_cmd.group == 1);
    assert(last_write_cmd.data == 5000);

    // 测试无效参数
    assert(yyln_send_write_module_vout_reference(0, 1, 5000) == false);
    assert(yyln_send_write_module_vout_reference(1, 16, 5000) == false);

    // 测试设置限流点
    printf("2. 测试设置模块限流点:\n");
    assert(yyln_send_write_module_iout_limit(2, 2, 2500) == true);

    // 测试设置组地址
    printf("3. 测试设置组地址:\n");
    assert(yyln_send_write_group_num(1, 5) == true);

    // 测试设置高低压模式
    printf("4. 测试设置高低压模式:\n");
    assert(yyln_send_write_module_himode_lomode_selection(1, 1, 1) == true);

    printf("✓ 写入命令测试通过\n\n");
}

void test_read_commands(void)
{
    printf("=== 测试读取命令接口 ===\n");

    // 测试读取输出电压
    printf("1. 测试读取输出电压:\n");
    assert(yyln_send_read_vout(1) == true);
    assert(yyln_send_read_vout_by_group(1) == true);

    // 测试读取输出电流
    printf("2. 测试读取输出电流:\n");
    assert(yyln_send_read_iout(2) == true);
    assert(yyln_send_read_iout_by_group(2) == true);

    // 测试读取序列号
    printf("3. 测试读取序列号:\n");
    assert(yyln_send_read_sn(1) == true);
    assert(yyln_send_read_sn_by_group(1) == true);

    // 测试无效参数
    assert(yyln_send_read_vout(0) == false);
    assert(yyln_send_read_vout_by_group(16) == false);

    printf("✓ 读取命令测试通过\n\n");
}

void test_data_get_functions(void)
{
    printf("=== 测试数据获取接口 ===\n");

    uint32_t voltage, current, temperature, group, work_mode, max_current;
    uint8_t status;

    // 测试基本数据获取
    printf("1. 测试基本数据获取:\n");
    for (uint8_t module = 1; module <= MODULE_NUM; module++)
    {
        printf("  模块%d:\n", module);

        // 测试电压获取
        assert(yyln_get_module_vout(module, &voltage) == true);
        printf("    输出电压: %u mV\n", voltage);

        // 测试电流获取
        assert(yyln_get_module_iout(module, &current) == true);
        printf("    输出电流: %u mA\n", current);

        // 测试设定值获取
        assert(yyln_get_module_v_set(module, &voltage) == true);
        printf("    电压设定值: %u mV\n", voltage);

        assert(yyln_get_module_i_set(module, &current) == true);
        printf("    电流限制设定值: %u mA\n", current);

        // 测试序列号获取
        uint32_t sn;
        assert(yyln_get_module_sn(module, &sn) == true);
        printf("    序列号: 0x%08X\n", sn);

        printf("\n");
    }

    // 测试状态获取
    printf("2. 测试状态获取:\n");
    for (uint8_t module = 1; module <= MODULE_NUM; module++)
    {
        printf("  模块%d状态:\n", module);

        // 测试交流电压状态
        assert(yyln_get_module_ac_overVoltage(module, &status) == true);
        printf("    交流过压: %s\n", status ? "异常" : "正常");

        assert(yyln_get_module_ac_underVoltage(module, &status) == true);
        printf("    交流欠压: %s\n", status ? "异常" : "正常");

        // 测试直流电压状态
        assert(yyln_get_module_dc_overVoltage(module, &status) == true);
        printf("    直流过压: %s\n", status ? "异常" : "正常");

        // 测试风扇状态
        assert(yyln_get_module_fan_stop(module, &status) == true);
        printf("    风扇状态: %s\n", status ? "停止" : "正常");

        // 测试温度状态
        assert(yyln_get_module_overTemple(module, &status) == true);
        printf("    过温状态: %s\n", status ? "过温" : "正常");

        printf("\n");
    }

    // 测试电压读取
    printf("3. 测试三相电压读取:\n");
    for (uint8_t module = 1; module <= MODULE_NUM; module++)
    {
        printf("  模块%d三相电压:\n", module);

        assert(yyln_get_module_v_ab(module, &voltage) == true);
        printf("    AB相电压: %u mV\n", voltage);

        assert(yyln_get_module_v_bc(module, &voltage) == true);
        printf("    BC相电压: %u mV\n", voltage);

        assert(yyln_get_module_v_ca(module, &voltage) == true);
        printf("    CA相电压: %u mV\n", voltage);

        printf("\n");
    }

    // 测试其他参数
    printf("4. 测试其他参数:\n");
    for (uint8_t module = 1; module <= MODULE_NUM; module++)
    {
        printf("  模块%d其他参数:\n", module);

        // 测试温度
        assert(yyln_get_module_air_intake_temple(module, &temperature) == true);
        printf("    进风口温度: %u 毫度\n", temperature);

        // 测试组号
        assert(yyln_get_module_module_group(module, &group) == true);
        printf("    模块组号: %u\n", group);

        // 测试工作模式
        assert(yyln_get_module_work_mode(module, &work_mode) == true);
        printf("    工作模式: %u (%s)\n", work_mode, work_mode == 1 ? "1000V高压模式" : "500V低压模式");

        // 测试电流能力
        assert(yyln_get_module_module_max_curr(module, &max_current) == true);
        printf("    最大电流能力: %u mA\n", max_current);

        printf("\n");
    }

    // 测试无效参数
    printf("5. 测试参数验证:\n");
    assert(yyln_get_module_vout(0, &voltage) == false);
    assert(yyln_get_module_vout(MODULE_NUM + 1, &voltage) == false);
    assert(yyln_get_module_vout(1, NULL) == false);
    printf("    参数验证功能正常\n");

    printf("✓ 数据获取测试通过\n\n");
}

int main(void)
{
    printf("YYLN接口函数测试程序\n");
    printf("====================\n\n");

    test_write_commands();
    test_read_commands();
    test_data_get_functions();

    printf("=== 测试总结 ===\n");
    printf("✓ 所有写入命令接口测试通过\n");
    printf("✓ 所有读取命令接口测试通过\n");
    printf("✓ 所有数据获取接口测试通过\n");
    printf("✓ 参数验证功能正常\n");
    printf("✓ 错误处理功能正常\n");

    return 0;
}
