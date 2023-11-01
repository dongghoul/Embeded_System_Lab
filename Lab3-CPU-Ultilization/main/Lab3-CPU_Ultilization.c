/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "freertos/FreeRTOSConfig.h"

#define TASK_MAXCOUNT 10000000

volatile uint32_t ulTask1Count = 0 ;
volatile uint32_t ulTask2Count = 0 ;

typedef struct {
    uint32_t total;
    uint32_t idle;
} cpu_usage_t;

cpu_usage_t cpu_usage[2];

void vApplicationTickHook( void )
{
    cpu_usage[xPortGetCoreID()].total++;
}

void vApplicationIdleHook ( void )
{
    cpu_usage[xPortGetCoreID()].idle++;
}

void cpu_monitor_task(void *pvParameter)
{
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(2000));
        printf("Core 0: %.2f%%\t Core 1: %.2f%%\n", 
                cpu_usage[0].idle * 1.0 / cpu_usage[0].total,
                cpu_usage[1].idle * 1.0 / cpu_usage[1].total);
        // printf("%ld/%ld - %ld/%ld\n", cpu_usage[0].idle, cpu_usage[0].total, cpu_usage[1].idle, cpu_usage[1].total);
    }
}

void countingTask(void *pvParameter)
{
    uint32_t *count;
    count = (uint32_t*) pvParameter;
    while (1){
        (*count)++;
        if (*count >= TASK_MAXCOUNT){
            printf("Task finished counting at core %d\n", xPortGetCoreID());
            *count = 0;
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    xTaskCreate(&cpu_monitor_task, "cpu_monitor_task", 2048, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(countingTask, "task1", 2048, (void*)&ulTask1Count, 1, NULL);
    xTaskCreate(countingTask, "task2", 2048, (void*)&ulTask2Count, 1, NULL);
}
