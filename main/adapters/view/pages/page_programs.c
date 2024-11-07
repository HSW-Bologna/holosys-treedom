#include <assert.h>
#include <stdlib.h>
#include "lvgl.h"
#include "model/model.h"
#include "../view.h"
#include "../theme/style.h"
#include "../intl/intl.h"
#include "../common.h"
#include <esp_log.h>


LV_IMAGE_DECLARE(img_left);
LV_IMAGE_DECLARE(img_right);
LV_IMAGE_DECLARE(img_edit);
LV_IMAGE_DECLARE(img_minus);
LV_IMAGE_DECLARE(img_plus);


#define PARAM_WINDOW_WIDTH 230


enum {
    BTN_BACK_ID,
    BTN_HOME_ID,
    BTN_PROGRAM_ID,
    BTN_EDIT_NAME_ID,
    BTN_MAX_SPEED_ID,
    BTN_ACCELERATION_TIME_ID,
    BTN_CRUISE_TIME_ID,
    BTN_DECELERATION_TIME_ID,
    KEYBOARD_ID,
};


struct page_data {
    lv_obj_t *label_program_num;
    lv_obj_t *label_program_name;
    lv_obj_t *label_max_speed;
    lv_obj_t *label_acceleration_time;
    lv_obj_t *label_cruise_time;
    lv_obj_t *label_deceleration_time;

    lv_obj_t *ta_program_name;

    lv_obj_t *keyboard;

    uint16_t program_index;
    uint8_t  modified;
    uint8_t  name_editing;
};


static void      update_page(model_t *model, struct page_data *pdata);
static lv_obj_t *param_window_create(lv_obj_t *parent, lv_obj_t **label);
static lv_obj_t *icon_button_create(lv_obj_t *parent, const lv_img_dsc_t *img_dsc, uint16_t id, uint16_t number);


static const char *TAG = __FILE_NAME__;


static void *create_page(pman_handle_t handle, void *extra) {
    (void)handle;
    (void)extra;

    struct page_data *pdata = lv_malloc(sizeof(struct page_data));
    assert(pdata != NULL);

    pdata->program_index = 0;
    pdata->modified      = 0;
    pdata->name_editing  = 0;

    return pdata;
}


