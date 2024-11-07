#include <assert.h>
#include <stdlib.h>
#include "lvgl.h"
#include "model/model.h"
#include "../view.h"
#include "../theme/style.h"
#include "../intl/intl.h"
#include "../common.h"
#include <esp_log.h>
#include "services/system_time.h"


LV_IMAGE_DECLARE(img_left);
LV_IMAGE_DECLARE(img_right);
LV_IMAGE_DECLARE(img_edit);
LV_IMAGE_DECLARE(img_minus);
LV_IMAGE_DECLARE(img_plus);


#define PARAM_WINDOW_WIDTH 230


enum {
    BTN_BACK_ID,
    BTN_HOME_ID,
    BTN_HOUR_ID,
    BTN_MINUTE_ID,
    BTN_DAY_ID,
    BTN_MONTH_ID,
    BTN_YEAR_ID,
};


struct page_data {
    lv_obj_t *label_hours;
    lv_obj_t *label_minutes;
    lv_obj_t *label_day;
    lv_obj_t *label_month;
    lv_obj_t *label_year;
    lv_obj_t *label_date;
    lv_obj_t *label_time;

    struct tm current_time;

    uint8_t modified;
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

    pdata->modified     = 0;
    pdata->current_time = system_time_get();

    return pdata;
}


static void open_page(pman_handle_t handle, void *state) {
    struct page_data *pdata = state;

    model_t *model = pman_get_user_data(handle);

    lv_obj_t *cont = lv_obj_create(lv_screen_active());
    lv_obj_add_style(cont, &style_background_cont, LV_STATE_DEFAULT);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_center(cont);

    view_common_title_create(cont, "DATE & TIME", BTN_HOME_ID, BTN_BACK_ID);


    lv_obj_t *label_time = lv_label_create(cont);
    lv_obj_set_style_text_font(label_time, STYLE_FONT_MEDIUM, LV_STATE_DEFAULT);
    lv_obj_align(label_time, LV_ALIGN_TOP_MID, 0, 48);
    pdata->label_time = label_time;

    lv_obj_t *label_date = lv_label_create(cont);
    lv_obj_set_style_text_font(label_date, STYLE_FONT_SMALL, LV_STATE_DEFAULT);
    lv_obj_align(label_date, LV_ALIGN_TOP_MID, 0, 64);
    pdata->label_date = label_date;


    lv_obj_t *param        = param_window_create(cont, &pdata->label_hours);
    lv_obj_t *button_left  = icon_button_create(cont, &img_minus, BTN_HOUR_ID, -1);
    lv_obj_t *button_right = icon_button_create(cont, &img_plus, BTN_HOUR_ID, +1);
    lv_obj_t *param_prev   = param;

    lv_obj_align(param, LV_ALIGN_TOP_MID, 0, 96);
    lv_obj_align_to(button_left, param, LV_ALIGN_OUT_LEFT_MID, -2, 0);
    lv_obj_align_to(button_right, param, LV_ALIGN_OUT_RIGHT_MID, +2, 0);


    param        = param_window_create(cont, &pdata->label_minutes);
    button_left  = icon_button_create(cont, &img_minus, BTN_MINUTE_ID, -1);
    button_right = icon_button_create(cont, &img_plus, BTN_MINUTE_ID, +1);

    lv_obj_align_to(param, param_prev, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    lv_obj_align_to(button_left, param, LV_ALIGN_OUT_LEFT_MID, -2, 0);
    lv_obj_align_to(button_right, param, LV_ALIGN_OUT_RIGHT_MID, +2, 0);
    param_prev = param;


    param        = param_window_create(cont, &pdata->label_day);
    button_left  = icon_button_create(cont, &img_minus, BTN_DAY_ID, -1);
    button_right = icon_button_create(cont, &img_plus, BTN_DAY_ID, +1);

    lv_obj_align_to(param, param_prev, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    lv_obj_align_to(button_left, param, LV_ALIGN_OUT_LEFT_MID, -2, 0);
    lv_obj_align_to(button_right, param, LV_ALIGN_OUT_RIGHT_MID, +2, 0);
    param_prev = param;


    param        = param_window_create(cont, &pdata->label_month);
    button_left  = icon_button_create(cont, &img_minus, BTN_MONTH_ID, -1);
    button_right = icon_button_create(cont, &img_plus, BTN_MONTH_ID, +1);

    lv_obj_align_to(param, param_prev, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    lv_obj_align_to(button_left, param, LV_ALIGN_OUT_LEFT_MID, -2, 0);
    lv_obj_align_to(button_right, param, LV_ALIGN_OUT_RIGHT_MID, +2, 0);
    param_prev = param;


    param        = param_window_create(cont, &pdata->label_year);
    button_left  = icon_button_create(cont, &img_minus, BTN_YEAR_ID, -1);
    button_right = icon_button_create(cont, &img_plus, BTN_YEAR_ID, +1);

    lv_obj_align_to(param, param_prev, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    lv_obj_align_to(button_left, param, LV_ALIGN_OUT_LEFT_MID, -2, 0);
    lv_obj_align_to(button_right, param, LV_ALIGN_OUT_RIGHT_MID, +2, 0);
    param_prev = param;


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
                    break;
                }

                case VIEW_EVENT_TAG_RIGHT_HAND_CLICK: {
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

            switch (lv_event_get_code(lv_event)) {
                case LV_EVENT_CLICKED: {
                    switch (id) {
                        case BTN_BACK_ID: {
                            if (pdata->modified) {
                                view_get_protocol(handle)->set_time(handle, pdata->current_time);
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

                        case BTN_HOUR_ID: {
                            pdata->modified = 1;
                            if (number > 0) {
                                pdata->current_time.tm_hour = (pdata->current_time.tm_hour + 1) % 24;
                            } else if (number < 0) {
                                if (pdata->current_time.tm_hour > 0) {
                                    pdata->current_time.tm_hour--;
                                } else {
                                    pdata->current_time.tm_hour = 23;
                                }
                            }
                            update_page(model, pdata);
                            break;
                        }

                        case BTN_MINUTE_ID: {
                            pdata->modified = 1;
                            if (number > 0) {
                                pdata->current_time.tm_min = (pdata->current_time.tm_min + 1) % 60;
                            } else if (number < 0) {
                                if (pdata->current_time.tm_min > 0) {
                                    pdata->current_time.tm_min--;
                                } else {
                                    pdata->current_time.tm_min = 59;
                                }
                            }
                            update_page(model, pdata);
                            break;
                        }

                        case BTN_DAY_ID: {
                            pdata->modified = 1;

                            if (number > 0) {
                                pdata->current_time.tm_mday++;
                            } else if (number < 0) {
                                if (pdata->current_time.tm_mday > 1) {
                                    pdata->current_time.tm_mday--;
                                }
                            }

                            time_t correct_time = mktime(&pdata->current_time);
                            pdata->current_time = *localtime(&correct_time);

                            update_page(model, pdata);
                            break;
                        }

                        case BTN_MONTH_ID: {
                            pdata->modified = 1;
                            if (number > 0) {
                                pdata->current_time.tm_mon = (pdata->current_time.tm_mon + 1) % 12;
                            } else if (number < 0) {
                                if (pdata->current_time.tm_mon > 0) {
                                    pdata->current_time.tm_mon--;
                                } else {
                                    pdata->current_time.tm_mon = 11;
                                }
                            }

                            time_t correct_time = mktime(&pdata->current_time);
                            pdata->current_time = *localtime(&correct_time);

                            update_page(model, pdata);
                            break;
                        }


                        case BTN_YEAR_ID: {
                            pdata->modified = 1;
                            if (number > 0) {
                                pdata->current_time.tm_year++;
                            } else if (number < 0) {
                                if (pdata->current_time.tm_year > 124) {
                                    pdata->current_time.tm_year--;
                                }
                            }

                            time_t correct_time = mktime(&pdata->current_time);
                            pdata->current_time = *localtime(&correct_time);

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
    lv_label_set_text_fmt(pdata->label_date, "%02i/%02i/%02i", pdata->current_time.tm_mday,
                          pdata->current_time.tm_mon + 1, 1900 + pdata->current_time.tm_year);
    lv_label_set_text_fmt(pdata->label_time, "%02i:%02i", pdata->current_time.tm_hour, pdata->current_time.tm_min);
    lv_label_set_text_fmt(pdata->label_hours, "Hours: %02i", pdata->current_time.tm_hour);
    lv_label_set_text_fmt(pdata->label_minutes, "Minutes: %02i", pdata->current_time.tm_min);
    lv_label_set_text_fmt(pdata->label_day, "Day: %02i", pdata->current_time.tm_mday);
    lv_label_set_text_fmt(pdata->label_month, "Month: %02i", pdata->current_time.tm_mon + 1);
    lv_label_set_text_fmt(pdata->label_year, "Year: %02i", 1900 + pdata->current_time.tm_year);
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



const pman_page_t page_datetime = {
    .create        = create_page,
    .destroy       = pman_destroy_all,
    .open          = open_page,
    .close         = pman_close_all,
    .process_event = page_event,
};
