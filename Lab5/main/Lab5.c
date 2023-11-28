#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

TimerHandle_t timer1; // Timer for "ahihi"
TimerHandle_t timer2; // Timer for "ihaha"

int count1 = 0; // Count for timer1
int count2 = 0; // Count for timer2
int count1_flag = 0;
int count2_flag = 0;

void timerCallback(TimerHandle_t xTimer) {
    if (xTimer == timer1 && count1 < 10) {
        printf("%ld ahihi\n", xTaskGetTickCount());
        count1++;
    }

    if (xTimer == timer2 && count2 < 5) {
        printf("%ld ihaha\n", xTaskGetTickCount());        
        count2++;
    }

    // If the count limit is reached, stop the timer
    if (count1 == 10 && count1_flag == 0) {
        xTimerStop(timer1, 0);
        count1_flag = 1;
        printf("Timer1 stopped after 10 times.\n");
    }

    if (count2 == 5 && count2_flag == 0) {
        xTimerStop(timer2, 0);
        count2_flag = 1;
        printf("Timer2 stopped after 5 times.\n");
    }
}

void app_main() {
    // Create the first timer for "ahihi"
    timer1 = xTimerCreate("Timer1", pdMS_TO_TICKS(2000), pdTRUE, (void*)0, timerCallback);
    if (timer1 == NULL) {
        printf("Failed to create Timer1\n");
    }

    // Create the second timer for "ihaha"
    timer2 = xTimerCreate("Timer2", pdMS_TO_TICKS(3000), pdTRUE, (void*)0, timerCallback);
    if (timer2 == NULL) {
        printf("Failed to create Timer2\n");
    }

    // Start the timers
    if (xTimerStart(timer1, 0) == pdPASS) {
        printf("Timer1 started\n");
    }

    if (xTimerStart(timer2, 0) == pdPASS) {
        printf("Timer2 started\n");
    }
}