static void open_page(pman_handle_t handle, void *state) {
    struct page_data *pdata = state;

    model_t *model = pman_get_user_data(handle);

    lv_obj_t *cont = lv_obj_create(lv_screen_active());
    lv_obj_add_style(cont, &style_background_cont, LV_STATE_DEFAULT);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_center(cont);

    view_common_title_create(cont, "PROGRAMS MENU", BTN_HOME_ID, BTN_BACK_ID);


    lv_obj_t *param        = param_window_create(cont, &pdata->label_program_num);
    lv_obj_t *button_left  = icon_button_create(cont, &img_left, BTN_PROGRAM_ID, -1);
    lv_obj_t *button_right = icon_button_create(cont, &img_right, BTN_PROGRAM_ID, +1);

    lv_obj_align(param, LV_ALIGN_TOP_MID, 0, 44);
    lv_obj_align_to(button_left, param, LV_ALIGN_OUT_LEFT_MID, -2, 0);
    lv_obj_align_to(button_right, param, LV_ALIGN_OUT_RIGHT_MID, +2, 0);


    param        = param_window_create(cont, &pdata->label_program_name);
    lv_obj_t *ta = lv_textarea_create(param);
    lv_obj_set_style_text_font(ta, STYLE_FONT_MEDIUM, LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ta, LV_TEXT_ALIGN_CENTER, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ta, 0, LV_STATE_DEFAULT);
    lv_obj_set_size(ta, LV_PCT(100), 38);
    lv_obj_center(ta);
    pdata->ta_program_name = ta;
    button_left            = icon_button_create(cont, &img_edit, BTN_EDIT_NAME_ID, 0);

    lv_obj_align(param, LV_ALIGN_TOP_MID, 0, 94);
    lv_obj_align_to(button_left, param, LV_ALIGN_OUT_LEFT_MID, -2, 0);
    lv_obj_t *param_prev = param;


    param        = param_window_create(cont, &pdata->label_max_speed);
    button_left  = icon_button_create(cont, &img_minus, BTN_MAX_SPEED_ID, -1);
    button_right = icon_button_create(cont, &img_plus, BTN_MAX_SPEED_ID, +1);

    lv_obj_align_to(param, param_prev, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    lv_obj_align_to(button_left, param, LV_ALIGN_OUT_LEFT_MID, -2, 0);
    lv_obj_align_to(button_right, param, LV_ALIGN_OUT_RIGHT_MID, +2, 0);
    param_prev = param;


    param        = param_window_create(cont, &pdata->label_acceleration_time);
    button_left  = icon_button_create(cont, &img_minus, BTN_ACCELERATION_TIME_ID, -1);
    button_right = icon_button_create(cont, &img_plus, BTN_ACCELERATION_TIME_ID, +1);

    lv_obj_align_to(param, param_prev, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    lv_obj_align_to(button_left, param, LV_ALIGN_OUT_LEFT_MID, -2, 0);
    lv_obj_align_to(button_right, param, LV_ALIGN_OUT_RIGHT_MID, +2, 0);
    param_prev = param;


    param        = param_window_create(cont, &pdata->label_cruise_time);
    button_left  = icon_button_create(cont, &img_minus, BTN_CRUISE_TIME_ID, -1);
    button_right = icon_button_create(cont, &img_plus, BTN_CRUISE_TIME_ID, +1);

    lv_obj_align_to(param, param_prev, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    lv_obj_align_to(button_left, param, LV_ALIGN_OUT_LEFT_MID, -2, 0);
    lv_obj_align_to(button_right, param, LV_ALIGN_OUT_RIGHT_MID, +2, 0);
    param_prev = param;


    param        = param_window_create(cont, &pdata->label_deceleration_time);
    button_left  = icon_button_create(cont, &img_minus, BTN_DECELERATION_TIME_ID, -1);
    button_right = icon_button_create(cont, &img_plus, BTN_DECELERATION_TIME_ID, +1);

    lv_obj_align_to(param, param_prev, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    lv_obj_align_to(button_left, param, LV_ALIGN_OUT_LEFT_MID, -2, 0);
    lv_obj_align_to(button_right, param, LV_ALIGN_OUT_RIGHT_MID, +2, 0);
    param_prev = param;


    lv_obj_t *keyboard = lv_keyboard_create(cont);
    lv_keyboard_set_textarea(keyboard, ta);
    lv_obj_set_style_text_font(keyboard, STYLE_FONT_MEDIUM, LV_STATE_DEFAULT);
    lv_obj_set_size(keyboard, LV_PCT(100), 182);
    lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
    view_register_object_default_callback(keyboard, KEYBOARD_ID);
    pdata->keyboard = keyboard;


    update_page(model, pdata);

    ESP_LOGI(TAG, "Open");
}


static pman_msg_t page_event(pman_handle_t handle, void *state, pman_event_t event) {
    (void)handle;

    pman_msg_t msg = PMAN_MSG_NULL;

    struct page_data *pdata = state;

    mut_model_t *model = pman_get_user_data(handle);

    switch (event.tag) {
        case PMAN_EVENT_TAG_TIMER: {
            update_page(model, pdata);
            break;
        }

        case PMAN_EVENT_TAG_USER: {
            view_event_t *view_event = event.as.user;
            switch (view_event->tag) {
                case VIEW_EVENT_TAG_PAGE_WATCHER: {
                    break;
                }

                case VIEW_EVENT_TAG_LEFT_HAND_CLICK: {
                    if (pdata->program_index > 0) {
                        pdata->program_index--;
                    } else {
                        pdata->program_index = PROGRAMS_NUM - 1;
                    }
                    update_page(model, pdata);
                    break;
                }

                case VIEW_EVENT_TAG_RIGHT_HAND_CLICK: {
                    pdata->program_index = (pdata->program_index + 1) % PROGRAMS_NUM;
                    update_page(model, pdata);
                    break;
                }

                case VIEW_EVENT_TAG_BOTH_HANDS_LONGCLICK: {
                    break;
                }
            }
            break;
        }

        case PMAN_EVENT_TAG_LVGL: {
            lv_event_t *lv_event = event.as.lvgl;
            lv_obj_t   *target   = lv_event_get_current_target_obj(lv_event);
            uint16_t    id       = view_get_obj_id(target);
            int16_t     number   = (int16_t)view_get_obj_number(target);
            program_t  *program  = &model->config.programs[pdata->program_index];

            switch (lv_event_get_code(lv_event)) {
                case LV_EVENT_CANCEL: {
                    switch (id) {
                        case KEYBOARD_ID: {
                            pdata->name_editing = 0;
                            update_page(model, pdata);
                            break;
                        }

                        default:
                            break;
                    }
                    break;
                }

                case LV_EVENT_READY: {
                    switch (id) {
                        case KEYBOARD_ID: {
                            pdata->name_editing = 0;
                            strcpy(program->name, lv_textarea_get_text(pdata->ta_program_name));
                            update_page(model, pdata);
                            break;
                        }

                        default:
                            break;
                    }
                    break;
                }

                case LV_EVENT_CLICKED: {
                    switch (id) {
                        case BTN_BACK_ID: {
                            if (pdata->modified) {
                                view_get_protocol(handle)->save_programs(handle);
                            }
                            msg.stack_msg = PMAN_STACK_MSG_BACK();
                            break;
                        }

                        case BTN_HOME_ID: {
                            if (pdata->modified) {
                                view_get_protocol(handle)->save_programs(handle);
                            }
                            msg.stack_msg = PMAN_STACK_MSG_REBASE(&page_main);
                            break;
                        }

                        case BTN_PROGRAM_ID: {
                            if (number > 0) {
                                pdata->program_index = (pdata->program_index + 1) % PROGRAMS_NUM;
                            } else if (number < 0) {
                                if (pdata->program_index > 0) {
                                    pdata->program_index--;
                                } else {
                                    pdata->program_index = PROGRAMS_NUM - 1;
                                }
                            }
                            update_page(model, pdata);
                            break;
                        }

                        case BTN_EDIT_NAME_ID:
                            pdata->modified     = 1;
                            pdata->name_editing = 1;
                            lv_textarea_set_text(pdata->ta_program_name, program->name);
                            update_page(model, pdata);
                            break;

                        case BTN_MAX_SPEED_ID: {
                            pdata->modified = 1;
                            if (number > 0) {
                                program->max_speed = (program->max_speed + 1) % 101;
                            } else if (number < 0) {
                                if (program->max_speed > 0) {
                                    program->max_speed--;
                                } else {
                                    program->max_speed = 100;
                                }
                            }
                            update_page(model, pdata);
                            break;
                        }

                        case BTN_ACCELERATION_TIME_ID: {
                            pdata->modified = 1;
                            if (number > 0) {
                                program->acceleration_time_minutes =
                                    ((program->acceleration_time_minutes + 1) % 30) + 1;
                            } else if (number < 0) {
                                if (program->acceleration_time_minutes > 1) {
                                    program->acceleration_time_minutes--;
                                } else {
                                    program->acceleration_time_minutes = 30;
                                }
                            }
                            update_page(model, pdata);
                            break;
                        }

                        case BTN_CRUISE_TIME_ID: {
                            pdata->modified = 1;
                            if (number > 0) {
                                program->max_speed_time_minutes = ((program->max_speed_time_minutes + 1) % 30) + 1;
                            } else if (number < 0) {
                                if (program->max_speed_time_minutes > 1) {
                                    program->max_speed_time_minutes--;
                                } else {
                                    program->max_speed_time_minutes = 30;
                                }
                            }
                            update_page(model, pdata);
                            break;
                        }

                        case BTN_DECELERATION_TIME_ID: {
                            pdata->modified = 1;
                            if (number > 0) {
                                program->deceleration_time_minutes =
                                    ((program->deceleration_time_minutes + 1) % 30) + 1;
                            } else if (number < 0) {
                                if (program->deceleration_time_minutes > 1) {
                                    program->deceleration_time_minutes--;
                                } else {
                                    program->deceleration_time_minutes = 30;
                                }
                            }
                            update_page(model, pdata);
                            break;
                        }

                        default:
                            break;
                    }

                    break;
                }

                default:
                    break;
            }

            break;
        }

        default:
            break;
    }

    return msg;
}


static void update_page(model_t *model, struct page_data *pdata) {
    const program_t *program = &model->config.programs[pdata->program_index];

    lv_label_set_text_fmt(pdata->label_program_num, "PROGRAM: %03i", pdata->program_index + 1);
    lv_label_set_text(pdata->label_program_name, program->name);
    lv_label_set_text_fmt(pdata->label_max_speed, "MAX SPEED: %03i%%", program->max_speed);
    lv_label_set_text_fmt(pdata->label_acceleration_time, "Ramp UP: %03i Min", program->acceleration_time_minutes);
    lv_label_set_text_fmt(pdata->label_cruise_time, "MAX Time: %03i Min", program->max_speed_time_minutes);
    lv_label_set_text_fmt(pdata->label_deceleration_time, "Ramp DOWN: %03i Min", program->deceleration_time_minutes);

    view_common_set_hidden(pdata->label_program_name, pdata->name_editing);
    view_common_set_hidden(pdata->ta_program_name, !pdata->name_editing);
    view_common_set_hidden(pdata->keyboard, !pdata->name_editing);
}


static lv_obj_t *param_window_create(lv_obj_t *parent, lv_obj_t **label) {
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_set_size(cont, PARAM_WINDOW_WIDTH, 40);
    lv_obj_add_style(cont, &style_dark_border, LV_STATE_DEFAULT);
    lv_obj_remove_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    *label = lv_label_create(cont);
    lv_label_set_long_mode(*label, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_font(*label, STYLE_FONT_MEDIUM, LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(*label, LV_TEXT_ALIGN_CENTER, LV_STATE_DEFAULT);
    lv_obj_set_width(*label, PARAM_WINDOW_WIDTH - 10);
    lv_obj_center(*label);

    return cont;
}


static lv_obj_t *icon_button_create(lv_obj_t *parent, const lv_img_dsc_t *img_dsc, uint16_t id, uint16_t number) {
    lv_obj_t *button = lv_button_create(parent);
    lv_obj_set_size(button, 40, 40);
    lv_obj_add_style(button, &style_dark_border, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(button, STYLE_COLOR_GRAY, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(button, STYLE_COLOR_BLACK, LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(button, 0, LV_STATE_DEFAULT);
    lv_obj_remove_flag(button, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *image = lv_image_create(button);
    lv_image_set_src(image, img_dsc);
    lv_obj_center(image);

    view_register_object_default_callback_with_number(button, id, number);

    return button;
}



const pman_page_t page_programs = {
    .create        = create_page,
    .destroy       = pman_destroy_all,
    .open          = open_page,
    .close         = pman_close_all,
    .process_event = page_event,
};
