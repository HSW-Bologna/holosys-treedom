#include "lvgl.h"
#include "style.h"


static const lv_style_const_prop_t style_transparent_cont_props[] = {
    LV_STYLE_CONST_PAD_BOTTOM(0), LV_STYLE_CONST_PAD_TOP(0),      LV_STYLE_CONST_PAD_LEFT(0),
    LV_STYLE_CONST_PAD_RIGHT(0),  LV_STYLE_CONST_BORDER_WIDTH(0), LV_STYLE_CONST_BG_OPA(LV_OPA_TRANSP),
    LV_STYLE_CONST_PROPS_END,
};
LV_STYLE_CONST_INIT(style_transparent_cont, (void *)style_transparent_cont_props);


static const lv_style_const_prop_t style_metallic_cont_props[] = {
    LV_STYLE_CONST_PAD_BOTTOM(0),
    LV_STYLE_CONST_PAD_TOP(0),
    LV_STYLE_CONST_PAD_LEFT(0),
    LV_STYLE_CONST_PAD_RIGHT(0),
    LV_STYLE_CONST_BORDER_WIDTH(0),
    LV_STYLE_CONST_RADIUS(0),
    LV_STYLE_CONST_BG_COLOR(STYLE_COLOR_GRAY),
    LV_STYLE_CONST_PROPS_END,
};
LV_STYLE_CONST_INIT(style_metallic_cont, (void *)style_metallic_cont_props);


static const lv_style_const_prop_t style_background_cont_props[] = {
    LV_STYLE_CONST_PAD_BOTTOM(0),
    LV_STYLE_CONST_PAD_TOP(0),
    LV_STYLE_CONST_PAD_LEFT(0),
    LV_STYLE_CONST_PAD_RIGHT(0),
    LV_STYLE_CONST_BORDER_WIDTH(0),
    LV_STYLE_CONST_BG_OPA(LV_OPA_COVER),
    LV_STYLE_CONST_BG_COLOR(STYLE_COLOR_WHITE),
    LV_STYLE_CONST_PROPS_END,
};
LV_STYLE_CONST_INIT(style_background_cont, (void *)style_background_cont_props);


static const lv_style_const_prop_t style_padless_cont_props[] = {
    LV_STYLE_CONST_PAD_BOTTOM(0), LV_STYLE_CONST_PAD_TOP(0),    LV_STYLE_CONST_PAD_LEFT(0), LV_STYLE_CONST_PAD_RIGHT(0),
    LV_STYLE_CONST_PAD_ROW(0),    LV_STYLE_CONST_PAD_COLUMN(0), LV_STYLE_CONST_PROPS_END,
};
LV_STYLE_CONST_INIT(style_padless_cont, (void *)style_padless_cont_props);

static const lv_style_const_prop_t style_icon_props[] = {
    LV_STYLE_CONST_IMAGE_RECOLOR_OPA(LV_OPA_COVER),
    LV_STYLE_CONST_IMAGE_RECOLOR(STYLE_COLOR_BLACK),
    LV_STYLE_CONST_PROPS_END,
};
LV_STYLE_CONST_INIT(style_icon, (void *)style_icon_props);


static const lv_style_const_prop_t style_icon_button_rel_props[] = {
    LV_STYLE_CONST_BG_OPA(LV_OPA_TRANSP),
    LV_STYLE_CONST_RADIUS(LV_RADIUS_CIRCLE),
    LV_STYLE_CONST_SHADOW_WIDTH(0),
    LV_STYLE_CONST_TEXT_COLOR(STYLE_COLOR_BLACK),
    LV_STYLE_CONST_PROPS_END,
};
LV_STYLE_CONST_INIT(style_icon_button_rel, (void *)style_icon_button_rel_props);


static const lv_style_const_prop_t style_icon_button_pr_props[] = {
    LV_STYLE_CONST_BG_OPA(LV_OPA_20),
    LV_STYLE_CONST_BG_COLOR(STYLE_COLOR_BLACK),
    LV_STYLE_CONST_PROPS_END,
};
LV_STYLE_CONST_INIT(style_icon_button_pr, (void *)style_icon_button_pr_props);


static const lv_style_const_prop_t style_dark_border_props[] = {
    LV_STYLE_CONST_RADIUS(20),
    LV_STYLE_CONST_BORDER_WIDTH(3),
    LV_STYLE_CONST_BORDER_COLOR(STYLE_COLOR_BLACK),
    LV_STYLE_CONST_PROPS_END,
};
LV_STYLE_CONST_INIT(style_dark_border, (void *)style_dark_border_props);


void style_init(void) {}
