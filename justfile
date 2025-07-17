# GD32H759 Temple Project Build System
# 使用 just 命令运行各种构建和开发任务

# 设置shell（Windows兼容）
set shell := ["powershell.exe", "-c"]

# 设置变量
CC := "gcc"
ARM_CC := "arm-none-eabi-gcc"
CFLAGS := "-Wall -Wextra -std=c99"
ARM_CFLAGS := "-mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard"

# 项目目录
PROJECT_DIR := "Project"
TOOLS_DIR := "tools"
LIB_DIR := "Lib"
DRIVER_DIR := "Driver"
USER_DIR := "User"

# 默认目标：显示帮助信息
default:
    @echo "GD32H759 Temple Project Build System"
    @echo "===================================="
    @echo ""
    @echo "🏗️  构建命令："
    @echo "  just build          - 使用Keil构建项目"
    @echo "  just build-debug    - 构建调试版本"
    @echo "  just build-release  - 构建发布版本"
    @echo "  just clean          - 清理构建文件"
    @echo ""
    @echo "🧪 测试命令："
    @echo "  just test           - 运行所有测试"
    @echo "  just test-yyln      - 测试YYLN模块"
    @echo "  just test-yfy       - 测试YFY模块"
    @echo "  just test-can       - 测试CAN模块"
    @echo "  just test-unit      - 运行单元测试"
    @echo ""
    @echo "🔧 开发工具："
    @echo "  just format         - 格式化代码"
    @echo "  just lint           - 代码静态检查"
    @echo "  just docs           - 生成文档"
    @echo "  just flash          - 烧录程序到MCU"
    @echo ""
    @echo "📊 分析工具："
    @echo "  just size           - 分析代码大小"
    @echo "  just deps           - 分析依赖关系"
    @echo "  just metrics        - 代码质量指标"
    @echo ""
    @echo "🛠️  维护命令："
    @echo "  just setup          - 初始化开发环境"
    @echo "  just check-tools    - 检查工具可用性"
    @echo "  just help           - 显示详细帮助"

# 显示详细帮助信息
help: default

# ===== 构建命令 =====

# 使用Keil构建项目
build:
    @echo "🏗️ 使用Keil构建项目..."
    @if (Test-Path "{{PROJECT_DIR}}/gd32h759temple.uvprojx") { \
        echo "找到Keil项目文件"; \
        echo "请在Keil uVision中打开项目并构建"; \
    } else { \
        echo "❌ 未找到Keil项目文件"; \
    }

# 构建调试版本
build-debug:
    @echo "🐛 构建调试版本..."
    @echo "请在Keil中选择Debug配置并构建"

# 构建发布版本
build-release:
    @echo "🚀 构建发布版本..."
    @echo "请在Keil中选择Release配置并构建"

# 清理构建文件
clean:
    @echo "🧹 清理构建文件..."
    @if (Test-Path "{{PROJECT_DIR}}/Output") { Remove-Item -Recurse -Force "{{PROJECT_DIR}}/Output/*" -ErrorAction SilentlyContinue }
    @if (Test-Path "{{PROJECT_DIR}}/Listings") { Remove-Item -Recurse -Force "{{PROJECT_DIR}}/Listings/*" -ErrorAction SilentlyContinue }
    @Remove-Item -Force "*.exe", "*.o", "*.elf", "*.hex", "*.bin" -Recurse -ErrorAction SilentlyContinue
    @echo "✅ 清理完成"

# ===== 测试命令 =====

# 运行所有测试
test: test-yyln test-yfy test-can
    @echo "🎉 所有测试完成！"

# 测试YYLN模块
test-yyln:
    @echo "🧪 测试YYLN模块..."
    @if (Test-Path "{{LIB_DIR}}/converter/yyln/test/test_yyln_interface.exe") { \
        echo "运行现有测试..."; \
        & "{{LIB_DIR}}/converter/yyln/test/test_yyln_interface.exe"; \
    } else { \
        echo "编译并运行测试..."; \
        Set-Location "{{LIB_DIR}}/converter/yyln/test"; \
        gcc -Wall -Wextra -std=c99 test_yyln_interface.c -o test_yyln_interface.exe; \
        if ($LASTEXITCODE -eq 0) { ./test_yyln_interface.exe }; \
        Set-Location "../../../.."; \
    }

