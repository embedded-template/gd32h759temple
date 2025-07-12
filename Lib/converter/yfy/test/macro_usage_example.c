#include "../yfy_data.h"
#include <stdio.h>

/**
 * @brief 32位数据组装宏使用示例
 * 
 * 本示例展示如何使用YFY_SET_ID宏来组装32位数据包，
 * 以及如何使用提取宏来解析32位数据包。
 */

void print_binary(uint32_t value, const char* label) {
    printf("%s: ", label);
    for (int i = 31; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
        if (i % 4 == 0 && i > 0) printf(" ");
    }
    printf(" (0x%08X)\n", value);
}

void demonstrate_bit_layout() {
    printf("=== 32位数据格式说明 ===\n");
    printf("位分布：[31:29]保留 [28:26]错误码 [25:22]设备号 [21:16]命令号 [15:8]目的地址 [7:0]源地址\n\n");
    
    // 示例：读取模块5的电压数据
    uint8_t error_code = 0;           // 无错误
    uint8_t device_id = DEVICE_ID;    // 0x0A
    uint8_t cmd = 0x03;               // 读取电压命令
    uint8_t dest_addr = 5;            // 模块5
    uint8_t src_addr = MONITOR_ADDR;  // 0xF0
    
    uint32_t data_packet = YFY_SET_ID(error_code, device_id, cmd, dest_addr, src_addr);
    
    printf("示例：读取模块5电压数据包\n");
    printf("输入参数：\n");
    printf("  错误码: %d (3 bits)\n", error_code);
    printf("  设备号: 0x%02X (4 bits)\n", device_id);
    printf("  命令号: 0x%02X (6 bits)\n", cmd);
    printf("  目的地址: %d (8 bits)\n", dest_addr);
    printf("  源地址: 0x%02X (8 bits)\n", src_addr);
    printf("\n");
    
    print_binary(data_packet, "组装结果");
    
    // 分段显示
    printf("\n位段分解：\n");
    printf("  [31:29] 保留位: %d%d%d\n", 
           (data_packet >> 31) & 1, (data_packet >> 30) & 1, (data_packet >> 29) & 1);
    printf("  [28:26] 错误码: %d%d%d (0x%X)\n", 
           (data_packet >> 28) & 1, (data_packet >> 27) & 1, (data_packet >> 26) & 1,
           YFY_EXTRACT_ERROR_CODE(data_packet));
    printf("  [25:22] 设备号: %d%d%d%d (0x%X)\n", 
           (data_packet >> 25) & 1, (data_packet >> 24) & 1, (data_packet >> 23) & 1, (data_packet >> 22) & 1,
           YFY_EXTRACT_DEVICE_ID(data_packet));
    printf("  [21:16] 命令号: %d%d%d%d%d%d (0x%02X)\n", 
           (data_packet >> 21) & 1, (data_packet >> 20) & 1, (data_packet >> 19) & 1,
           (data_packet >> 18) & 1, (data_packet >> 17) & 1, (data_packet >> 16) & 1,
           YFY_EXTRACT_CMD(data_packet));
    printf("  [15:8]  目的地址: 0x%02X\n", YFY_EXTRACT_DEST_ADDR(data_packet));
    printf("  [7:0]   源地址: 0x%02X\n", YFY_EXTRACT_SRC_ADDR(data_packet));
}

