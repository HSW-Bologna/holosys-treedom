#include "observer.h"
#include "watcher.h"
#include "model/model.h"
#include "bsp/inverter.h"
#include "services/timestamp.h"
#include "configuration.h"


static void update_output_percentage(void *old_value, const void *new_value, watcher_size_t size, void *user_ptr,
                                     void *arg);
static void update_param(void *old_value, const void *new_value, watcher_size_t size, void *user_ptr, void *arg);


static struct {
    watcher_t watcher;
    uint8_t   output_percentage;
} state = {
    .watcher           = {},
    .output_percentage = 0,
};


void observer_init(mut_model_t *model) {
    WATCHER_INIT_STD(&state.watcher, (void *)model);

    WATCHER_ADD_ENTRY(&state.watcher, &state.output_percentage, update_output_percentage, NULL);
    WATCHER_ADD_ENTRY_DELAYED(&state.watcher, &model->config.auto_mode, update_param, (void*)configuration_auto_mode_key, 4000);
}


void observer_manage(model_t *model) {
    state.output_percentage = model_get_speed_percentage(model);

    watcher_watch(&state.watcher, timestamp_get());
}


static void update_output_percentage(void *old_value, const void *new_value, watcher_size_t size, void *user_ptr,
                                     void *arg) {
    (void)old_value;
    (void)new_value;
    (void)size;
    (void)arg;
    (void)user_ptr;

    bsp_inverter_set_speed(state.output_percentage);
}


static void update_param(void *old_value, const void *new_value, watcher_size_t size, void *user_ptr, void *arg) {
    (void)old_value;
    (void)user_ptr;

    configuration_save_parameter((void*)new_value, size, arg);
}
