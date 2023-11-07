#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/FreeRTOSConfig.h"

#define NUM_TASK 3 // Number of functional tasks
#define MAX_REQUEST_ID NUM_TASK + 2

// Queue handle for storing requests
QueueHandle_t requestQueue;

// Request structure
typedef struct {
    int taskID;
    int data;
    int counter;  // Counter to track how many tasks have inspected the request
} Request;

// Reception task function
void receptionTask(void *pvParameters)
{
    while (1)
    {
        // Simulate receiving requests from somewhere
        Request newRequest;
        newRequest.taskID = rand() % (MAX_REQUEST_ID);
        newRequest.data = rand() % 100;
        newRequest.counter = NUM_TASK;

        // Send the request to the queue
        xQueueSend(requestQueue, &newRequest, portMAX_DELAY);
        printf("Sended request {ID: %d, data: %d} to queue\n", newRequest.taskID, newRequest.data);
    
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
        // Request *receivedRequest = (Request*) malloc(sizeof(Request));
        Request receivedRequest;

        // Peek at the next item in the queue without removing it
        if (xQueueReceive(requestQueue, &receivedRequest, portMAX_DELAY))
        {
            // Check if the request is for this task
            if (receivedRequest.taskID == taskID)
            {
                // xQueueReceive(requestQueue, &receivedRequest, 0);

                printf("Task %d received request {ID: %d, data: %d}\n", taskID, receivedRequest.taskID, receivedRequest.data);
                // Handle the request here
            }
            else
            {
                // Ignore request
                printf("Task %d ignored request {ID: %d, data: %d}\n", taskID, receivedRequest.taskID, receivedRequest.data);

                // Decrement the counter
                receivedRequest.counter--;
                // printf("%d %d\n", receivedRequest.counter, receivedRequest.data);

                // Remove the request from the queue if all tasks have inspected it
                // else send it back to the queue
                if (receivedRequest.counter <= 0)
                {
                    // xQueueReceive(requestQueue, receivedRequest, 0);
                    // Raise an error and remove request
                    printf("Task %d removed request {ID: %d, data: %d} from queue\n", taskID, receivedRequest.taskID, receivedRequest.data);
                }
                else 
                {
                    xQueueSendToFront(requestQueue, &receivedRequest, portMAX_DELAY);
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
    requestQueue = xQueueCreate(10, sizeof(Request));

    // Create the reception task
    xTaskCreate(receptionTask, "ReceptionTask", 2048, NULL, 1, NULL);

    // Create functional tasks
    for (int i = 0; i < NUM_TASK; i++)
    {
        xTaskCreate(functionalTask, "FunctionalTask", 2048, (void *)i, 1, NULL);
    }
}
