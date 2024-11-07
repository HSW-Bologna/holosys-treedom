#include "bsp/input.h"
#include "debounce.h"
#include "keypad.h"
#include "sdl_window.h"
#include "services/timestamp.h"


static keypad_key_t keys[] = {
    KEYPAD_KEY(0x1, BSP_INPUT_BUTTON_1),
    KEYPAD_KEY(0x2, BSP_INPUT_BUTTON_2),
    KEYPAD_KEY(0x3, BSP_INPUT_BUTTON_BOTH),
    KEYPAD_NULL_KEY,
};


void bsp_input_init(void) {
    keypad_reset_keys(keys);
}


uint8_t bsp_input_is_emergency_ok(void) {
    return (sdl_window_get_bitmap() & 0x4) == 0;
}


void bsp_input_manage(keypad_event_t *event) {
    uint16_t bitmap = sdl_window_get_bitmap();
    *event          = keypad_routine(keys, 50, 10000, 100, timestamp_get(), bitmap & 0x3);
}
