#pragma once
#include "stdint.h"

#define YYLN_MODULE_NUM 12

typedef enum yyln_message_type_e{
    yyln_SetData    = 0u,
    yyln_SetDataResponse = 1u,
    yyln_ReadData = 2u,
    yyln_ReadDataResponse = 3u,
    yyln_ReadSerialNumberResponse = 4u,
    yyln_AllSetData = 5u,
    yyln_AllSetDataResponse = 6u,
}message_type_e;

typedef enum yyln_message_cmd_e{
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
}yyln_message_cmd_e;

typedef struct yyln_module_data_t
{
    uint32_t vout[YYLN_MODULE_NUM];// 模块输出电压 mV
    uint32_t iout[YYLN_MODULE_NUM];//模块输出电流 mA
    uint32_t v_set[YYLN_MODULE_NUM];//模块输出电压设定值 mV
    uint32_t i_set[YYLN_MODULE_NUM];//模块输出限流点设定值 mA
    uint32_t sn[YYLN_MODULE_NUM];//模块序列号的低 32 位
    uint8_t ac_overVoltage[YYLN_MODULE_NUM];//
    uint8_t ac_underVoltage[YYLN_MODULE_NUM];//
    uint8_t ac_overVoltageClose[YYLN_MODULE_NUM];//
    uint8_t pfcBus_overVoltage[YYLN_MODULE_NUM];//
    uint8_t pfcBus_underVoltage[YYLN_MODULE_NUM];//
    uint8_t pfcBus_unbalance[YYLN_MODULE_NUM];//
    uint8_t dc_overVoltage[YYLN_MODULE_NUM];//
    uint8_t dc_overVoltageClose[YYLN_MODULE_NUM];//
    uint8_t dc_underVoltage[YYLN_MODULE_NUM];//
    uint8_t fan_stop[YYLN_MODULE_NUM];//
    uint8_t fan_driverFaild[YYLN_MODULE_NUM];//
    uint8_t overTemple[YYLN_MODULE_NUM];
    uint8_t underTempl[YYLN_MODULE_NUM];
    uint8_t pfc_overTempleProtection[YYLN_MODULE_NUM];
    uint8_t relay_faild[YYLN_MODULE_NUM];
    uint8_t dc_overTempleProtection[YYLN_MODULE_NUM];
    uint8_t dcdc_pfc_comFaild[YYLN_MODULE_NUM];
    uint8_t pfc_faild[YYLN_MODULE_NUM];
    uint8_t dcdc_faile[YYLN_MODULE_NUM];
    uint8_t dcdc_noWork[YYLN_MODULE_NUM];
    uint8_t outState[YYLN_MODULE_NUM];
    uint8_t dc_voltageUnbalance[YYLN_MODULE_NUM];
    uint8_t bleeder_circuitFailure[YYLN_MODULE_NUM];
    uint32_t v_ab[YYLN_MODULE_NUM];
    uint32_t v_bc[YYLN_MODULE_NUM];
    uint32_t v_ca[YYLN_MODULE_NUM];
    uint32_t air_intake_temple[YYLN_MODULE_NUM];
    uint32_t work_mode[YYLN_MODULE_NUM];
    uint32_t module_max_curr[YYLN_MODULE_NUM];
}yyln_module_data_t;

typedef struct yyln_single_module_info_t
{
    message_type_e  message_type;
    yyln_message_cmd_e  message_cmd;
};