# 测试YFY模块
test-yfy:
    @echo "🧪 测试YFY模块..."
    @Push-Location "{{LIB_DIR}}/converter/yfy/test"
    @if (Test-Path "run_tests.exe") { \
        echo "运行现有测试..."; \
        ./run_tests.exe; \
    } else { \
        echo "编译并运行测试..."; \
        gcc -I../../.. -I.. -I../../ ../yfy_data.c ../yfy_interface.c run_tests.c -o run_tests.exe; \
        if ($?) { ./run_tests.exe }; \
    }
    @Pop-Location

# 测试CAN模块
test-can:
    @echo "🧪 测试CAN模块..."
    @echo "CAN模块测试需要硬件支持，请在目标硬件上运行"

# 运行单元测试
test-unit:
    @echo "🧪 运行单元测试..."
    @just test-yyln
    @just test-yfy

# ===== 开发工具 =====

# 格式化代码
format:
    @echo "🎨 格式化代码..."
    @if (Get-Command "clang-format" -ErrorAction SilentlyContinue) { \
        echo "使用clang-format格式化代码..."; \
        Get-ChildItem -Recurse -Path "{{DRIVER_DIR}}", "{{LIB_DIR}}", "{{USER_DIR}}" -Include "*.c", "*.h" -ErrorAction SilentlyContinue | ForEach-Object { \
            clang-format -i --style=file:"{{TOOLS_DIR}}/code.clang-format" $_.FullName; \
        }; \
        echo "代码格式化完成"; \
    } else { \
        echo "⚠️ clang-format未安装，跳过代码格式化"; \
    }

# 代码静态检查
lint:
    @echo "🔍 代码静态检查..."
    @echo "运行静态分析工具..."
    @if (Get-Command "cppcheck" -ErrorAction SilentlyContinue) { \
        cppcheck --enable=all --std=c99 "{{DRIVER_DIR}}" "{{LIB_DIR}}" "{{USER_DIR}}" --suppress=missingIncludeSystem; \
    } else { \
        echo "⚠️ cppcheck未安装，跳过静态检查"; \
    }

# 生成文档
docs:
    @echo "📚 生成文档..."
    @if (Get-Command "doxygen" -ErrorAction SilentlyContinue) { \
        echo "使用Doxygen生成文档..."; \
        if (Test-Path "Doxyfile") { doxygen Doxyfile } else { echo "未找到Doxyfile配置" }; \
    } else { \
        echo "⚠️ Doxygen未安装，跳过文档生成"; \
    }

# 烧录程序到MCU
flash:
    @echo "⚡ 烧录程序到MCU..."
    @if (Test-Path "{{PROJECT_DIR}}/Output/gh32h759temple.hex") { \
        echo "找到hex文件，准备烧录..."; \
        echo "请使用J-Link或ST-Link工具烧录"; \
    } else { \
        echo "❌ 未找到hex文件，请先构建项目"; \
    }

# ===== 分析工具 =====

# 分析代码大小
size:
    @echo "📊 分析代码大小..."
    @if (Test-Path "{{PROJECT_DIR}}/Output/gh32h759temple.axf") { \
        echo "分析ELF文件大小..."; \
        if (Get-Command "arm-none-eabi-size" -ErrorAction SilentlyContinue) { \
            arm-none-eabi-size "{{PROJECT_DIR}}/Output/gh32h759temple.axf"; \
        } else { \
            echo "⚠️ arm-none-eabi-size未安装"; \
        }; \
    } else { \
        echo "❌ 未找到ELF文件，请先构建项目"; \
    }

