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

#define TASK1_PRIORITY 3
#define TASK2_PRIORITY 2
#define TASK3_PRIORITY 0
#define TASK3_SIMULATED_PROCESS_TIME 800

uint32_t task3_yield_timeout = 0;

char *task1 = "task1 is running";
char *task2 = "task2 is running";
char *task3 = "task3 is running";

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
        vTaskDelay( (/*rand() % */pdMS_TO_TICKS(3000)));
        printTask(task1, true);
    }
    vTaskDelete(NULL);
}

void vtask2(void *pvParameters)
{
    while (1)
    {
        vTaskDelay( (/*rand() % */pdMS_TO_TICKS(2000)));
        printTask(task2, true);
    }
    
    vTaskDelete(NULL);
}

void vtask3(void *pvParameters)
{
    //printTask(task3, true);
    while (1)
    {
        if (task3_yield_timeout <= xTaskGetTickCount()){
            taskYIELD();
            //delay yield time (blocking delay)
            task3_yield_timeout = xTaskGetTickCount() + TASK3_SIMULATED_PROCESS_TIME;
            printTask(task3, true);        
        }
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    xTaskCreate(vtask1, "task1", 2048, NULL, TASK1_PRIORITY, NULL);
    xTaskCreate(vtask2, "task2", 2048, NULL, TASK2_PRIORITY, NULL);
    xTaskCreate(vtask3, "task3", 2048, NULL, TASK3_PRIORITY, NULL);
}
