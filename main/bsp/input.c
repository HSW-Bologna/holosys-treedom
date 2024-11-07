#include "debounce.h"
#include "keypad.h"
#include <driver/gpio.h>
#include "services/timestamp.h"
#include "hardwareprofile.h"
#include "input.h"
#include <esp_log.h>


static const char *TAG = __FILE_NAME__;


static keypad_key_t keys[] = {
    KEYPAD_KEY(0x1, BSP_INPUT_BUTTON_1),
    KEYPAD_KEY(0x2, BSP_INPUT_BUTTON_2),
    KEYPAD_KEY(0x3, BSP_INPUT_BUTTON_BOTH),
    KEYPAD_NULL_KEY,
};
static debounce_filter_t filter = {0};


void bsp_input_init(void) {
    gpio_config_t config = {
        .intr_type    = GPIO_INTR_DISABLE,
        .mode         = GPIO_MODE_INPUT,
        .pin_bit_mask = BIT64(BSP_HAP_IN1) | BIT64(BSP_HAP_IN2) | BIT64(BSP_HAP_IN3) | BIT64(BSP_HAP_IN4),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&config));

    keypad_reset_keys(keys);
    debounce_filter_init(&filter);

    ESP_LOGI(TAG, "Initialized");
}


uint8_t bsp_input_is_emergency_ok(void) {
    return debounce_read(&filter, 0);
}


void bsp_input_manage(keypad_event_t *event) {
    {
        static timestamp_t ts = 0;
        if (timestamp_is_expired(ts, 10)) {
            uint16_t bitmap = (!gpio_get_level(BSP_HAP_IN1)) | ((!gpio_get_level(BSP_HAP_IN2)) << 1) |
                              (!gpio_get_level(BSP_HAP_IN3) << 2) | ((!gpio_get_level(BSP_HAP_IN4)) << 3);
            debounce_filter(&filter, bitmap, 6);
            ts = timestamp_get();
        }
    }

    {
        uint16_t bitmap = debounce_read(&filter, 0) | (debounce_read(&filter, 1) << 1);
        *event          = keypad_routine(keys, 20, 3000, 100, timestamp_get(), bitmap);
    }
}
