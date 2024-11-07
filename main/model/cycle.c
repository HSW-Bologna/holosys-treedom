#include <time.h>
#include "cycle.h"
#include "services/timestamp.h"
#include "model.h"
#include "state_machine.h"
#include "stopwatch.h"
#include "stopwatch_timer.h"
#include <esp_log.h>


static void timer_event_callback(stopwatch_timer_t *timer, void *user_ptr);
static void fix_timer(stopwatch_timer_t *timer, unsigned long period, int arg);
static int  stopped_event_manager(cycle_event_code_t event, void *arg);
static void starting_entry(void *arg);
static int  starting_event_manager(cycle_event_code_t event, void *arg);
static void accelerating_entry(void *arg);
static int  accelerating_event_manager(cycle_event_code_t event, void *arg);
static void cruise_entry(void *arg);
static int  cruise_event_manager(cycle_event_code_t event, void *arg);
static void decelerating_entry(void *arg);
static int  decelerating_event_manager(cycle_event_code_t event, void *arg);
static void interrupted_entry(void *arg);
static int  interrupted_event_manager(cycle_event_code_t event, void *arg);
static void success_entry(void *arg);


STATE_MACHINE_DEFINE(cycle, cycle_event_code_t);


static const char        *TAG        = __FILE_NAME__;
static const cycle_node_t managers[] = {
    // Everything off
    [CYCLE_STATE_STOPPED]      = STATE_MACHINE_EVENT_MANAGER(stopped_event_manager),
    [CYCLE_STATE_STARTING]     = STATE_MACHINE_MANAGER(starting_event_manager, starting_entry, NULL),
    [CYCLE_STATE_ACCELERATING] = STATE_MACHINE_MANAGER(accelerating_event_manager, accelerating_entry, NULL),
    [CYCLE_STATE_CRUISE]       = STATE_MACHINE_MANAGER(cruise_event_manager, cruise_entry, NULL),
    [CYCLE_STATE_DECELERATING] = STATE_MACHINE_MANAGER(decelerating_event_manager, decelerating_entry, NULL),
    [CYCLE_STATE_INTERRUPTED]  = STATE_MACHINE_MANAGER(interrupted_event_manager, interrupted_entry, NULL),
    [CYCLE_STATE_SUCCESSFUL]   = STATE_MACHINE_MANAGER(stopped_event_manager, success_entry, NULL),
};


void cycle_init(mut_model_t *model) {
    model->run.cycle.state_machine.nodes      = (cycle_node_t *)managers;
    model->run.cycle.state_machine.node_index = CYCLE_STATE_STOPPED;

    stopwatch_timer_init(&model->run.cycle.timer_cycle, 0, timer_event_callback, NULL);
}


uint8_t cycle_manage(mut_model_t *model) {
    cycle_state_t old_cycle_state = model->run.cycle.state_machine.node_index;

    stopwatch_timer_manage(&model->run.cycle.timer_cycle, timestamp_get(), model);
    cycle_send_event(&model->run.cycle.state_machine, model, CYCLE_EVENT_CODE_CHECK);

    cycle_state_t new_cycle_state = model->run.cycle.state_machine.node_index;

    if (old_cycle_state != new_cycle_state) {
        ESP_LOGI(TAG, "Cycle state: %i -> %i", old_cycle_state, new_cycle_state);
        return 1;
    } else {
        return 0;
    }
}


static int stopped_event_manager(cycle_event_code_t event, void *arg) {
    mut_model_t *model = arg;

    switch (event) {
        case CYCLE_EVENT_CODE_CHECK:
            if (model->run.left_button_pressed && model->run.right_button_pressed) {
                return CYCLE_STATE_STARTING;
            }
            break;

        case CYCLE_EVENT_CODE_STOP:
            return CYCLE_STATE_STOPPED;

        default:
            break;
    }

    return -1;
}


static void starting_entry(void *arg) {
    mut_model_t *model         = arg;
    model->run.cycle.timestamp = timestamp_get();
}


static int starting_event_manager(cycle_event_code_t event, void *arg) {
    mut_model_t *model = arg;

    switch (event) {
        case CYCLE_EVENT_CODE_CHECK:
            if (!model->run.left_button_pressed || !model->run.right_button_pressed) {
                return CYCLE_STATE_STOPPED;
            } else if (timestamp_is_expired(model->run.cycle.timestamp, 3000UL) && model->run.emergency_ok) {
                return CYCLE_STATE_ACCELERATING;
            }
            break;

        default:
            break;
    }

    return -1;
}


static void accelerating_entry(void *arg) {
    mut_model_t       *model              = arg;
    stopwatch_timer_t *timer_cycle        = &model->run.cycle.timer_cycle;
    const program_t   *program            = model_get_current_program(model);
    model->run.cycle.elapsed_program_time = 0;
    model->run.cycle.timestamp            = timestamp_get();

    fix_timer(timer_cycle, program->acceleration_time_minutes * 60000UL, CYCLE_EVENT_CODE_TIMER_DONE);
}


