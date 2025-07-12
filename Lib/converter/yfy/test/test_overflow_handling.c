#include "../yfy_data.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

// 模拟时间函数
static uint32_t mock_time = 0;

bool mock_time_func(void)
{
    return mock_time;
}

// 测试32位时间溢出处理
void test_time_overflow_handling()
{
    printf("=== 32位时间溢出处理测试 ===\n");

    // 初始化模块句柄
    yfy_module_handle_init(NULL, NULL, mock_time_func);

    // 获取在线信息数组（需要外部访问）
    extern module_online_info_t stModuleOnlineInfo[MODULE_NUM];

    printf("\n测试1：正常时间递增情况\n");
    // 设置模块0在线，时间为1000
    mock_time = 1000;
    stModuleOnlineInfo[0].module_addr = 0;
    stModuleOnlineInfo[0].last_online_time = 1000;
    stModuleOnlineInfo[0].is_online = true;

    // 时间推进到3000（差值2000 < 5000，应该仍在线）
    mock_time = 3000;
    yfy_process_data();
    printf("时间从1000推进到3000，模块应该在线: %s\n", stModuleOnlineInfo[0].is_online ? "在线" : "离线");
    assert(stModuleOnlineInfo[0].is_online == true);

    // 时间推进到7000（差值6000 > 5000，应该离线）
    mock_time = 7000;
    yfy_process_data();
    printf("时间从1000推进到7000，模块应该离线: %s\n", stModuleOnlineInfo[0].is_online ? "在线" : "离线");
    assert(stModuleOnlineInfo[0].is_online == false);

    printf("\n测试2：32位时间溢出情况\n");
    // 设置模块1在线，时间接近32位最大值
    uint32_t near_max = UINT32_MAX - 2000; // 距离溢出还有2000
    mock_time = near_max;
    stModuleOnlineInfo[1].module_addr = 1;
    stModuleOnlineInfo[1].last_online_time = near_max;
    stModuleOnlineInfo[1].is_online = true;

    // 时间溢出到1000（实际时间差为3000 < 5000，应该仍在线）
    mock_time = 1000;
    yfy_process_data();
    printf("时间从%u溢出到1000，实际时间差3000，模块应该在线: %s\n", near_max, stModuleOnlineInfo[1].is_online ? "在线" : "离线");
    assert(stModuleOnlineInfo[1].is_online == true);

    // 时间继续到6000（实际时间差为8000 > 5000，应该离线）
    mock_time = 6000;
    yfy_process_data();
    printf("时间从%u溢出到6000，实际时间差8000，模块应该离线: %s\n", near_max, stModuleOnlineInfo[1].is_online ? "在线" : "离线");
    assert(stModuleOnlineInfo[1].is_online == false);

    printf("\n测试3：边界情况 - 刚好溢出\n");
    // 设置模块2在线，时间为最大值
    mock_time = UINT32_MAX;
    stModuleOnlineInfo[2].module_addr = 2;
    stModuleOnlineInfo[2].last_online_time = UINT32_MAX;
    stModuleOnlineInfo[2].is_online = true;

    // 时间溢出到0（实际时间差为1 < 5000，应该仍在线）
    mock_time = 0;
    yfy_process_data();
    printf("时间从%u溢出到0，实际时间差1，模块应该在线: %s\n", UINT32_MAX, stModuleOnlineInfo[2].is_online ? "在线" : "离线");
    assert(stModuleOnlineInfo[2].is_online == true);

    printf("\n测试4：大跨度溢出情况\n");
    // 设置模块3在线，时间为最大值-1000
    uint32_t test_time = UINT32_MAX - 1000;
    mock_time = test_time;
    stModuleOnlineInfo[3].module_addr = 3;
    stModuleOnlineInfo[3].last_online_time = test_time;
    stModuleOnlineInfo[3].is_online = true;

    // 时间溢出到4000（实际时间差为5000，刚好等于超时时间）
    mock_time = 4000;
    yfy_process_data();
    printf("时间从%u溢出到4000，实际时间差5000，模块应该离线: %s\n", test_time, stModuleOnlineInfo[3].is_online ? "在线" : "离线");
    assert(stModuleOnlineInfo[3].is_online == false);

    printf("\n测试5：验证时间差计算公式\n");
    // 测试各种溢出情况的时间差计算
    struct
    {
        uint32_t last_time;
        uint32_t current_time;
        uint32_t expected_diff;
        const char* description;
    } test_cases[] = {{1000, 3000, 2000, "正常递增"},
                      {UINT32_MAX - 1000, 1000, 2001, "小跨度溢出"},
                      {UINT32_MAX, 0, 1, "刚好溢出"},
                      {UINT32_MAX - 5000, 0, 5001, "中等跨度溢出"},
                      {0, UINT32_MAX, UINT32_MAX, "最大时间差（无溢出）"}};

    for (int i = 0; i < 5; i++)
    {
        uint32_t last_time = test_cases[i].last_time;
        uint32_t current_time = test_cases[i].current_time;
        uint32_t expected_diff = test_cases[i].expected_diff;
        uint32_t actual_diff;

        if (current_time >= last_time)
        {
            actual_diff = current_time - last_time;
        }
        else
        {
            actual_diff = (UINT32_MAX - last_time) + current_time + 1;
        }

        printf("%s: 从%u到%u，期望差值%u，实际差值%u %s\n", test_cases[i].description, last_time, current_time, expected_diff, actual_diff,
               (actual_diff == expected_diff) ? "✓" : "✗");
        assert(actual_diff == expected_diff);
    }

    printf("\n🎉 所有32位时间溢出处理测试通过！\n");
}

int main()
{
    printf("=== YFY模块32位时间溢出处理测试 ===\n");
    printf("MODULE_OFFLINE_TIME = %d ms\n", MODULE_OFFLINE_TIME);
    printf("UINT32_MAX = %u\n", UINT32_MAX);

    test_time_overflow_handling();

    printf("\n=== 测试完成 ===\n");
    return 0;
}
