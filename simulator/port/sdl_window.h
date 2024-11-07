#ifndef SDL_WINDOW_H_INCLUDED
#define SDL_WINDOW_H_INCLUDED


#include "lvgl.h"


lv_display_t *sdl_window_create(int32_t hor_res, int32_t ver_res);
uint16_t      sdl_window_get_bitmap(void);


#endif
