#include "i2c_devices/rtc/RX8010/rx8010.h"
#include "controller.h"
#include "model/model.h"
#include "adapters/view/view.h"
#include "gui.h"
#include "keypad.h"
#include "bsp/input.h"
#include "bsp/i2c_devices.h"
#include <esp_log.h>
#include "observer.h"
#include "configuration.h"
#include "services/system_time.h"


static const char *TAG = __FILE_NAME__;


void controller_init(mut_model_t *model) {

    if (rx8010_is_stopped(rx8010_driver)) {
        ESP_LOGI(TAG, "RTC was stopped, initializing...");
        rtc_time_t rtc_time = {.day = 6, .wday = 1, .month = 3, .year = 22};
        rx8010_set_stop(rx8010_driver, 0);
        rx8010_set_time(rx8010_driver, rtc_time);
        ESP_LOGI(TAG, "RTC Clock started");
    } else {
        rtc_time_t rtc_time = {0};
        rx8010_get_time(rx8010_driver, &rtc_time);
        struct tm tm = rx8010_tm_from_rtc(rtc_time);
        tm.tm_isdst  = -1;
        system_time_set(&tm);
    }

    configuration_load(model);
    observer_init(model);
    view_change_page(&page_main);

    ESP_LOGI(TAG, "Initialized");
}


void controller_process_message(pman_handle_t handle, void *msg) {
    (void)handle;
    (void)msg;
}


void controller_manage(mut_model_t *model) {
    {
        keypad_event_t event = {0};
        bsp_input_manage(&event);

        switch (event.tag) {
            case KEYPAD_EVENT_TAG_PRESS:
            case KEYPAD_EVENT_TAG_PRESSING:
            case KEYPAD_EVENT_TAG_LONGPRESSING:
                if (event.code == BSP_INPUT_BUTTON_1) {
                    model->run.left_button_pressed = 1;
                } else if (event.code == BSP_INPUT_BUTTON_2) {
                    model->run.right_button_pressed = 1;
                } else {
                    model->run.left_button_pressed  = 1;
                    model->run.right_button_pressed = 1;
                }
                break;

            case KEYPAD_EVENT_TAG_CLICK:
                if (event.code == BSP_INPUT_BUTTON_1) {
                    view_event((view_event_t){.tag = VIEW_EVENT_TAG_LEFT_HAND_CLICK});
                } else if (event.code == BSP_INPUT_BUTTON_2) {
                    view_event((view_event_t){.tag = VIEW_EVENT_TAG_RIGHT_HAND_CLICK});
                }
                break;

            case KEYPAD_EVENT_TAG_LONGCLICK:
                if (event.code == BSP_INPUT_BUTTON_1 || event.code == BSP_INPUT_BUTTON_2) {
                } else if (event.code == BSP_INPUT_BUTTON_BOTH) {
                    view_event((view_event_t){.tag = VIEW_EVENT_TAG_BOTH_HANDS_LONGCLICK});
                }
                break;

            case KEYPAD_EVENT_TAG_RELEASE:
                if (event.code == BSP_INPUT_BUTTON_1) {
                    model->run.left_button_pressed = 0;
                } else if (event.code == BSP_INPUT_BUTTON_2) {
                    model->run.right_button_pressed = 0;
                } else {
                    model->run.left_button_pressed  = 0;
                    model->run.right_button_pressed = 0;
                }
                break;

            default:
                break;
        }
    }

    {
        static timestamp_t ts = 0;
        if (timestamp_is_expired(ts, 100)) {
            model_manage(model, bsp_input_is_emergency_ok());
            ts = timestamp_get();
        }
    }

    observer_manage(model);
    controller_gui_manage();
}
