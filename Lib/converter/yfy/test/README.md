# YFY模块测试套件

本目录包含YFY模块的完整测试套件，提供简洁但全面的单元测试覆盖。

## 测试文件说明

### 1. `run_tests.c` - 主要测试套件 ⭐
**推荐使用**：简洁高效的测试套件，适合日常开发使用。

```bash
# 编译
gcc -I../../../utilities -I.. ../yfy_data.c ../yfy_interface.c run_tests.c -o run_tests.exe

# 运行完整测试
./run_tests.exe

# 运行快速冒烟测试
./run_tests.exe quick
```

**测试覆盖**：
- ✅ 基本功能测试（电压、电流、组号、温度）
- ✅ 状态位读取测试（所有状态表）
- ✅ 电压电流测试（交流输入、最大最小值）
- ✅ 组数据测试（组电压、电流、模块数）
- ✅ 系统数据测试（系统级参数）
- ✅ 32位宏测试（数据组装和提取）
- ✅ 边界条件测试（地址越界、空指针）
- ✅ 数据类型测试（float、uint8_t、uint16_t、int8_t）

### 2. `unit_test.c` - 详细单元测试
提供详细的测试输出和错误信息，适合调试使用。

```bash
# 编译运行
gcc -I../../../utilities -I.. ../yfy_data.c ../yfy_interface.c unit_test.c -o unit_test.exe && ./unit_test.exe
```

### 3. `comprehensive_test.c` - 综合性能测试
包含压力测试、性能测试和随机数据测试。

```bash
# 编译运行
gcc -I../../../utilities -I.. ../yfy_data.c ../yfy_interface.c comprehensive_test.c -o comprehensive_test.exe && ./comprehensive_test.exe
```

**特色功能**：
- 🚀 压力测试（10万次读取操作）
- ⏱️ 性能测试（计时统计）
- 🎲 随机数据测试
- 📊 详细性能报告

### 4. `justfile` - 构建自动化 ⭐
**现代化构建工具**：使用Just命令运行器自动化所有构建和测试任务。

```bash
# 安装Just (如果未安装)
# Windows: scoop install just
# macOS: brew install just
# Linux: cargo install just

# 查看所有可用命令
just

# 常用命令
just test           # 主要测试套件
just test-quick     # 快速冒烟测试
just test-all       # 所有测试
just build          # 编译所有程序
just clean          # 清理构建文件
just info           # 显示构建信息
```

**Just命令完整列表**：
- `just build` - 编译所有测试程序
- `just test` - 运行主要测试套件
- `just test-quick` - 快速冒烟测试
- `just test-full` - 详细单元测试
- `just test-perf` - 性能测试
- `just test-macro` - 32位宏测试
- `just test-all` - 运行所有测试
- `just demo-macro` - 宏使用示例
- `just clean` - 清理构建文件
- `just rebuild` - 重新构建
- `just ci` - 持续集成测试
- `just dev` - 开发者快速验证
- `just info` - 显示构建信息

### 5. 其他测试文件
- `test_macro_only.c` - 专门测试32位宏
- `macro_usage_example.c` - 宏使用示例
- `test_header_completeness.c` - 头文件完整性测试

## 测试结果示例

### 完整测试输出
```
=== YFY模块测试套件 ===
运行完整测试套件...
运行 test_basic_functions... ✓ 通过
运行 test_status_functions... ✓ 通过
运行 test_voltage_current... ✓ 通过
运行 test_group_functions... ✓ 通过
运行 test_system_functions... ✓ 通过
运行 test_32bit_macros... ✓ 通过
运行 test_boundary_conditions... ✓ 通过
运行 test_data_types... ✓ 通过

=== 测试结果 ===
总计: 8, 通过: 8, 失败: 0
成功率: 100.0%
🎉 所有测试通过！
```

### 性能测试输出
```
=== 压力测试：读取操作 ===
完成 90000 次读取操作，耗时: 16.00 ms
平均每次读取: 0.0002 ms

=== 32位宏性能测试 ===
完成 600000 次宏操作，耗时: 16.00 ms
平均每次宏操作: 0.000027 ms
```

## 测试覆盖统计

### 函数覆盖率：100%
- **第一类函数**：47个（单个模块数据读取）
- **第二类函数**：3个（组数据读取）
- **第三类函数**：3个（系统数据读取）
- **32位宏**：6个（数据组装和提取）

### 测试类型覆盖
- ✅ **功能测试**：所有API函数
- ✅ **边界测试**：地址越界、组号越界
- ✅ **错误处理**：空指针、无效参数
- ✅ **数据类型**：float、uint8_t、uint16_t、int8_t
- ✅ **性能测试**：大量操作压力测试
- ✅ **一致性测试**：多次读取数据一致性

## 使用建议

### 使用Just构建工具 ⭐ (推荐)
```bash
# 显示所有可用命令
just

# 快速验证功能
just test-quick

# 完整功能测试
just test

# 运行所有测试
just test-all

# 性能测试
just test-perf

# 清理构建文件
just clean
```

### 传统编译方式
```bash
# 快速验证功能
./run_tests.exe quick

# 完整功能测试
./run_tests.exe
```

### 性能调优
```bash
# 使用Just
just test-perf

# 传统方式
./comprehensive_test.exe
```

### 调试问题
```bash
# 使用Just
just test-full

# 传统方式
./unit_test.exe
```

## 测试数据说明

测试使用标准化的测试数据：
- **模块0**：电压48.0V，电流10.0A，组号1，温度25°C
- **组1**：电压48.0V，电流100.0A，模块数4
- **系统**：电压480.0V，电流1000.0A，模块数12

## 编译要求

- GCC编译器
- 包含路径：`-I../../../utilities -I..`
- 链接文件：`../yfy_data.c ../yfy_interface.c`

## 返回值说明

- **0**：所有测试通过
- **1**：有测试失败

## 注意事项

1. 测试前会自动初始化测试数据
2. 所有测试都是非破坏性的
3. 测试覆盖了所有公开API
4. 支持快速测试和完整测试两种模式
