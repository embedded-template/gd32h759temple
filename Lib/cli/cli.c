/*
 * CLI 使用示例
 * 展示如何使用 FreeRTOS CLI 系统
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"

/* CLI includes */
#include "FreeRTOS_CLI.h"

/* 日志系统 */
#include "log.h"

#include "usart/console_usart.h"

/* 函数声明 */
static BaseType_t prvLedCommand(char* pcWriteBuffer, size_t xWriteBufferLen, const char* pcCommandString);
static BaseType_t prvInfoCommand(char* pcWriteBuffer, size_t xWriteBufferLen, const char* pcCommandString);
static BaseType_t prvMemoryCommand(char* pcWriteBuffer, size_t xWriteBufferLen, const char* pcCommandString);
static BaseType_t prvTaskStatsCommand(char* pcWriteBuffer, size_t xWriteBufferLen, const char* pcCommandString);
static BaseType_t prvSetCommand(char* pcWriteBuffer, size_t xWriteBufferLen, const char* pcCommandString);
static BaseType_t prvGetCommand(char* pcWriteBuffer, size_t xWriteBufferLen, const char* pcCommandString);

static const CLI_Command_Definition_t xInfoCommand = {
    "info", "\r\ninfo:\r\n 显示系统信息\r\n", prvInfoCommand, 0 /* 无参数 */
};

static const CLI_Command_Definition_t xMemoryCommand = {"memory", "\r\nmemory:\r\n 显示内存使用情况\r\n", prvMemoryCommand, 0};

static const CLI_Command_Definition_t xTaskStatsCommand = {"tasks", "\r\ntasks:\r\n 显示任务状态信息\r\n", prvTaskStatsCommand, 0};

/**
 * @brief CLI初始化
 *
 */
void CLI_Init(void)
{
    /* 注册命令 */
    FreeRTOS_CLIRegisterCommand(&xInfoCommand);
    FreeRTOS_CLIRegisterCommand(&xMemoryCommand);
    FreeRTOS_CLIRegisterCommand(&xTaskStatsCommand);
}

/**
 * @brief 系统信息命令处理函数
 */
static BaseType_t prvInfoCommand(char* pcWriteBuffer, size_t xWriteBufferLen, const char* pcCommandString)
{
    (void) pcCommandString;

    snprintf(pcWriteBuffer, xWriteBufferLen,
             "系统信息:\r\n"
             "  FreeRTOS版本: %s\r\n"
             "  系统运行时间: %lu ms\r\n"
             "  CPU频率: 600 MHz\r\n"
             "\r\n",
             tskKERNEL_VERSION_NUMBER, (unsigned long) (xTaskGetTickCount() * portTICK_PERIOD_MS));

    return pdFALSE;
}
/**
 * @brief 内存信息命令处理函数
 */
static BaseType_t prvMemoryCommand(char* pcWriteBuffer, size_t xWriteBufferLen, const char* pcCommandString)
{
    (void) pcCommandString;

    size_t xFreeHeapSpace = xPortGetFreeHeapSize();
    size_t xMinimumEverFreeHeapSpace = xPortGetMinimumEverFreeHeapSize();

    snprintf(pcWriteBuffer, xWriteBufferLen,
             "内存使用情况:\r\n"
             "  当前可用堆内存: %u 字节\r\n"
             "  历史最小可用堆内存: %u 字节\r\n"
             "  已使用堆内存: %u 字节\r\n"
             "\r\n",
             (unsigned int) xFreeHeapSpace, (unsigned int) xMinimumEverFreeHeapSpace, (unsigned int) (configTOTAL_HEAP_SIZE - xFreeHeapSpace));

    return pdFALSE;
}
/**
 * @brief 任务状态命令处理函数
 */
static BaseType_t prvTaskStatsCommand(char* pcWriteBuffer, size_t xWriteBufferLen, const char* pcCommandString)
{
    (void) pcCommandString;

    TaskStatus_t xTaskArray[6]; // 支持最多6个任务
    UBaseType_t uxArraySize;
    char* pcPtr = pcWriteBuffer;
    size_t xRemaining = xWriteBufferLen;
    int iLen;

    // 获取任务信息
    uxArraySize = uxTaskGetSystemState(xTaskArray, 6, NULL);

    // 输出每个任务
    for (UBaseType_t x = 0; x < uxArraySize && xRemaining > 50; x++)
    {
        char cStatus;
        switch (xTaskArray[x].eCurrentState)
        {
        case eRunning:
            cStatus = 'R';
            break;
        case eReady:
            cStatus = 'r';
            break;
        case eBlocked:
            cStatus = 'B';
            break;
        case eSuspended:
            cStatus = 'S';
            break;
        case eDeleted:
            cStatus = 'D';
            break;
        default:
            cStatus = '?';
            break;
        }

        iLen = snprintf(pcPtr, xRemaining,
                        "Task:%u\r\n"
                        "name:%s\r\n"
                        "p:%u\r\n"
                        "s:%u\r\n"
                        "st:%c\r\n"
                        "\r\n",
                        (unsigned int) (x + 1), xTaskArray[x].pcTaskName, (unsigned int) xTaskArray[x].uxCurrentPriority,
                        (unsigned int) xTaskArray[x].usStackHighWaterMark, cStatus);

        if (iLen < 0 || (size_t) iLen >= xRemaining) break;
        pcPtr += iLen;
        xRemaining -= iLen;
    }

    return pdFALSE;
}

/**
 * @brief CLI任务
 */
void CLI_Task(void* pvParameters)
{
    char cRxedChar;
    char cInputString[50] = {0};
    int cInputIndex = 0;
    BaseType_t xMoreDataToFollow;
    char* pcOutputString;
    pcOutputString = FreeRTOS_CLIGetOutputBuffer();

    CLI_Init();

    for (;;)
    {

        uart_handle_t* console = get_console();

        if (console->rx_ring_buffer->read((uint8_t*) &cRxedChar, 1, 10) == 1)
        {
            /* 处理接收到的字符 */
            if (cRxedChar == '\r' || cRxedChar == '\n')
            {

                if (cInputIndex > 0)
                {
                    cInputString[cInputIndex] = '\0';

                    do
                    {
                        /* 处理命令 */
                        xMoreDataToFollow = FreeRTOS_CLIProcessCommand(cInputString, pcOutputString, configCOMMAND_INT_MAX_OUTPUT_SIZE);

                        /* 输出结果 */
                        printf("%s", pcOutputString);

                    } while (xMoreDataToFollow != pdFALSE);

                    /* 重置输入缓冲区 */
                    cInputIndex = 0;
                    memset(cInputString, 0, sizeof(cInputString));
                }
            }
            else if (cInputIndex < (sizeof(cInputString) - 1))
            {
                /* 普通字符 */
                cInputString[cInputIndex] = cRxedChar;
                cInputIndex++;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(*(uint32_t*) pvParameters));
    }
}