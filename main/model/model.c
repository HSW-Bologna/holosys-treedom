#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "model.h"
#include "cycle.h"
#include <esp_log.h>


static const char *TAG = __FILE_NAME__;


void model_init(mut_model_t *model) {
    assert(model != NULL);

    memset(model, 0, sizeof(*model));

    for (uint16_t i = 0; i < PROGRAMS_NUM; i++) {
        snprintf(model->config.programs[i].name, sizeof(model->config.programs[i].name), "Program %i", i + 1);
        model->config.programs[i].max_speed                 = 50;
        model->config.programs[i].acceleration_time_minutes = 1;
        model->config.programs[i].max_speed_time_minutes    = 1;
        model->config.programs[i].deceleration_time_minutes = 1;
    }

    cycle_init(model);
}


void model_manage(mut_model_t *model, uint8_t emergency_ok) {
    assert(model != NULL);
    model->run.emergency_ok = emergency_ok;
    cycle_manage(model);
}


void model_stop(mut_model_t *model) {
    assert(model != NULL);
    cycle_send_event(&model->run.cycle.state_machine, model, CYCLE_EVENT_CODE_STOP);
}


uint8_t model_get_speed_percentage(model_t *model) {
    assert(model != NULL);

    const program_t *program = model_get_current_program(model);

    switch (model->run.cycle.state_machine.node_index) {
        case CYCLE_STATE_STOPPED:
        case CYCLE_STATE_INTERRUPTED:
        case CYCLE_STATE_SUCCESSFUL:
        case CYCLE_STATE_STARTING:
            return 0;

        case CYCLE_STATE_CRUISE:
            return program->max_speed;

        case CYCLE_STATE_ACCELERATING: {
            timestamp_t acceleration_elapsed_period =
                stopwatch_get_elapsed(&model->run.cycle.timer_cycle.stopwatch, timestamp_get());
            uint32_t percentage = (acceleration_elapsed_period * 100) / (program->acceleration_time_minutes * 60000UL);
            uint32_t actual_percentage = (program->max_speed * percentage) / 100;
            return (uint8_t)(actual_percentage > 100 ? 100 : actual_percentage);
        }

        case CYCLE_STATE_DECELERATING: {
            timestamp_t deceleration_remaining_period =
                stopwatch_get_elapsed(&model->run.cycle.timer_cycle.stopwatch, timestamp_get());
            uint32_t percentage =
                (deceleration_remaining_period * 100) / (program->acceleration_time_minutes * 60000UL);
            uint32_t actual_percentage = (program->max_speed * percentage) / 100;
            return program->max_speed - (uint8_t)(actual_percentage > 100 ? 100 : actual_percentage);
        }
    }

    return 0;
}


const program_t *model_get_current_program(model_t *model) {
    assert(model != NULL);
    return &model->config.programs[model->run.program_index];
}


uint16_t model_get_program_acceleration_time_permillage(const program_t *program) {
    assert(program != NULL);

    uint32_t total =
        program->acceleration_time_minutes + program->deceleration_time_minutes + program->max_speed_time_minutes;
    return (uint16_t)((program->acceleration_time_minutes * 1000) / total);
}


uint16_t model_get_program_cruise_time_permillage(const program_t *program) {
    assert(program != NULL);

    uint32_t total =
        program->acceleration_time_minutes + program->deceleration_time_minutes + program->max_speed_time_minutes;
    uint16_t permillage = (uint16_t)((program->max_speed_time_minutes * 1000) / total);

    uint16_t total_permillage = model_get_program_acceleration_time_permillage(program) + permillage +
                                model_get_program_deceleration_time_permillage(program);

    // If rounding is needed add it to the cruise section
    if (total_permillage < 1000) {
        permillage += 1000 - total_permillage;
    }
    return permillage;
}


uint16_t model_get_program_deceleration_time_permillage(const program_t *program) {
    assert(program != NULL);

    uint32_t total =
        program->acceleration_time_minutes + program->deceleration_time_minutes + program->max_speed_time_minutes;
    return (uint16_t)((program->deceleration_time_minutes * 1000) / total);
}


uint32_t model_get_program_elapsed_time_seconds(model_t *model) {
    assert(model != NULL);

    switch (model->run.cycle.state_machine.node_index) {
        case CYCLE_STATE_STOPPED:
        case CYCLE_STATE_SUCCESSFUL:
        case CYCLE_STATE_STARTING:
            return 0;

        case CYCLE_STATE_INTERRUPTED:
        case CYCLE_STATE_CRUISE:
        case CYCLE_STATE_ACCELERATING:
        case CYCLE_STATE_DECELERATING: {
            return (model->run.cycle.elapsed_program_time +
                    stopwatch_get_elapsed(&model->run.cycle.timer_cycle.stopwatch, timestamp_get())) /
                   1000UL;
        }
    }

    return 0;
}


uint32_t model_get_program_total_time_seconds(model_t *model) {
    assert(model != NULL);

    const program_t *program = model_get_current_program(model);

    return (program->acceleration_time_minutes + program->deceleration_time_minutes + program->max_speed_time_minutes) *
           60UL;
}


uint32_t model_get_program_remaining_time_seconds(model_t *model) {
    assert(model != NULL);

    uint32_t total_elapsed_time_seconds = model_get_program_elapsed_time_seconds(model);
    uint32_t total_seconds              = model_get_program_total_time_seconds(model);

    if (total_seconds > total_elapsed_time_seconds) {
        return total_seconds - total_elapsed_time_seconds;
    } else {
        return 0;
    }
}


uint16_t model_get_program_time_permillage(model_t *model) {
    assert(model != NULL);

    switch (model->run.cycle.state_machine.node_index) {
        case CYCLE_STATE_STOPPED:
        case CYCLE_STATE_SUCCESSFUL:
        case CYCLE_STATE_STARTING:
            return 1000;

        case CYCLE_STATE_INTERRUPTED:
        case CYCLE_STATE_CRUISE:
        case CYCLE_STATE_ACCELERATING:
        case CYCLE_STATE_DECELERATING: {
            uint32_t total_elapsed_time_seconds = model_get_program_elapsed_time_seconds(model);
            uint32_t total_seconds              = model_get_program_total_time_seconds(model);

            // ESP_LOGI(TAG, "Elapsed time %4lu %6lu %4i", model->run.cycle.elapsed_program_time,
            // stopwatch_get_elapsed(&model->run.cycle.timer_cycle.stopwatch, timestamp_get()), total_seconds);

            return (uint16_t)((total_elapsed_time_seconds * 1000) / total_seconds);
        }
    }

    return 0;
}


uint8_t model_is_not_running(model_t *model) {
    assert(model != NULL);

    switch (model->run.cycle.state_machine.node_index) {
        case CYCLE_STATE_STOPPED:
        case CYCLE_STATE_SUCCESSFUL:
        case CYCLE_STATE_INTERRUPTED:
            return 1;

        case CYCLE_STATE_CRUISE:
        case CYCLE_STATE_ACCELERATING:
        case CYCLE_STATE_DECELERATING:
            return 0;
    }

    return 0;
}
