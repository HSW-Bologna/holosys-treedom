#ifndef CONFIGURATION_H_INCLUDED
#define CONFIGURATION_H_INCLUDED


#include "model/model.h"


void configuration_load(mut_model_t *model);
void configuration_save_programs(mut_model_t *model);
void configuration_save_parameter(void *param_ptr, size_t size, char *key);


extern const char *configuration_auto_mode_key;


#endif
