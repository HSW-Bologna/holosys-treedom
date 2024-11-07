#include "model/model.h"
#include "adapters/view/view.h"
#include "controller.h"
#include "esp_log.h"
#include "services/timestamp.h"
#include "services/system_time.h"
#include "lvgl.h"
#include "configuration.h"
#include "i2c_devices/rtc/RX8010/rx8010.h"
#include "bsp/i2c_devices.h"
#include "bsp/buzzer.h"


static void save_programs(pman_handle_t handle);
static void set_time(pman_handle_t handle, struct tm new_time);
static void beep(pman_handle_t handle);


static const char *TAG = "Gui";


const view_gui_protocol_t controller_gui_protocol = {
    .save_programs = save_programs,
    .set_time      = set_time,
    .beep = beep,
};


void controller_gui_manage(void) {
    (void)TAG;

#ifndef BUILD_CONFIG_SIMULATED_APP
    static timestamp_t last_invoked = 0;

    if (last_invoked != timestamp_get()) {
        if (last_invoked > 0) {
            lv_tick_inc(timestamp_interval(last_invoked, timestamp_get()));
        }
        last_invoked = timestamp_get();
    }
#endif

    lv_timer_handler();
    view_manage();
}


static void save_programs(pman_handle_t handle) {
    mut_model_t *model = pman_get_user_data(handle);
    configuration_save_programs(model);
}


static void set_time(pman_handle_t handle, struct tm new_time) {
    rtc_time_t rtc_time = rx8010_rtc_from_tm(new_time);
    rx8010_set_time(rx8010_driver, rtc_time);
    system_time_set(&new_time);
}


static void beep(pman_handle_t handle) {
    (void)handle;
    bsp_buzzer_beep(1, 50, 50, 3);
}
