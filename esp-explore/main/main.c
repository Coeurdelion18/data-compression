#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strips.h"
#include "driver/rmt_tx.h"
#include "esp_log.h"

#define LED_STRIP_RMT_RES_HZ 10000000 // 10MHz
#define LED_PIN 38                    // GPIO38
#define LED_STRIP_LED_NUM 1

static const char *TAG = "RGB_LED";

led_strip_handle_t led_strip;

void rgb_task(void *arg) {
    ESP_LOGI(TAG, "RGB LED task running");
    int count = 0;
    while (1) {
        if(count == 30){
            led_strip_clear(led_strip);
            vTaskDelete(NULL);
        }
        // Red
        led_strip_set_pixel(led_strip, 0, 255, 0, 0);
        led_strip_refresh(led_strip);
        vTaskDelay(pdMS_TO_TICKS(500));
        ++count;
        // Green
        led_strip_set_pixel(led_strip, 0, 0, 255, 0);
        led_strip_refresh(led_strip);
        vTaskDelay(pdMS_TO_TICKS(500));
        ++count;
        // Blue
        led_strip_set_pixel(led_strip, 0, 0, 0, 255);
        led_strip_refresh(led_strip);
        vTaskDelay(pdMS_TO_TICKS(500));
        ++count;
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Initializing RMT for RGB LED");

    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_PIN,
        .max_leds = LED_STRIP_LED_NUM,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,
        .led_model = LED_MODEL_WS2812,
        .flags.invert_out = false,
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = LED_STRIP_RMT_RES_HZ,
        .flags.with_dma = false,
    };

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));

    // Clear LED on init
    led_strip_clear(led_strip);

    // Start RGB blinking task
    xTaskCreate(rgb_task, "rgb_task", 2048, NULL, 2, NULL);
}
