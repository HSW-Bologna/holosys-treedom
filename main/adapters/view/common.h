#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED


#include "lvgl.h"


void      view_common_set_hidden(lv_obj_t *obj, uint8_t hidden);
lv_obj_t *view_common_title_create(lv_obj_t *parent, const char *text, uint16_t home_id, uint16_t back_id);
lv_obj_t *view_common_menu_button(lv_obj_t *parent, const char *text, const lv_img_dsc_t *img_dsc, uint16_t id);


#endif
