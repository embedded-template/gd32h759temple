#include <assert.h>
#include <stdint.h>
#include <stdio.h>

// 直接包含宏定义用于测试
#define YYLN_SET_ID(protocol, monitor_addr, module_addr, production_day, serial_low)                                                                           \
    ((uint32_t) (((protocol) & 0x0F) << 25) | (uint32_t) (((monitor_addr) & 0x0F) << 21) | (uint32_t) (((module_addr) & 0x7F) << 14) |                         \
     (uint32_t) (((production_day) & 0x1F) << 9) | (uint32_t) ((serial_low) & 0x1FF))

#define YYLN_EXTRACT_PROTOCOL(id) (((id) >> 25) & 0x0F)
#define YYLN_EXTRACT_MONITOR_ADDR(id) (((id) >> 21) & 0x0F)
#define YYLN_EXTRACT_MODULE_ADDR(id) (((id) >> 14) & 0x7F)
#define YYLN_EXTRACT_PRODUCTION_DAY(id) (((id) >> 9) & 0x1F)
#define YYLN_EXTRACT_SERIAL_LOW(id) ((id) & 0x1FF)

/**
 * @brief 测试YYLN协议ID组装和拆分宏
 */
void test_yyln_id_macros(void)
{
    printf("开始测试YYLN协议ID组装和拆分宏...\n");

    // 测试用例1: 基本功能测试
    uint8_t protocol = 0x01;     // 协议号 (4 bits)
    uint8_t monitor_addr = 0x0F; // 监控地址 (4 bits) - Monitor范围0x01-0x0F，BroadCast为0
    uint8_t module_addr = 0x7F;  // 模块地址 (7 bits) - Module范围0x01-0x7F，BroadCast为0
    uint8_t production_day = 31; // 生产日期 (5 bits) - 1-31可选，0可选
    uint16_t serial_low = 0x1FF; // 序列号低位 (9 bits) - 0可选

    // 组装ID
    uint32_t assembled_id = YYLN_SET_ID(protocol, monitor_addr, module_addr, production_day, serial_low);

    printf("测试用例1 - 基本功能:\n");
    printf("  输入: protocol=0x%02X, monitor_addr=0x%02X, module_addr=0x%02X, production_day=%d, serial_low=0x%03X\n", protocol, monitor_addr, module_addr,
           production_day, serial_low);
    printf("  组装后ID: 0x%08X\n", assembled_id);

    // 拆分ID并验证
    uint8_t extracted_protocol = YYLN_EXTRACT_PROTOCOL(assembled_id);
    uint8_t extracted_monitor = YYLN_EXTRACT_MONITOR_ADDR(assembled_id);
    uint8_t extracted_module = YYLN_EXTRACT_MODULE_ADDR(assembled_id);
    uint8_t extracted_day = YYLN_EXTRACT_PRODUCTION_DAY(assembled_id);
    uint16_t extracted_serial = YYLN_EXTRACT_SERIAL_LOW(assembled_id);

    printf("  拆分结果: protocol=0x%02X, monitor_addr=0x%02X, module_addr=0x%02X, production_day=%d, serial_low=0x%03X\n", extracted_protocol,
           extracted_monitor, extracted_module, extracted_day, extracted_serial);

    // 验证结果
    assert(extracted_protocol == protocol);
    assert(extracted_monitor == monitor_addr);
    assert(extracted_module == module_addr);
    assert(extracted_day == production_day);
    assert(extracted_serial == serial_low);
    printf("  ✓ 测试用例1通过\n\n");

    // 测试用例2: 边界值测试
    printf("测试用例2 - 边界值:\n");

    // 最小值测试
    uint32_t min_id = YYLN_SET_ID(0, 0, 0, 0, 0);
    printf("  最小值ID: 0x%08X\n", min_id);
    assert(YYLN_EXTRACT_PROTOCOL(min_id) == 0);
    assert(YYLN_EXTRACT_MONITOR_ADDR(min_id) == 0);
    assert(YYLN_EXTRACT_MODULE_ADDR(min_id) == 0);
    assert(YYLN_EXTRACT_PRODUCTION_DAY(min_id) == 0);
    assert(YYLN_EXTRACT_SERIAL_LOW(min_id) == 0);
    printf("  ✓ 最小值测试通过\n");

    // 最大值测试
    uint32_t max_id = YYLN_SET_ID(0x0F, 0x0F, 0x7F, 0x1F, 0x1FF);
    printf("  最大值ID: 0x%08X\n", max_id);
    assert(YYLN_EXTRACT_PROTOCOL(max_id) == 0x0F);
    assert(YYLN_EXTRACT_MONITOR_ADDR(max_id) == 0x0F);
    assert(YYLN_EXTRACT_MODULE_ADDR(max_id) == 0x7F);
    assert(YYLN_EXTRACT_PRODUCTION_DAY(max_id) == 0x1F);
    assert(YYLN_EXTRACT_SERIAL_LOW(max_id) == 0x1FF);
    printf("  ✓ 最大值测试通过\n\n");

    // 测试用例3: 实际应用场景测试
    printf("测试用例3 - 实际应用场景:\n");

    // 场景1: 监控向模块1发送命令
    uint32_t monitor_to_module1 = YYLN_SET_ID(0x01, 0x01, 0x01, 15, 100);
    printf("  监控->模块1: 0x%08X\n", monitor_to_module1);
    assert(YYLN_EXTRACT_PROTOCOL(monitor_to_module1) == 0x01);
    assert(YYLN_EXTRACT_MONITOR_ADDR(monitor_to_module1) == 0x01);
    assert(YYLN_EXTRACT_MODULE_ADDR(monitor_to_module1) == 0x01);
    assert(YYLN_EXTRACT_PRODUCTION_DAY(monitor_to_module1) == 15);
    assert(YYLN_EXTRACT_SERIAL_LOW(monitor_to_module1) == 100);

    // 场景2: 广播消息 (模块地址为0)
    uint32_t broadcast_id = YYLN_SET_ID(0x01, 0x01, 0x00, 1, 0);
    printf("  广播消息: 0x%08X\n", broadcast_id);
    assert(YYLN_EXTRACT_PROTOCOL(broadcast_id) == 0x01);
    assert(YYLN_EXTRACT_MONITOR_ADDR(broadcast_id) == 0x01);
    assert(YYLN_EXTRACT_MODULE_ADDR(broadcast_id) == 0x00);
    assert(YYLN_EXTRACT_PRODUCTION_DAY(broadcast_id) == 1);
    assert(YYLN_EXTRACT_SERIAL_LOW(broadcast_id) == 0);

    printf("  ✓ 实际应用场景测试通过\n\n");

    printf("所有测试通过！YYLN协议ID宏定义正确。\n");
}

int main(void)
{
    test_yyln_id_macros();
    return 0;
}
