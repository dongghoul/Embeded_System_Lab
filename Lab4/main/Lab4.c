#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/FreeRTOSConfig.h"

#define NUM_TASK 3 // Number of functional tasks
#define MAX_REQUEST_ID (NUM_TASK + 2)
#define ALL_CHECKED (0 | ((1 << NUM_TASK) - 1)) // Flag to check if all tasks have inspected a request

// Queue handle for storing requests
QueueHandle_t requestQueue;

// Request structure
typedef struct {
    int taskID;
    int data;
    uint8_t flags;  // flags to track tasks that have inspected the request
} Request;

// Reception task function
void receptionTask(void *pvParameters)
{
    while (1)
    {
        // Simulate receiving requests from somewhere
        Request* newRequest = (Request*) malloc(sizeof(Request));
        newRequest->taskID = rand() % (MAX_REQUEST_ID);
        newRequest->data = rand() % 100;
        newRequest->flags = 0;

        // Send the request to the queue
        xQueueSend(requestQueue, (void *) &newRequest, portMAX_DELAY);
        printf("Sended request {ID: %d, data: %d} to queue\n", newRequest->taskID, newRequest->data);
    
        // Simulate delay between requests
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
    }
    vTaskDelete(NULL);
}

// Functional task function
void functionalTask(void *pvParameters)
{
    int taskID = (int)pvParameters;

    while (1)
    {
        Request* receivedRequest = NULL;

        // Peek at the next item in the queue without removing it
        if (xQueuePeek(requestQueue, &receivedRequest, portMAX_DELAY))
        {

            // Check if the request is for this task
            if (receivedRequest->taskID == taskID)
            {
                xQueueReceive(requestQueue, &receivedRequest, 0);

                printf("Task %d received request {ID: %d, data: %d}\n", taskID, receivedRequest->taskID, receivedRequest->data);
                // Handle the request here
                free(receivedRequest);
                receivedRequest = NULL;
            }
            else
            {
                // Ignore request
                printf("Task %d ignored request {ID: %d, data: %d}\n", taskID, receivedRequest->taskID, receivedRequest->data);

                // Signal the flag
                receivedRequest->flags |= ((1 << taskID));
                // printf("%d %d\n", receivedRequest->flags, ALL_CHECKED);

                // Remove the request from the queue if all tasks have inspected it
                if (receivedRequest->flags == ALL_CHECKED)
                {
                    xQueueReceive(requestQueue, &receivedRequest, 0);
                    // Raise an error and remove request
                    printf("Task %d removed request {ID: %d, data: %d} from queue\n", taskID, receivedRequest->taskID, receivedRequest->data);
                    free(receivedRequest);
                    receivedRequest = NULL;
                }

                vTaskDelay(1);
            }
        }
    }
    vTaskDelete(NULL);
}

void app_main()
{
    // Create the request queue
    requestQueue = xQueueCreate(10, sizeof(Request*));

    // Create the reception task
    xTaskCreate(receptionTask, "ReceptionTask", 2048, NULL, 1, NULL);

    // Create functional tasks
    for (int i = 0; i < NUM_TASK; i++)
    {
        xTaskCreate(functionalTask, "FunctionalTask", 2048, (void *)i, 1, NULL);
    }
}
