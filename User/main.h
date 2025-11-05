#pragma once
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>

typedef struct task_info_t
{
    TaskFunction_t pxTaskCode;
    const char* const pcName;
    const configSTACK_DEPTH_TYPE uxStackDepth;
    uint32_t time;
    UBaseType_t uxPriority;
    TaskHandle_t* const pxCreatedTask;
} task_info_t;

#define NAME_cli_task "cli"
#define STACK_cli_task 200
#define PARAM_cli_task 10
#define PRIORITY_cli_task 3

#define NAME_tcp_client_task "tcp_client"
#define STACK_tcp_client_task 1000
#define PARAM_tcp_client_task 2000
#define PRIORITY_tcp_client_task 3

#define USE_ENET0
#ifndef USE_ENET0
    #define USE_ENET1
#endif
#ifdef USE_ENET0
    #undef USE_ENET1
#endif


/* MAC address: MAC_ADDR0:MAC_ADDR1:MAC_ADDR2:MAC_ADDR3:MAC_ADDR4:MAC_ADDR5 */
#define MAC_ADDR0   4
#define MAC_ADDR1   0xA
#define MAC_ADDR2   0xD
#define MAC_ADDR3   0xE
#define MAC_ADDR4   0xD
#define MAC_ADDR5   6

/* static IP address: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   103
#define IP_ADDR3   213

/* remote IP address: IP_S_ADDR0.IP_S_ADDR1.IP_S_ADDR2.IP_S_ADDR3 */
#define IP_S_ADDR0   192
#define IP_S_ADDR1   168
#define IP_S_ADDR2   103
#define IP_S_ADDR3   19
#define IP_S_PORT    8080

/* net mask */
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/* gateway address */
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   103
#define GW_ADDR3   254

/* MII and RMII mode selection */
#define RMII_MODE  // user have to provide the 50 MHz clock by soldering a 50 MHz oscillator
//#define MII_MODE

/* clock the PHY from external 25MHz crystal (only for MII mode) */
#ifdef  MII_MODE
#define PHY_CLOCK_MCO
#endif
