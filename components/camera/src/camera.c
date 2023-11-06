#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"

#include <esp_log.h>
#include "camera.h"
#include "esp_camera.h"

static bool auto_jpeg_support = false; // whether the camera sensor support compression or JPEG encode
static const char *TAG = "example:take_picture";

static esp_err_t init_camera(uint32_t xclk_freq_hz, pixformat_t pixel_format, framesize_t frame_size, uint8_t fb_count)
{
    // AIthinker esp32cam
    camera_config_t camera_config = {
        .pin_pwdn = 32,
        .pin_reset = -1,
        .pin_xclk = 0,
        .pin_sscb_sda = 26,
        .pin_sscb_scl = 27,

        .pin_d7 = 35,
        .pin_d6 = 34,
        .pin_d5 = 39,
        .pin_d4 = 36,
        .pin_d3 = 21,
        .pin_d2 = 19,
        .pin_d1 = 18,
        .pin_d0 = 5,
        .pin_vsync = 25,
        .pin_href = 23,
        .pin_pclk = 22,

        // EXPERIMENTAL: Set to 16MHz on ESP32-S2 or ESP32-S3 to enable EDMA mode.
        .xclk_freq_hz = xclk_freq_hz,
        .ledc_timer = LEDC_TIMER_0, // This is only valid on ESP32/ESP32-S2. ESP32-S3 use LCD_CAM interface.
        .ledc_channel = LEDC_CHANNEL_0,

        .pixel_format = pixel_format, // YUV422,GRAYSCALE,RGB565,JPEG
        .frame_size = frame_size,    // QQVGA-UXGA, sizes above QVGA are not been recommended when not JPEG format.

        .jpeg_quality = 30, // 0-63, used only with JPEG format.
        .fb_count = fb_count,       // For ESP32/ESP32-S2, if more than one, i2s runs in continuous mode. 
        .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
        .fb_location = CAMERA_FB_IN_PSRAM
    };

    // initialize the camera sensor
    esp_err_t ret = esp_camera_init(&camera_config);
    if (ret != ESP_OK) {
        return ret;
    }

    // Get the sensor object, and then use some of its functions to adjust the parameters when taking a photo.
    // Note: Do not call functions that set resolution, set picture format and PLL clock,
    // If you need to reset the appeal parameters, please reinitialize the sensor.
    sensor_t *s = esp_camera_sensor_get();
    s->set_vflip(s, 1); // flip it back
    // initial sensors are flipped vertically and colors are a bit saturated
    if (s->id.PID == OV3660_PID) {
        s->set_brightness(s, 1); // up the blightness just a bit
        s->set_saturation(s, -2); // lower the saturation
    }

    if (s->id.PID == OV3660_PID || s->id.PID == OV2640_PID) {
        s->set_vflip(s, 1); // flip it back    
    } else if (s->id.PID == GC0308_PID) {
        s->set_hmirror(s, 0);
    } else if (s->id.PID == GC032A_PID) {
        s->set_vflip(s, 1);
    }

    // Get the basic information of the sensor.
    camera_sensor_info_t *s_info = esp_camera_sensor_get_info(&(s->id));

    if (ESP_OK == ret && PIXFORMAT_JPEG == pixel_format && s_info->support_jpeg == true) {
        auto_jpeg_support = true;
    }

    return ret;
}

void func_take_picture(void)
{
    if (ESP_OK != init_camera(10 * 1000000, PIXFORMAT_RGB565, FRAMESIZE_QVGA, 2)) {
        ESP_LOGE(TAG, "init camrea sensor fail");
        return;
    }

    // take a picture every two seconds and print the size of the picture.
    while (1) {
        ESP_LOGI(TAG, "Taking picture...");
        camera_fb_t *pic = esp_camera_fb_get();

        if(pic){
            // use pic->buf to access the image
            ESP_LOGI(TAG, "Picture taken! Its size was: %zu bytes", pic->len);
            // To enable the frame buffer can be reused again.
            // Note: If you don't call fb_return(), the next time you call fb_get() you may get 
            // an error "Failed to get the frame on time!"because there is no frame buffer space available.
            esp_camera_fb_return(pic);
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void test_take_picture(void)
{
    printf("test success!\n");
}