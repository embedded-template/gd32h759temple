#pragma once
#include "module_config.h"
#include "stdbool.h"
#include "stdint.h"

// 模块的起始组号是1,0为广播。模块地址是0的时候，按组号进行广播。组号为0，按模块地址进行点对点传输。模块地址为0，组地址为0暂不清楚会发生什么。
#define YYLN_BROADCAST_ADDR 0x00

#define YYLN_MONITOR_ADDR 0x01

// 高压模式
#define YYLN_MOUDLE_HV 0x01
// 低压模式
#define YYLN_MODULE_LV 0x02

/**
 * @brief YYLN协议CAN ID组装宏
 * @param protocol 协议号 (4 bits, 0-15) - 位域[28:25]
 * @param monitor_addr 监控地址 (4 bits, 0-15) - 位域[24:21]
 * @param module_addr 模块地址 (7 bits, 0-127) - 位域[20:14]
 * @param production_day 生产日期 (5 bits, 1-31) - 位域[13:9]
 * @param serial_low 序列号低位 (9 bits, 0-511) - 位域[8:0]
 * @return 组装后的29位CAN ID
 * @note ID域报文格式：[28:25]协议号 [24:21]监控地址 [20:14]模块地址 [13:9]生产日期 [8:0]序列号低位
 */
#define YYLN_SET_ID(protocol, monitor_addr, module_addr, production_day, serial_low)                                                                           \
    ((uint32_t) (((protocol) & 0x0F) << 25) | (uint32_t) (((monitor_addr) & 0x0F) << 21) | (uint32_t) (((module_addr) & 0x7F) << 14) |                         \
     (uint32_t) (((production_day) & 0x1F) << 9) | (uint32_t) ((serial_low) & 0x1FF))

/**
 * @brief 从CAN ID中提取协议号
 * @param id CAN ID
 * @return 协议号 (4 bits)
 */
#define YYLN_EXTRACT_PROTOCOL(id) (((id) >> 25) & 0x0F)

/**
 * @brief 从CAN ID中提取监控地址
 * @param id CAN ID
 * @return 监控地址 (4 bits)
 */
#define YYLN_EXTRACT_MONITOR_ADDR(id) (((id) >> 21) & 0x0F)

/**
 * @brief 从CAN ID中提取模块地址
 * @param id CAN ID
 * @return 模块地址 (7 bits)
 */
#define YYLN_EXTRACT_MODULE_ADDR(id) (((id) >> 14) & 0x7F)

/**
 * @brief 从CAN ID中提取生产日期
 * @param id CAN ID
 * @return 生产日期 (5 bits)
 */
#define YYLN_EXTRACT_PRODUCTION_DAY(id) (((id) >> 9) & 0x1F)

/**
 * @brief 从CAN ID中提取序列号低位
 * @param id CAN ID
 * @return 序列号低位 (9 bits)
 */
#define YYLN_EXTRACT_SERIAL_LOW(id) ((id) & 0x1FF)

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
    yyln_Vab = 20u,
    yyln_Vbc = 21u,
    yyln_Vca = 22u,
    yyln_Tin = 30u,
    yyln_Iout_fastest = 47u,
    yyln_Iout_fast = 48u,
    yyln_GroupAddress = 89u,
    yyln_HiMode_LoMode_Selection = 95u,
    yyln_HiMode_LoMode_Status = 96u,
    yyln_Vout_fast = 98u,
    yyln_TrueHiLo_Status = 101u,
    yyln_CurrentCapability = 104u,
    yyln_CurrentAndCapability = 114u,
} yyln_message_cmd_e;

typedef struct yyln_module_data_t
{
    uint32_t vout[MODULE_NUM];                    // 模块输出电压 mV
    uint32_t iout[MODULE_NUM];                    // 模块输出电流 mA
    uint32_t v_set[MODULE_NUM];                   // 模块输出电压设定值 mV
    uint32_t i_set[MODULE_NUM];                   // 模块输出限流点设定值 mA
    uint32_t sn[MODULE_NUM];                      // 模块序列号的低 32 位
    uint8_t ac_overVoltage[MODULE_NUM];           // 交流过压
    uint8_t ac_underVoltage[MODULE_NUM];          // 交流欠压
    uint8_t ac_overVoltageClose[MODULE_NUM];      // 交流过压脱离(交流过压ss关机)
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
    uint8_t pfc_overTempleProtection[MODULE_NUM]; // PFC 过温保护 1
    uint8_t relay_faild[MODULE_NUM];              // 输出继电器故障
    uint8_t dc_overTempleProtection[MODULE_NUM];  // DC 过温保护 1
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
    uint32_t work_mode[MODULE_NUM];               // 读取模块实际的高低压档位状态（1：1000V 2：500V ）
    uint32_t module_max_curr[MODULE_NUM];         // 读取模块输出电流能力 单位：mA
    uint32_t iout1[MODULE_NUM];                   // 读取模块输出电流 单位：0.1A
    uint32_t module_max_curr1[MODULE_NUM];        // 读取模块输出功率能力 单位：0.1A
} yyln_module_data_t;

typedef struct yyln_single_module_info_t
{
    yyln_message_type_e message_type;
    yyln_message_cmd_e message_cmd;
    uint8_t byte_start;
    uint8_t byte_end;
    uint8_t bit_start;
    uint8_t bit_end;
    void* pdata;
} yyln_single_module_info_t;

typedef struct yyln_module_handle_t
{
    bool (*send)(uint32_t id, uint8_t* pdata);
    bool (*recv)(uint32_t id, uint8_t* pdata);
    uint32_t (*time)(void);
} yyln_module_handle_t;

// 模块在线信息
typedef struct yyln_module_online_info_t
{
    uint32_t last_online_time;
    bool is_online;
} yyln_module_online_info_t;

// 函数声明
yyln_module_handle_t* yyln_module_handle_get(void);
void yyln_module_handle_init(bool (*send)(uint32_t id, uint8_t* pdata), bool (*recv)(uint32_t id, uint8_t* pdata), uint32_t (*time)(void));

// 数据处理函数
void yyln_process_data(void);
void yyln_module_online_check(void);

// 数据获取函数
yyln_module_data_t* yyln_get_module_data(void);
yyln_module_online_info_t* yyln_get_module_online_info(void);

// 命令发送函数
void yyln_send_read_cmd(uint8_t module_addr, uint8_t group, yyln_message_type_e message_type, yyln_message_cmd_e message_cmd);
void yyln_send_write_cmd(uint8_t module_addr, uint8_t group, yyln_message_type_e message_type, yyln_message_cmd_e message_cmd, uint32_t data);