# 分析依赖关系
deps:
    @echo "🔗 分析依赖关系..."
    @echo "分析头文件依赖..."
    @Get-ChildItem -Recurse -Include "*.c", "*.h" "{{DRIVER_DIR}}", "{{LIB_DIR}}", "{{USER_DIR}}" | ForEach-Object { \
        $includes = Select-String -Path $_.FullName -Pattern '#include\s*[<"]([^>"]+)[>"]' -AllMatches; \
        if ($includes) { \
            Write-Host "$($_.Name):"; \
            $includes.Matches | ForEach-Object { Write-Host "  $($_.Groups[1].Value)" }; \
        } \
    }

# 代码质量指标
metrics:
    @echo "📈 代码质量指标..."
    @echo "统计代码行数..."
    @$cFiles = Get-ChildItem -Recurse -Path "{{DRIVER_DIR}}", "{{LIB_DIR}}", "{{USER_DIR}}" -Include "*.c" -ErrorAction SilentlyContinue
    @$hFiles = Get-ChildItem -Recurse -Path "{{DRIVER_DIR}}", "{{LIB_DIR}}", "{{USER_DIR}}" -Include "*.h" -ErrorAction SilentlyContinue
    @echo "C文件: $($cFiles.Count) 个"
    @echo "H文件: $($hFiles.Count) 个"
    @echo "总计: $($cFiles.Count + $hFiles.Count) 个文件"

# ===== 维护命令 =====

# 初始化开发环境
setup:
    @echo "🛠️ 初始化开发环境..."
    @echo "检查必要工具..."
    @just check-tools
    @echo "创建必要目录..."
    @if (!(Test-Path "build")) { New-Item -ItemType Directory -Path "build" }
    @if (!(Test-Path "docs")) { New-Item -ItemType Directory -Path "docs" }
    @echo "✅ 开发环境初始化完成"

# 检查工具可用性
check-tools:
    @echo "🔧 检查工具可用性..."
    @$tools = @("gcc", "arm-none-eabi-gcc", "clang-format", "cppcheck", "doxygen", "git")
    @foreach ($tool in $tools) { \
        if (Get-Command $tool -ErrorAction SilentlyContinue) { \
            echo "✅ $tool 可用"; \
        } else { \
            echo "❌ $tool 未安装"; \
        } \
    }

# ===== 快捷命令 =====

# 快速测试（开发时使用）
quick-test: test-yyln

# 完整验证（提交前使用）
verify: format lint test
    @echo "🎉 完整验证完成！"

# 发布准备
release-prep: clean build-release test size
    @echo "🚀 发布准备完成！"

# 开发者日常工作流
dev: format quick-test
    @echo "👨‍💻 开发者工作流完成！"

# ===== 调试命令 =====

# 启动调试会话
debug:
    @echo "🐛 启动调试会话..."
    @echo "请在Keil uVision中启动调试会话"

# 查看内存使用
memory:
    @echo "💾 查看内存使用..."
    @if (Test-Path "{{PROJECT_DIR}}/Listings/gh32h759temple.map") { \
        echo "分析MAP文件..."; \
        Select-String -Path "{{PROJECT_DIR}}/Listings/gh32h759temple.map" -Pattern "Total RO Size|Total RW Size|Total ROM Size"; \
    } else { \
        echo "❌ 未找到MAP文件，请先构建项目"; \
    }

# ===== 信息命令 =====

# 显示项目信息
info:
    @echo "ℹ️ 项目信息："
    @echo "项目名称: GD32H759 Temple"
    @echo "MCU型号: GD32H759"
    @echo "架构: ARM Cortex-M7"
    @echo "RTOS: FreeRTOS"
    @echo "IDE: Keil uVision"
    @echo "编译器: ARM Compiler"
    @echo ""
    @echo "目录结构："
    @echo "  Driver/     - 硬件驱动"
    @echo "  Lib/        - 库文件"
    @echo "  User/       - 用户代码"
    @echo "  Project/    - Keil项目文件"
    @echo "  Firmware/   - 固件库"
    @echo "  FreeRtos/   - RTOS内核"

