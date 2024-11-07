#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include "state_machine.h"
#include "stopwatch.h"
#include "stopwatch_timer.h"
#include "services/timestamp.h"


#define PROGRAM_NAME_SIZE 32
#define PROGRAMS_NUM      10


#define GETTER(name, field)                                                                                            \
    static inline                                                                                                      \
        __attribute__((always_inline, const)) typeof(((mut_model_t *)0)->field) model_get_##name(model_t *pmodel) {    \
        assert(pmodel != NULL);                                                                                        \
        return pmodel->field;                                                                                          \
    }


typedef enum {
    CYCLE_EVENT_CODE_CHECK,
    CYCLE_EVENT_CODE_STOP,
    CYCLE_EVENT_CODE_TIMER_DONE,
} cycle_event_code_t;


typedef enum {
    CYCLE_STATE_STOPPED = 0,
    CYCLE_STATE_STARTING,
    CYCLE_STATE_ACCELERATING,
    CYCLE_STATE_CRUISE,
    CYCLE_STATE_DECELERATING,
    CYCLE_STATE_INTERRUPTED,
    CYCLE_STATE_SUCCESSFUL,
} cycle_state_t;


STATE_MACHINE_DECLARE(cycle, cycle_event_code_t);



typedef struct {
    char     name[PROGRAM_NAME_SIZE + 1];
    uint8_t  max_speed;
    uint16_t acceleration_time_minutes;
    uint16_t max_speed_time_minutes;
    uint16_t deceleration_time_minutes;
} program_t;


struct model {
    struct {
        uint16_t language;

        program_t programs[PROGRAMS_NUM];
        uint8_t   auto_mode;
    } config;

    struct {
        uint16_t program_index;
        uint8_t  left_button_pressed;
        uint8_t  right_button_pressed;
        uint8_t  emergency_ok;

        struct {
            stopwatch_timer_t     timer_cycle;
            timestamp_t           timestamp;
            time_t                done_time;
            timestamp_t           elapsed_program_time;
            cycle_state_machine_t state_machine;
        } cycle;
    } run;
};


typedef const struct model model_t;
typedef struct model       mut_model_t;


void             model_init(mut_model_t *pmodel);
const program_t *model_get_current_program(model_t *model);
void             model_manage(mut_model_t *model, uint8_t emergency_ok);
uint8_t          model_get_speed_percentage(model_t *model);
uint16_t         model_get_program_acceleration_time_permillage(const program_t *program);
uint16_t         model_get_program_cruise_time_permillage(const program_t *program);
uint16_t         model_get_program_deceleration_time_permillage(const program_t *program);
uint16_t         model_get_program_time_permillage(model_t *model);
uint32_t         model_get_program_elapsed_time_seconds(model_t *model);
uint32_t         model_get_program_remaining_time_seconds(model_t *model);
uint32_t         model_get_program_total_time_seconds(model_t *model);
uint8_t          model_is_not_running(model_t *model);
void             model_stop(mut_model_t *model);


GETTER(language, config.language);


#endif
