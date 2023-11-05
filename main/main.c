#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_camera.h"

void testTask(void *parameter)
{
    while (1) 
    {
        int a = 1;
        int b[1];     // totally 400 bytes > (1024 - 768 = 256) task allocated

        for (int i = 0; i < 1; i++)
        {
            b[i] = a + 1;
        }

        printf("%d\n", b[0]);
    }
}

/* task function structure exam. */
void goodLuck_Task(void *parameter)
{
    // printf("High water mark (words)");
    // printf(uxTaskGetStackHighWaterMark(NULL));
    while (1)
    {
        // get heap size at beginning.
        // printf("Heap before malloc (bytes):  ");
        // printf(xPortGetFreeHeapSize());
        
        printf("good luck\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        // get heap size in the end.
        // printf("Heap after malloc (bytes):  ");
        // printf(xPortGetFreeHeapSize());
    }
    // ensure the task exit is clean.
    vTaskDelete(NULL);
}

void app_main(void)
{
    // test
    // xTaskCreate(testTask, "Test Task", 1024, NULL, 1, NULL);

    

    /*
    int *ptr = (int*)pvPortMalloc(1024 * sizeof(int));
    for (int i = 0; i < 1024; i++)
    {
        ptr[i] = 3;
    }
    */

    xTaskCreate(goodLuck_Task, "goodLuck", 1500, NULL, 1, NULL);
}
