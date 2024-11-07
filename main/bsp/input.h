#ifndef BSP_INPUT_H_INCLUDED
#define BSP_INPUT_H_INCLUDED


#include "keypad.h"


typedef enum {
    BSP_INPUT_BUTTON_1,
    BSP_INPUT_BUTTON_2,
    BSP_INPUT_BUTTON_BOTH,
} bsp_input_button_t;


void    bsp_input_init(void);
void    bsp_input_manage(keypad_event_t *event);
uint8_t bsp_input_is_emergency_ok(void);


#endif
