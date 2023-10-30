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
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOSConfig.h"

#define TASK1_PRIORITY_1 5
#define TASK2_PRIORITY_2 0
#define IDLE_TASK_MAXCOUNT 100
#define TASK2_MAXCOUNT 10000000

volatile uint32_t ulIdleCycleCount = IDLE_TASK_MAXCOUNT ;
volatile uint32_t ulTask2CycleCount = TASK2_MAXCOUNT ;

void vApplicationIdleHook ( void );

char *task1 = "task1 preempt";
char *task2 = "task2 starts counting again";
char *idle = "idle task starts counting again";

int task2Count = 0;
int idleCount = 0;

void printTask(char *s, bool time)
{
    if (time) {
        printf("%ld: %s\n", xTaskGetTickCount(), s);
    }
    else {
        printf("%s\n", s);
    }
}

void vtask1(void *pvParameters)
{
    while (1)
    {
        vTaskDelay( (/*rand() % */pdMS_TO_TICKS(2000)));
        printTask(task1, true);
    }
    vTaskDelete(NULL);
}



void vtask2(void *pvParameters)
{
    while (1)
    {

        ulTask2CycleCount ++;
        if (ulTask2CycleCount >= TASK2_MAXCOUNT)
        {
            // task2Count++;
            printTask(task2, true);
            ulTask2CycleCount = 0;
        
        }
    }
    
    vTaskDelete(NULL);
}

void vApplicationIdleHook ( void )
{
    ulIdleCycleCount ++;
    if (ulIdleCycleCount >= IDLE_TASK_MAXCOUNT)
    {
        // idleCount++;
        printTask(idle, true);
        ulIdleCycleCount = 0;
    }
}

void app_main(void)
{
    xTaskCreate(vtask1, "event_task", 4096, NULL, 5, NULL);
    xTaskCreate(vtask2, "idle_task2", 4096, NULL, 0, NULL);
}
