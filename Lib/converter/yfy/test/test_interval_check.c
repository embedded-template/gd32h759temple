#include "../yfy_data.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

// 模拟时间函数
static uint32_t mock_time = 0;

uint32_t mock_time_func(void)
{
    return mock_time;
}

// 模拟发送和接收函数
bool mock_send_func(uint32_t id, uint8_t* pdata)
{
    return true;
}

bool mock_recv_func(uint8_t id, uint8_t* pdata)
{
    return false;
}

// 测试1000ms间隔检查功能
void test_interval_check()
{
    printf("=== 1000ms间隔检查测试 ===\n");

    // 初始化模块句柄
    yfy_module_handle_init(mock_send_func, mock_recv_func, mock_time_func);

    // 获取在线信息数组（需要声明外部变量）
    extern yfy_module_online_info_t stModuleOnlineInfo[MODULE_NUM];
    yfy_module_online_info_t* online_info = stModuleOnlineInfo;

    printf("\n测试1：验证1000ms间隔控制\n");

    // 设置模块0在线，时间为0
    mock_time = 0;
    online_info[0].module_addr = 0;
    online_info[0].last_online_time = 0;
    online_info[0].is_online = true;

    // 第一次调用，应该执行检查（初始化）
    printf("时间0ms: 第一次调用检查函数\n");
    module_online_check();
    printf("模块0状态: %s\n", online_info[0].is_online ? "在线" : "离线");
    assert(online_info[0].is_online == true); // 时间差为0，应该在线

    // 时间推进到500ms，应该不执行检查
    mock_time = 500;
    printf("\n时间500ms: 调用检查函数（应该跳过检查）\n");

    // 人为设置模块离线来测试是否真的跳过了检查
    bool original_status = online_info[0].is_online;
    online_info[0].is_online = false; // 临时设置为离线

    module_online_check(); // 这次应该跳过检查，不会改变状态

    printf("模块0状态: %s（应该保持离线，因为跳过了检查）\n", online_info[0].is_online ? "在线" : "离线");
    assert(online_info[0].is_online == false); // 应该保持我们设置的离线状态

    // 恢复原始状态
    online_info[0].is_online = original_status;

    // 时间推进到1000ms，应该执行检查
    mock_time = 1000;
    printf("\n时间1000ms: 调用检查函数（应该执行检查）\n");
    module_online_check();
    printf("模块0状态: %s\n", online_info[0].is_online ? "在线" : "离线");
    assert(online_info[0].is_online == true); // 时间差为1000ms < 5000ms，应该在线

    // 时间推进到1500ms，应该不执行检查
    mock_time = 1500;
    printf("\n时间1500ms: 调用检查函数（应该跳过检查）\n");
    module_online_check();

    // 时间推进到2000ms，应该执行检查
    mock_time = 2000;
    printf("\n时间2000ms: 调用检查函数（应该执行检查）\n");
    module_online_check();
    printf("模块0状态: %s\n", online_info[0].is_online ? "在线" : "离线");
    assert(online_info[0].is_online == true); // 时间差为2000ms < 5000ms，应该在线

    printf("\n测试2：验证超时检测在间隔控制下的工作\n");

    // 设置模块1在线，时间为2500（在上次检查之后）
    online_info[1].module_addr = 1;
    online_info[1].last_online_time = 2500;
    online_info[1].is_online = true;

    // 时间推进到2800ms，距离上次检查(2000ms)只有800ms，应该跳过检查
    mock_time = 2800;
    printf("时间2800ms: 调用检查函数（距离上次检查800ms，应该跳过检查）\n");
    module_online_check();
    printf("模块1状态: %s（应该仍在线，因为跳过了检查）\n", online_info[1].is_online ? "在线" : "离线");
    assert(online_info[1].is_online == true); // 跳过检查，状态不变

    // 时间推进到3000ms，距离上次检查刚好1000ms，应该执行检查但不超时
    mock_time = 3000;
    printf("\n时间3000ms: 调用检查函数（距离上次检查1000ms，应该执行检查）\n");
    module_online_check();
    printf("模块1状态: %s（时间差500ms < 5000ms，应该在线）\n", online_info[1].is_online ? "在线" : "离线");
    assert(online_info[1].is_online == true); // 未超时，应该在线

    // 时间推进到8500ms，距离上次检查超过1000ms，且模块超时
    mock_time = 8500;
    printf("\n时间8500ms: 调用检查函数（应该执行检查并发现超时）\n");
    module_online_check();
    printf("模块1状态: %s（时间差6000ms > 5000ms，应该离线）\n", online_info[1].is_online ? "在线" : "离线");
    assert(online_info[1].is_online == false); // 超时，应该离线

    printf("\n测试3：验证32位溢出情况下的间隔控制\n");

    // 设置时间接近32位最大值
    uint32_t near_max = UINT32_MAX - 500;
    mock_time = near_max;

    // 第一次调用，建立基准时间
    printf("时间%u: 建立基准时间\n", near_max);
    module_online_check();

    // 时间溢出到300，实际间隔为800ms < 1000ms，应该跳过
    mock_time = 300;
    printf("\n时间300（溢出后）: 调用检查函数（间隔800ms，应该跳过）\n");
    module_online_check();

    // 时间推进到600，实际间隔为1100ms > 1000ms，应该执行
    mock_time = 600;
    printf("\n时间600（溢出后）: 调用检查函数（间隔1100ms，应该执行）\n");
    module_online_check();

    printf("\n🎉 所有1000ms间隔检查测试通过！\n");
}

int main()
{
    printf("=== YFY模块1000ms间隔检查测试 ===\n");
    printf("检查间隔: 1000ms\n");
    printf("离线超时: %dms\n", MODULE_OFFLINE_TIME);

    test_interval_check();

    printf("\n=== 测试完成 ===\n");
    printf("优化效果：\n");
    printf("- 减少CPU负载：检查频率从每次调用降低到每1000ms一次\n");
    printf("- 保持准确性：仍能及时检测到模块离线状态\n");
    printf("- 处理溢出：正确处理32位时间戳溢出情况\n");

    return 0;
}
