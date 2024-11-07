#include <stdio.h>
#include "bsp/storage.h"
#include "configuration.h"
#include "model/model.h"


static const char *program_n_name              = "PROG%iNAME";
static const char *program_n_max_speed         = "PROG%iSPEED";
static const char *program_n_acceleration_time = "PROG%iACC";
static const char *program_n_cruise_time       = "PROG%iCRU";
static const char *program_n_deceleration_time = "PROG%iDEC";

const char *configuration_auto_mode_key = "AUTOMODE";


void configuration_load(mut_model_t *model) {
    char string[32] = {0};

    for (uint16_t i = 0; i < PROGRAMS_NUM; i++) {
        snprintf(string, sizeof(string), program_n_name, i);
        storage_load_blob(model->config.programs[i].name, sizeof(model->config.programs[i].name), string);

        snprintf(string, sizeof(string), program_n_max_speed, i);
        storage_load_uint8(&model->config.programs[i].max_speed, string);

        snprintf(string, sizeof(string), program_n_acceleration_time, i);
        storage_load_uint16(&model->config.programs[i].acceleration_time_minutes, string);

        snprintf(string, sizeof(string), program_n_cruise_time, i);
        storage_load_uint16(&model->config.programs[i].max_speed_time_minutes, string);

        snprintf(string, sizeof(string), program_n_deceleration_time, i);
        storage_load_uint16(&model->config.programs[i].deceleration_time_minutes, string);
    }

    storage_load_uint8(&model->config.auto_mode, (char *)configuration_auto_mode_key);
}


void configuration_save_programs(mut_model_t *model) {
    char string[32] = {0};

    for (uint16_t i = 0; i < PROGRAMS_NUM; i++) {
        snprintf(string, sizeof(string), program_n_name, i);
        storage_save_blob(model->config.programs[i].name, sizeof(model->config.programs[i].name), string);

        snprintf(string, sizeof(string), program_n_max_speed, i);
        storage_save_uint8(&model->config.programs[i].max_speed, string);

        snprintf(string, sizeof(string), program_n_acceleration_time, i);
        storage_save_uint16(&model->config.programs[i].acceleration_time_minutes, string);

        snprintf(string, sizeof(string), program_n_cruise_time, i);
        storage_save_uint16(&model->config.programs[i].max_speed_time_minutes, string);

        snprintf(string, sizeof(string), program_n_deceleration_time, i);
        storage_save_uint16(&model->config.programs[i].deceleration_time_minutes, string);
    }
}


void configuration_save_parameter(void *param_ptr, size_t size, char *key) {
    switch (size) {
        case 1:
            storage_save_uint8(param_ptr, key);
            break;

        case 2:
            storage_save_uint16(param_ptr, key);
            break;

        case 4:
            storage_save_uint32(param_ptr, key);
            break;

        default:
            storage_save_blob(param_ptr, size, key);
            break;
    }
}
