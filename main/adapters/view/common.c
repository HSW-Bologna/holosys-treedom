#include "lvgl.h"
#include "theme/style.h"
#include "view.h"


LV_IMAGE_DECLARE(img_back);
LV_IMAGE_DECLARE(img_home);


void view_common_set_hidden(lv_obj_t *obj, uint8_t hidden) {
    if (hidden && !lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN)) {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else if (!hidden && lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN)) {
        lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}


lv_obj_t *view_common_menu_button(lv_obj_t *parent, const char *text, const lv_img_dsc_t *img_dsc, uint16_t id) {
    lv_obj_t *button = lv_button_create(parent);
    lv_obj_add_style(button, &style_dark_border, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(button, STYLE_COLOR_GRAY, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(button, STYLE_COLOR_BLACK, LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(button, 0, LV_STATE_DEFAULT);
    lv_obj_set_size(button, 310, 44);

    lv_obj_t *label = lv_label_create(button);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
    lv_obj_set_width(label, 280);
    lv_obj_set_style_text_font(label, STYLE_FONT_MEDIUM, LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_STATE_DEFAULT);
    lv_label_set_text(label, text);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t *image = lv_image_create(button);
    lv_obj_add_style(image, &style_icon, LV_STATE_DEFAULT);
    lv_image_set_src(image, img_dsc);
    lv_obj_align(image, LV_ALIGN_RIGHT_MID, 0, 0);

    view_register_object_default_callback(button, id);

    return button;
}


lv_obj_t *view_common_title_create(lv_obj_t *parent, const char *text, uint16_t home_id, uint16_t back_id) {
    lv_obj_t *top_bar = lv_obj_create(parent);
    lv_obj_set_size(top_bar, LV_PCT(100), 40);
    lv_obj_add_style(top_bar, &style_metallic_cont, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_hor(top_bar, 4, LV_STATE_DEFAULT);
    lv_obj_remove_flag(top_bar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(top_bar, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t *label = lv_label_create(top_bar);
    lv_obj_set_style_text_font(label, STYLE_FONT_MEDIUM, LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_STATE_DEFAULT);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
    lv_obj_set_width(label, 240);
    lv_label_set_text(label, text);
    lv_obj_center(label);

    lv_obj_t *button_back = lv_button_create(top_bar);
    lv_obj_add_style(button_back, &style_icon_button_rel, LV_STATE_DEFAULT);
    lv_obj_add_style(button_back, &style_icon_button_pr, LV_STATE_PRESSED);
    lv_obj_set_size(button_back, 40, 40);
    lv_obj_t *image_back = lv_image_create(button_back);
    lv_image_set_src(image_back, &img_back);
    lv_obj_add_style(image_back, &style_icon, LV_STATE_DEFAULT);
    lv_obj_center(image_back);
    lv_obj_align(button_back, LV_ALIGN_RIGHT_MID, 0, 0);
    view_register_object_default_callback(button_back, back_id);

    lv_obj_t *button_home = lv_button_create(top_bar);
    lv_obj_add_style(button_home, &style_icon_button_rel, LV_STATE_DEFAULT);
    lv_obj_add_style(button_home, &style_icon_button_pr, LV_STATE_PRESSED);
    lv_obj_set_size(button_home, 40, 40);
    lv_obj_t *image_home = lv_image_create(button_home);
    lv_image_set_src(image_home, &img_home);
    lv_obj_add_style(image_home, &style_icon, LV_STATE_DEFAULT);
    lv_obj_center(image_home);
    lv_obj_align(button_home, LV_ALIGN_LEFT_MID, 0, 0);
    view_register_object_default_callback(button_home, home_id);

    return top_bar;
}