# 显示构建状态
status:
    @echo "📊 构建状态："
    @if (Test-Path "{{PROJECT_DIR}}/Output/gh32h759temple.axf") { \
        $file = Get-Item "{{PROJECT_DIR}}/Output/gh32h759temple.axf"; \
        echo "✅ 最后构建: $($file.LastWriteTime)"; \
        echo "📁 文件大小: $([math]::Round($file.Length/1KB, 2)) KB"; \
    } else { \
        echo "❌ 项目未构建"; \
    }
    @if (Test-Path "{{PROJECT_DIR}}/Output/gh32h759temple.hex") { \
        echo "✅ HEX文件已生成"; \
    } else { \
        echo "❌ HEX文件未生成"; \
    }

# ===== 高级功能 =====

# 代码覆盖率分析
coverage:
    @echo "📊 代码覆盖率分析..."
    @echo "需要在测试环境中运行以获取覆盖率数据"

# 性能分析
profile:
    @echo "⚡ 性能分析..."
    @echo "请在目标硬件上运行性能分析工具"

# 生成发布包
package: clean build-release
    @echo "📦 生成发布包..."
    @$date = Get-Date -Format "yyyyMMdd_HHmmss"
    @$packageName = "gd32h759temple_$date"
    @if (!(Test-Path "release")) { New-Item -ItemType Directory -Path "release" }
    @if (Test-Path "{{PROJECT_DIR}}/Output/gh32h759temple.hex") { \
        Copy-Item "{{PROJECT_DIR}}/Output/gh32h759temple.hex" "release/$packageName.hex"; \
        echo "✅ 发布包已生成: release/$packageName.hex"; \
    } else { \
        echo "❌ 构建失败，无法生成发布包"; \
    }

# 备份项目
backup:
    @echo "💾 备份项目..."
    @$date = Get-Date -Format "yyyyMMdd_HHmmss"
    @$backupName = "gd32h759temple_backup_$date.zip"
    @if (Get-Command "7z" -ErrorAction SilentlyContinue) { \
        7z a $backupName . -xr!*.o -xr!*.exe -xr!Output -xr!Listings -xr!.git; \
        echo "✅ 备份完成: $backupName"; \
    } else { \
        echo "⚠️ 7z未安装，请手动备份项目"; \
    }

# 检查代码风格
style-check:
    @echo "🎨 检查代码风格..."
    @cd "{{TOOLS_DIR}}" && just format
    @echo "代码风格检查完成"

# 生成版本信息
version:
    @echo "🏷️ 生成版本信息..."
    @if (Get-Command "git" -ErrorAction SilentlyContinue) { \
        $commit = git rev-parse --short HEAD; \
        $branch = git rev-parse --abbrev-ref HEAD; \
        $date = Get-Date -Format "yyyy-MM-dd HH:mm:ss"; \
        echo "版本信息:"; \
        echo "  分支: $branch"; \
        echo "  提交: $commit"; \
        echo "  日期: $date"; \
    } else { \
        echo "⚠️ Git未安装，无法获取版本信息"; \
    }

# 环境诊断
diagnose:
    @echo "🔍 环境诊断..."
    @echo "系统信息:"
    @echo "  操作系统: $(Get-CimInstance Win32_OperatingSystem | Select-Object -ExpandProperty Caption)"
    @echo "  PowerShell版本: $($PSVersionTable.PSVersion)"
    @echo ""
    @just check-tools
    @echo ""
    @just status

# 清理所有临时文件
deep-clean: clean
    @echo "🧹 深度清理..."
    @Remove-Item -Recurse -Force ".vs", "*.tmp", "*.bak", "*.log" -ErrorAction SilentlyContinue
    @Get-ChildItem -Recurse -Include "*.orig", "*.rej" | Remove-Item -Force -ErrorAction SilentlyContinue
    @echo "✅ 深度清理完成"

# 重置开发环境
reset: deep-clean setup
    @echo "🔄 开发环境已重置"

# ===== 别名和快捷方式 =====

# 常用别名
b := "build"
t := "test"
f := "format"
c := "clean"
h := "help"

# 快速构建和测试
bt: build test

# 快速格式化和测试
ft: format test

# 完整工作流
full: clean format build test
    @echo "🎉 完整工作流完成！"