static int accelerating_event_manager(cycle_event_code_t event, void *arg) {
    mut_model_t *model = arg;

    switch (event) {
        case CYCLE_EVENT_CODE_CHECK:
            if (!model->run.emergency_ok) {
                return CYCLE_STATE_INTERRUPTED;
            } else if (!model->run.left_button_pressed || !model->run.right_button_pressed) {
                if (timestamp_is_expired(model->run.cycle.timestamp, 3000)) {
                    return CYCLE_STATE_INTERRUPTED;
                }
            } else {
                model->run.cycle.timestamp = timestamp_get();
            }
            break;

        case CYCLE_EVENT_CODE_STOP:
            return CYCLE_STATE_INTERRUPTED;
            break;

        case CYCLE_EVENT_CODE_TIMER_DONE:
            return CYCLE_STATE_CRUISE;
            break;

        default:
            break;
    }

    return -1;
}


static void cruise_entry(void *arg) {
    mut_model_t       *model       = arg;
    stopwatch_timer_t *timer_cycle = &model->run.cycle.timer_cycle;
    const program_t   *program     = model_get_current_program(model);

    model->run.cycle.elapsed_program_time += stopwatch_get_elapsed(&timer_cycle->stopwatch, timestamp_get());
    fix_timer(timer_cycle, program->max_speed_time_minutes * 60000UL, CYCLE_EVENT_CODE_TIMER_DONE);
}


static int cruise_event_manager(cycle_event_code_t event, void *arg) {
    mut_model_t *model = arg;

    switch (event) {
        case CYCLE_EVENT_CODE_CHECK:
            if (!model->run.emergency_ok) {
                return CYCLE_STATE_INTERRUPTED;
            } else if (!model->config.auto_mode &&
                       (!model->run.left_button_pressed || !model->run.right_button_pressed)) {
                if (timestamp_is_expired(model->run.cycle.timestamp, 3000)) {
                    return CYCLE_STATE_INTERRUPTED;
                }
            } else {
                model->run.cycle.timestamp = timestamp_get();
            }
            break;

        case CYCLE_EVENT_CODE_STOP:
            return CYCLE_STATE_INTERRUPTED;
            break;

        case CYCLE_EVENT_CODE_TIMER_DONE:
            return CYCLE_STATE_DECELERATING;
            break;

        default:
            break;
    }

    return -1;
}


static void decelerating_entry(void *arg) {
    mut_model_t       *model       = arg;
    stopwatch_timer_t *timer_cycle = &model->run.cycle.timer_cycle;
    const program_t   *program     = model_get_current_program(model);

    model->run.cycle.elapsed_program_time += stopwatch_get_elapsed(&timer_cycle->stopwatch, timestamp_get());
    fix_timer(timer_cycle, program->deceleration_time_minutes * 60000UL, CYCLE_EVENT_CODE_TIMER_DONE);
}


static int decelerating_event_manager(cycle_event_code_t event, void *arg) {
    mut_model_t *model = arg;

    switch (event) {
        case CYCLE_EVENT_CODE_CHECK:
            if (!model->run.emergency_ok) {
                return CYCLE_STATE_INTERRUPTED;
            } else if (!model->config.auto_mode &&
                       (!model->run.left_button_pressed || !model->run.right_button_pressed)) {
                if (timestamp_is_expired(model->run.cycle.timestamp, 3000)) {
                    return CYCLE_STATE_INTERRUPTED;
                }
            } else {
                model->run.cycle.timestamp = timestamp_get();
            }
            break;

        case CYCLE_EVENT_CODE_STOP:
            return CYCLE_STATE_INTERRUPTED;
            break;

        case CYCLE_EVENT_CODE_TIMER_DONE:
            return CYCLE_STATE_SUCCESSFUL;
            break;

        default:
            break;
    }

    return -1;
}


static void interrupted_entry(void *arg) {
    mut_model_t       *model       = arg;
    stopwatch_timer_t *timer_cycle = &model->run.cycle.timer_cycle;
    stopwatch_timer_pause(timer_cycle, timestamp_get());
    model->run.cycle.done_time = time(NULL);
}


static int interrupted_event_manager(cycle_event_code_t event, void *arg) {
    mut_model_t *model = arg;

    switch (event) {
        case CYCLE_EVENT_CODE_CHECK:
            if (model->run.left_button_pressed && model->run.right_button_pressed && model->run.emergency_ok) {
                return CYCLE_STATE_STARTING;
            }
            break;

        case CYCLE_EVENT_CODE_STOP:
            return CYCLE_STATE_STOPPED;

        default:
            break;
    }

    return -1;
}


static void success_entry(void *arg) {
    mut_model_t *model         = arg;
    model->run.cycle.done_time = time(NULL);
}


static void timer_event_callback(stopwatch_timer_t *timer, void *user_ptr) {
    mut_model_t *model = user_ptr;

    cycle_send_event(&model->run.cycle.state_machine, model, (cycle_event_code_t)(uintptr_t)timer->arg);
}


static void fix_timer(stopwatch_timer_t *timer, unsigned long period, int arg) {
    stopwatch_timer_set_period(timer, period);
    stopwatch_timer_set_arg(timer, (void *)(uintptr_t)arg);
    stopwatch_timer_reset(timer, timestamp_get());
    stopwatch_timer_resume(timer, timestamp_get());
}
