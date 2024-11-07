#include <driver/ledc.h>
#include "hardwareprofile.h"
#include <esp_log.h>
#include "inverter.h"


static const char            *TAG                            = __FILE_NAME__;
static const ledc_mode_t      INVERTER_LEDC_MODE             = LEDC_LOW_SPEED_MODE;
static const ledc_channel_t   INVERTER_LEDC_CHANNEL          = LEDC_CHANNEL_1;
static const ledc_timer_t     INVERTER_LEDC_TIMER            = LEDC_TIMER_2;
static const ledc_timer_bit_t INVERTER_LEDC_TIMER_RESOLUTION = LEDC_TIMER_10_BIT;
static const uint32_t         INVERTER_LEDC_FREQUENCY        = 1000;


void bsp_inverter_init(void) {
    const ledc_channel_config_t channel = {
        .gpio_num   = BSP_HAP_PWM_OI,
        .speed_mode = INVERTER_LEDC_MODE,
        .channel    = INVERTER_LEDC_CHANNEL,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = INVERTER_LEDC_TIMER,
        .duty       = 0,
        .hpoint     = 0,
        .flags      = {.output_invert = 0},
    };
    const ledc_timer_config_t timer = {
        .speed_mode      = INVERTER_LEDC_MODE,
        .duty_resolution = INVERTER_LEDC_TIMER_RESOLUTION,
        .timer_num       = INVERTER_LEDC_TIMER,
        .freq_hz         = INVERTER_LEDC_FREQUENCY,
        .clk_cfg         = LEDC_AUTO_CLK,
    };
    ESP_LOGI(TAG, "Initializing LEDC for inverter PWM");

    ESP_ERROR_CHECK(ledc_timer_config(&timer));
    ESP_ERROR_CHECK(ledc_channel_config(&channel));

    gpio_config_t config = {
        .intr_type    = GPIO_INTR_DISABLE,
        .mode         = GPIO_MODE_OUTPUT,
        .pin_bit_mask = BIT64(BSP_HAP_OUT_OI),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&config));

    bsp_inverter_set_speed(0);
}


void bsp_inverter_set_speed(uint8_t speed_percentage) {
    if (speed_percentage > 100) {
        speed_percentage = 100;
    } else if (speed_percentage < 0) {
        speed_percentage = 0;
    }

    if (speed_percentage == 0) {
        gpio_set_level(BSP_HAP_OUT_OI, 0);
    } else {
        gpio_set_level(BSP_HAP_OUT_OI, 1);
    }

    // LEDC resolution set to 10bits, thus: 100% = 1023
    uint32_t duty_cycle = (1023 * speed_percentage) / 100;
    ESP_ERROR_CHECK(ledc_set_duty(INVERTER_LEDC_MODE, INVERTER_LEDC_CHANNEL, duty_cycle));
    ESP_ERROR_CHECK(ledc_update_duty(INVERTER_LEDC_MODE, INVERTER_LEDC_CHANNEL));
}
