#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"

#include "driver/gpio.h"
#include "sdkconfig.h"

#include "esp_camera.h"

// user include
#include "camera.h"

/* task function structure exam. */
void take_picture(void *parameter)
{
    while (1)
    {
        func_take_picture();
        // test_take_picture();
        // printf("test\n");
    }
    // ensure the task exit is clean.
    vTaskDelete(NULL);
}


void app_main(void)
{
    xTaskCreate(take_picture, "camera", 5000, NULL, 1, NULL);
}
