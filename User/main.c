#include "gd32h7xx.h"
#include "usart/debug_usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

void vTask1(void *pvParameters)
{
	while (1)
	{
		printf("Task1\n");
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

int main(void)
{
	uint8_t ucaRxBuf[256];
	uint16_t usRxCount = 0;

	nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);

	debug_usart_init();

	printf("你好\n");

	xTaskCreate(vTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
	vTaskStartScheduler();
	while(1);
}

#if(configCHECK_FOR_STACK_OVERFLOW)
void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName)
{
    for (;;) {}
}
#endif
