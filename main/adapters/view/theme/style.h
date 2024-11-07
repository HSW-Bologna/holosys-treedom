#ifndef STYLE_H_INCLUDED
#define STYLE_H_INCLUDED

#include "lvgl.h"

#define STYLE_FONT_SMALL  (&lv_font_montserrat_12)
#define STYLE_FONT_MEDIUM (&lv_font_montserrat_18)
#define STYLE_FONT_BIG    (&lv_font_montserrat_30)
#define STYLE_FONT_HUGE   (&lv_font_montserrat_48)

#define STYLE_COLOR_GRAY   ((lv_color_t)LV_COLOR_MAKE(0xE0, 0xE0, 0xE0))
#define STYLE_COLOR_WHITE  ((lv_color_t)LV_COLOR_MAKE(0xFF, 0xFF, 0xFF))
#define STYLE_COLOR_BLACK  ((lv_color_t)LV_COLOR_MAKE(0x0, 0x0, 0x0))
#define STYLE_COLOR_GREEN  ((lv_color_t)LV_COLOR_MAKE(0x00, 0xBF, 0x63))
#define STYLE_COLOR_RED    ((lv_color_t)LV_COLOR_MAKE(0xE2, 0x20, 0x28))
#define STYLE_COLOR_YELLOW ((lv_color_t)LV_COLOR_MAKE(0xFA, 0xBF, 0x01))


void style_init(void);


extern const lv_style_t style_transparent_cont;
extern const lv_style_t style_background_cont;
extern const lv_style_t style_padless_cont;
extern const lv_style_t style_metallic_cont;
extern const lv_style_t style_icon_button_rel;
extern const lv_style_t style_icon_button_pr;
extern const lv_style_t style_icon;
extern const lv_style_t style_dark_border;


#endif