void demonstrate_common_commands() {
    printf("\n=== 常用命令示例 ===\n");
    
    struct {
        const char* description;
        uint8_t error_code;
        uint8_t device_id;
        uint8_t cmd;
        uint8_t dest_addr;
        uint8_t src_addr;
    } commands[] = {
        {"读取模块0电压", 0, DEVICE_ID, 0x03, 0, MONITOR_ADDR},
        {"读取模块1电流", 0, DEVICE_ID, 0x03, 1, MONITOR_ADDR},
        {"读取组1数据", 0, GROUP_DEVICE_ID, 0x01, 1, MONITOR_ADDR},
        {"读取组2数据", 0, GROUP_DEVICE_ID, 0x01, 2, MONITOR_ADDR},
        {"系统广播命令", 0, DEVICE_ID, 0x01, BROADCAST_ADDR, MONITOR_ADDR},
        {"错误响应示例", 0x05, DEVICE_ID, 0x03, 0, MONITOR_ADDR}
    };
    
    int num_commands = sizeof(commands) / sizeof(commands[0]);
    
    for (int i = 0; i < num_commands; i++) {
        uint32_t cmd_data = YFY_SET_ID(commands[i].error_code, commands[i].device_id, 
                                       commands[i].cmd, commands[i].dest_addr, commands[i].src_addr);
        printf("%s: 0x%08X\n", commands[i].description, cmd_data);
    }
}

void demonstrate_data_parsing() {
    printf("\n=== 数据解析示例 ===\n");
    
    // 模拟接收到的数据包
    uint32_t received_packets[] = {
        0x028300F0,  // 读取模块0电压
        0x02C101F0,  // 读取组1数据
        0x14830AF0   // 错误响应：模块10不存在
    };
    
    const char* descriptions[] = {
        "正常电压读取命令",
        "正常组数据读取命令", 
        "错误响应数据包"
    };
    
    int num_packets = sizeof(received_packets) / sizeof(received_packets[0]);
    
    for (int i = 0; i < num_packets; i++) {
        uint32_t packet = received_packets[i];
        printf("\n%s (0x%08X):\n", descriptions[i], packet);
        printf("  错误码: 0x%02X", YFY_EXTRACT_ERROR_CODE(packet));
        if (YFY_EXTRACT_ERROR_CODE(packet) != 0) {
            printf(" (有错误!)");
        }
        printf("\n");
        printf("  设备号: 0x%02X", YFY_EXTRACT_DEVICE_ID(packet));
        if (YFY_EXTRACT_DEVICE_ID(packet) == DEVICE_ID) {
            printf(" (单模块设备)");
        } else if (YFY_EXTRACT_DEVICE_ID(packet) == GROUP_DEVICE_ID) {
            printf(" (组设备)");
        }
        printf("\n");
        printf("  命令号: 0x%02X\n", YFY_EXTRACT_CMD(packet));
        printf("  目的地址: 0x%02X", YFY_EXTRACT_DEST_ADDR(packet));
        if (YFY_EXTRACT_DEST_ADDR(packet) == BROADCAST_ADDR) {
            printf(" (广播地址)");
        }
        printf("\n");
        printf("  源地址: 0x%02X", YFY_EXTRACT_SRC_ADDR(packet));
        if (YFY_EXTRACT_SRC_ADDR(packet) == MONITOR_ADDR) {
            printf(" (监控地址)");
        }
        printf("\n");
    }
}

int main() {
    printf("YFY 32位数据组装宏使用示例\n");
    printf("================================\n\n");
    
    demonstrate_bit_layout();
    demonstrate_common_commands();
    demonstrate_data_parsing();
    
    printf("\n=== 宏定义说明 ===\n");
    printf("YFY_SET_ID(error_code, device_id, cmd, dest_addr, src_addr)\n");
    printf("  - 组装32位数据包\n");
    printf("  - 参数范围：error_code(0-7), device_id(0-15), cmd(0-63), addr(0-255)\n\n");
    
    printf("YFY_EXTRACT_ERROR_CODE(data) - 提取错误码\n");
    printf("YFY_EXTRACT_DEVICE_ID(data)  - 提取设备号\n");
    printf("YFY_EXTRACT_CMD(data)        - 提取命令号\n");
    printf("YFY_EXTRACT_DEST_ADDR(data)  - 提取目的地址\n");
    printf("YFY_EXTRACT_SRC_ADDR(data)   - 提取源地址\n");
    
    return 0;
}
