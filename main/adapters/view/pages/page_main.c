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


LV_IMAGE_DECLARE(img_blender);
LV_IMAGE_DECLARE(img_emergency);
LV_IMAGE_DECLARE(img_left_hand);
LV_IMAGE_DECLARE(img_right_hand);
LV_IMAGE_DECLARE(img_settings);
LV_IMAGE_DECLARE(img_wifi);


#define BAR_TIME_WIDTH 310


enum {
    BTN_SETTINGS_ID,
    BTN_SCREEN_ID,
};


struct page_data {
    lv_obj_t *label_date;
    lv_obj_t *label_time;
    lv_obj_t *label_speed;
    lv_obj_t *label_max_speed;
    lv_obj_t *label_program;
    lv_obj_t *label_acceleration_time;
    lv_obj_t *label_cruise_time;
    lv_obj_t *label_deceleration_time;
    lv_obj_t *label_popup_message;

    lv_obj_t *image_emergency;
    lv_obj_t *image_left_hand;
    lv_obj_t *image_right_hand;

    lv_obj_t *obj_bar_acceleration_time;
    lv_obj_t *obj_bar_cruise_time;
    lv_obj_t *obj_bar_deceleration_time;
    lv_obj_t *obj_bar_time;
    lv_obj_t *obj_speed;
    lv_obj_t *obj_popup_message;

    pman_timer_t *timer;

    view_controller_msg_t cmsg;
};

static void update_page(model_t *model, struct page_data *pdata);
static void activate_click(lv_obj_t *obj);


static const char *TAG = __FILE_NAME__;


static void *create_page(pman_handle_t handle, void *extra) {
    (void)handle;
    (void)extra;

    struct page_data *pdata = lv_malloc(sizeof(struct page_data));
    assert(pdata != NULL);

    pdata->timer = PMAN_REGISTER_TIMER_ID(handle, 250, 0);

    return pdata;
}


static void open_page(pman_handle_t handle, void *state) {
    struct page_data *pdata = state;

    pman_timer_resume(pdata->timer);

    model_t *model = pman_get_user_data(handle);

    lv_obj_t *cont = lv_obj_create(lv_screen_active());
    lv_obj_add_style(cont, &style_background_cont, LV_STATE_DEFAULT);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_center(cont);
    activate_click(cont);

    {
        lv_obj_t *top_bar = lv_obj_create(cont);
        lv_obj_set_size(top_bar, LV_PCT(100), 40);
        lv_obj_add_style(top_bar, &style_metallic_cont, LV_STATE_DEFAULT);
        lv_obj_set_style_pad_hor(top_bar, 4, LV_STATE_DEFAULT);
        lv_obj_remove_flag(top_bar, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_align(top_bar, LV_ALIGN_TOP_MID, 0, 0);
        activate_click(top_bar);

        lv_obj_t *label_time = lv_label_create(top_bar);
        lv_obj_set_style_text_font(label_time, STYLE_FONT_MEDIUM, LV_STATE_DEFAULT);
        lv_obj_align(label_time, LV_ALIGN_TOP_MID, 0, 1);
        pdata->label_time = label_time;

        lv_obj_t *label_date = lv_label_create(top_bar);
        lv_obj_set_style_text_font(label_date, STYLE_FONT_SMALL, LV_STATE_DEFAULT);
        lv_obj_align(label_date, LV_ALIGN_BOTTOM_MID, 0, -1);
        pdata->label_date = label_date;

        lv_obj_t *button_settings = lv_button_create(top_bar);
        lv_obj_add_style(button_settings, &style_icon_button_rel, LV_STATE_DEFAULT);
        lv_obj_add_style(button_settings, &style_icon_button_pr, LV_STATE_PRESSED);
        lv_obj_set_size(button_settings, 40, 40);
        lv_obj_t *image_settings = lv_image_create(button_settings);
        lv_image_set_src(image_settings, &img_settings);
        lv_obj_add_style(image_settings, &style_icon, LV_STATE_DEFAULT);
        lv_obj_center(image_settings);
        lv_obj_align(button_settings, LV_ALIGN_RIGHT_MID, 0, 0);
        view_register_object_default_callback(button_settings, BTN_SETTINGS_ID);

        lv_obj_t *button_wifi = lv_button_create(top_bar);
        lv_obj_add_style(button_wifi, &style_icon_button_rel, LV_STATE_DEFAULT);
        lv_obj_add_style(button_wifi, &style_icon_button_pr, LV_STATE_PRESSED);
        lv_obj_set_size(button_wifi, 40, 40);
        lv_obj_t *image_wifi = lv_image_create(button_wifi);
        lv_image_set_src(image_wifi, &img_wifi);
        lv_obj_add_style(image_wifi, &style_icon, LV_STATE_DEFAULT);
        lv_obj_center(image_wifi);
        lv_obj_align(button_wifi, LV_ALIGN_LEFT_MID, 0, 0);
    }

    {
        lv_obj_t *obj_speed = lv_obj_create(cont);
        lv_obj_remove_flag(obj_speed, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_border_width(obj_speed, 0, LV_STATE_DEFAULT);
        lv_obj_set_size(obj_speed, 48, 32);
        pdata->obj_speed = obj_speed;

        lv_obj_t *label_speed = lv_label_create(obj_speed);
        lv_obj_set_style_text_font(label_speed, &lv_font_montserrat_10, LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(label_speed, LV_TEXT_ALIGN_CENTER, LV_STATE_DEFAULT);
        lv_obj_align(label_speed, LV_ALIGN_CENTER, 0, 0);
        pdata->label_speed = label_speed;

        lv_obj_t *image_blender = lv_image_create(cont);
        lv_image_set_src(image_blender, &img_blender);
        lv_obj_align(image_blender, LV_ALIGN_TOP_MID, 0, 48);
        activate_click(image_blender);

        lv_obj_align_to(obj_speed, image_blender, LV_ALIGN_CENTER, 0, -4);

        lv_obj_t *image_left_hand = lv_image_create(cont);
        lv_image_set_src(image_left_hand, &img_left_hand);
        lv_obj_set_style_image_recolor_opa(image_left_hand, LV_OPA_COVER, LV_STATE_DEFAULT);
        lv_obj_align_to(image_left_hand, image_blender, LV_ALIGN_LEFT_MID, -12, -34);
        activate_click(image_left_hand);
        pdata->image_left_hand = image_left_hand;

        lv_obj_t *image_right_hand = lv_image_create(cont);
        lv_image_set_src(image_right_hand, &img_right_hand);
        lv_obj_set_style_image_recolor_opa(image_right_hand, LV_OPA_COVER, LV_STATE_DEFAULT);
        lv_obj_align_to(image_right_hand, image_blender, LV_ALIGN_RIGHT_MID, +12, -34);
        activate_click(image_right_hand);
        pdata->image_right_hand = image_right_hand;

        lv_obj_t *image_emergency = lv_image_create(cont);
        lv_image_set_src(image_emergency, &img_emergency);
        lv_obj_align_to(image_emergency, image_blender, LV_ALIGN_BOTTOM_RIGHT, 36, 2);
        activate_click(image_emergency);
        pdata->image_emergency = image_emergency;

        lv_obj_t *obj_popup_message = lv_obj_create(cont);
        lv_obj_remove_flag(obj_popup_message, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_style(obj_popup_message, &style_dark_border, LV_STATE_DEFAULT);
        lv_obj_set_size(obj_popup_message, 310, 48);
        lv_obj_align_to(obj_popup_message, image_blender, LV_ALIGN_BOTTOM_MID, 0, -16);
        activate_click(obj_popup_message);
        pdata->obj_popup_message = obj_popup_message;

        lv_obj_t *label_popup_message = lv_label_create(obj_popup_message);
        lv_label_set_long_mode(label_popup_message, LV_LABEL_LONG_WRAP);
        lv_obj_set_width(label_popup_message, 300);
        lv_obj_set_style_text_align(label_popup_message, LV_TEXT_ALIGN_CENTER, LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(label_popup_message, STYLE_FONT_SMALL, LV_STATE_DEFAULT);
        lv_obj_center(label_popup_message);
        pdata->label_popup_message = label_popup_message;
    }

    {
        lv_obj_t *bottom_bar = lv_obj_create(cont);
        lv_obj_set_size(bottom_bar, LV_PCT(100), 88);
        lv_obj_add_style(bottom_bar, &style_metallic_cont, LV_STATE_DEFAULT);
        lv_obj_set_style_pad_hor(bottom_bar, 8, LV_STATE_DEFAULT);
        lv_obj_set_style_pad_top(bottom_bar, 2, LV_STATE_DEFAULT);
        lv_obj_remove_flag(bottom_bar, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_align(bottom_bar, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_add_flag(bottom_bar, LV_OBJ_FLAG_CLICKABLE);
        view_register_object_default_callback(bottom_bar, BTN_SCREEN_ID);

        lv_obj_t *label_max_speed = lv_label_create(bottom_bar);
        lv_obj_set_style_text_font(label_max_speed, STYLE_FONT_SMALL, LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(label_max_speed, LV_TEXT_ALIGN_LEFT, LV_STATE_DEFAULT);
        lv_obj_align(label_max_speed, LV_ALIGN_TOP_LEFT, 0, 0);
        pdata->label_max_speed = label_max_speed;

        lv_obj_t *label_program = lv_label_create(bottom_bar);
        lv_obj_set_style_text_font(label_program, STYLE_FONT_SMALL, LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(label_program, LV_TEXT_ALIGN_RIGHT, LV_STATE_DEFAULT);
        lv_obj_align(label_program, LV_ALIGN_TOP_RIGHT, 0, 0);
        pdata->label_program = label_program;

        lv_obj_t *obj_speed_bar = lv_obj_create(bottom_bar);
        // lv_obj_set_style_bg_color(obj_speed_bar, STYLE_COLOR_GREEN, LV_STATE_DEFAULT);
        lv_obj_set_style_pad_all(obj_speed_bar, 0, LV_STATE_DEFAULT);
        lv_obj_set_style_pad_column(obj_speed_bar, 0, LV_STATE_DEFAULT);
        lv_obj_set_style_pad_row(obj_speed_bar, 0, LV_STATE_DEFAULT);
        lv_obj_remove_flag(obj_speed_bar, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_style(obj_speed_bar, &style_dark_border, LV_STATE_DEFAULT);
        lv_obj_set_size(obj_speed_bar, BAR_TIME_WIDTH, 24);
        // lv_obj_set_layout(obj_speed_bar, LV_LAYOUT_FLEX);
        // lv_obj_set_flex_flow(obj_speed_bar, LV_FLEX_FLOW_ROW);
        // lv_obj_set_flex_align(obj_speed_bar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_align(obj_speed_bar, LV_ALIGN_BOTTOM_MID, 0, -30);
        activate_click(obj_speed_bar);

        lv_obj_t **obj_bars[] = {
            &pdata->obj_bar_acceleration_time,
            &pdata->obj_bar_cruise_time,
            &pdata->obj_bar_deceleration_time,
        };
        lv_color_t colors[] = {STYLE_COLOR_RED, STYLE_COLOR_YELLOW, STYLE_COLOR_GREEN};
        for (uint16_t i = 0; i < 3; i++) {
            lv_obj_t *obj_bar = lv_obj_create(obj_speed_bar);
            lv_obj_remove_flag(obj_bar, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_height(obj_bar, LV_PCT(100));
            lv_obj_set_style_border_width(obj_bar, 0, LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj_bar, 0, LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj_bar, colors[i], LV_STATE_DEFAULT);
            activate_click(obj_bar);

            // lv_obj_set_flex_grow(obj_bar, 1);

            if (i == 0) {
                lv_obj_align(obj_bar, LV_ALIGN_LEFT_MID, 0, 0);
            } else {
                lv_obj_align_to(obj_bar, *obj_bars[i - 1], LV_ALIGN_OUT_RIGHT_MID, 0, 0);
            }
            *obj_bars[i] = obj_bar;
        }

        lv_obj_t *obj_bar_time = lv_obj_create(obj_speed_bar);
        lv_obj_set_size(obj_bar_time, LV_PCT(100), LV_PCT(100));
        lv_obj_add_flag(obj_bar_time, LV_OBJ_FLAG_IGNORE_LAYOUT);
        lv_obj_remove_flag(obj_bar_time, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_bg_opa(obj_bar_time, LV_OPA_70, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(obj_bar_time, STYLE_COLOR_GRAY, LV_STATE_DEFAULT);
        lv_obj_set_style_radius(obj_bar_time, 0, LV_STATE_DEFAULT);
        lv_obj_set_style_border_side(obj_bar_time, LV_BORDER_SIDE_LEFT, LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(obj_bar_time, 4, LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(obj_bar_time, STYLE_COLOR_BLACK, LV_STATE_DEFAULT);
        activate_click(obj_bar_time);
        pdata->obj_bar_time = obj_bar_time;

        lv_obj_t *label_acceleration_time = lv_label_create(bottom_bar);
        lv_obj_set_style_text_font(label_acceleration_time, STYLE_FONT_SMALL, LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(label_acceleration_time, LV_TEXT_ALIGN_LEFT, LV_STATE_DEFAULT);
        lv_obj_align(label_acceleration_time, LV_ALIGN_BOTTOM_LEFT, 0, 0);
        pdata->label_acceleration_time = label_acceleration_time;

        lv_obj_t *label_cruise_time = lv_label_create(bottom_bar);
        lv_obj_set_style_text_font(label_cruise_time, STYLE_FONT_SMALL, LV_STATE_DEFAULT);
        lv_obj_align(label_cruise_time, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_set_style_text_align(label_cruise_time, LV_TEXT_ALIGN_CENTER, LV_STATE_DEFAULT);
        pdata->label_cruise_time = label_cruise_time;

        lv_obj_t *label_deceleration_time = lv_label_create(bottom_bar);
        lv_obj_set_style_text_font(label_deceleration_time, STYLE_FONT_SMALL, LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(label_deceleration_time, LV_TEXT_ALIGN_RIGHT, LV_STATE_DEFAULT);
        lv_obj_align(label_deceleration_time, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
        pdata->label_deceleration_time = label_deceleration_time;
    }
    LV_IMAGE_DECLARE(test);

    VIEW_ADD_WATCHED_VARIABLE(&model->run.left_button_pressed, 0);
    VIEW_ADD_WATCHED_VARIABLE(&model->run.right_button_pressed, 0);
    VIEW_ADD_WATCHED_VARIABLE(&model->run.emergency_ok, 0);
    VIEW_ADD_WATCHED_VARIABLE(&model->run.cycle.state_machine.node_index, 0);

    update_page(model, pdata);

    ESP_LOGI(TAG, "Main open");
}


static pman_msg_t page_event(pman_handle_t handle, void *state, pman_event_t event) {
    (void)handle;

    pman_msg_t msg = PMAN_MSG_NULL;

    struct page_data *pdata = state;

    mut_model_t *model = pman_get_user_data(handle);

    msg.user_msg    = &pdata->cmsg;
    pdata->cmsg.tag = VIEW_CONTROLLER_MESSAGE_TAG_NOTHING;

    switch (event.tag) {
        case PMAN_EVENT_TAG_TIMER: {
            update_page(model, pdata);
            break;
        }

        case PMAN_EVENT_TAG_USER: {
            view_event_t *view_event = event.as.user;
            switch (view_event->tag) {
                case VIEW_EVENT_TAG_PAGE_WATCHER: {
                    update_page(model, pdata);
                    break;
                }

                case VIEW_EVENT_TAG_LEFT_HAND_CLICK: {
                    if (model_is_not_running(model)) {
                        if (model->run.program_index > 0) {
                            model->run.program_index--;
                        } else {
                            model->run.program_index = PROGRAMS_NUM - 1;
                        }
                        model_stop(model);
                        update_page(model, pdata);
                    }
                    break;
                }
                case VIEW_EVENT_TAG_RIGHT_HAND_CLICK: {
                    if (model_is_not_running(model)) {
                        model->run.program_index = (model->run.program_index + 1) % PROGRAMS_NUM;
                        model_stop(model);
                        update_page(model, pdata);
                    }
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

            switch (lv_event_get_code(lv_event)) {
                case LV_EVENT_CLICKED: {
                    switch (id) {
                        case BTN_SETTINGS_ID: {
                            if (model_is_not_running(model)) {
                                msg.stack_msg = PMAN_STACK_MSG_PUSH_PAGE(&page_menu);
                            }
                            break;
                        }

                        case BTN_SCREEN_ID: {
                            if (model_is_not_running(model)) {
                                model_stop(model);
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

            break;
        }

        default:
            break;
    }

    return msg;
}


static char *format_time(uint32_t seconds) {
    static char string[32] = {0};
    if (seconds < 60) {
        snprintf(string, sizeof(string), "%02i sec", (int)seconds);
    } else if ((seconds % 60) > 30) {
        snprintf(string, sizeof(string), "%02i min", (int)(seconds / 60) + 1);
    } else {
        snprintf(string, sizeof(string), "%02i min", (int)seconds / 60);
    }
    return string;
}


static void update_page(model_t *model, struct page_data *pdata) {
    const program_t *program     = model_get_current_program(model);
    cycle_state_t    cycle_state = model->run.cycle.state_machine.node_index;

    lv_label_set_text_fmt(pdata->label_max_speed, "MAX SPEED\n%03i%%", program->max_speed);
    lv_label_set_text_fmt(pdata->label_program, "Program: %03i\n%s", model->run.program_index + 1, program->name);
    lv_label_set_text_fmt(pdata->label_speed, "%03i%%", model_get_speed_percentage(model));

    switch (cycle_state) {
        case CYCLE_STATE_ACCELERATING:
        case CYCLE_STATE_CRUISE:
        case CYCLE_STATE_DECELERATING:
        case CYCLE_STATE_INTERRUPTED: {
            lv_label_set_text_fmt(pdata->label_acceleration_time, "%s\nSTART",
                                  format_time(model_get_program_elapsed_time_seconds(model)));
            lv_label_set_text_fmt(pdata->label_cruise_time, "%s\nRUN TIME",
                                  format_time(model_get_program_total_time_seconds(model)));
            lv_label_set_text_fmt(pdata->label_deceleration_time, "%s\nSTOP",
                                  format_time(model_get_program_remaining_time_seconds(model)));
            break;
        }

        default: {
            lv_label_set_text_fmt(pdata->label_acceleration_time, "%02i min\nRamp UP",
                                  program->acceleration_time_minutes);
            lv_label_set_text_fmt(pdata->label_cruise_time, "%02i min\nMAX TIME", program->max_speed_time_minutes);
            lv_label_set_text_fmt(pdata->label_deceleration_time, "%02i min\nRamp DOWN",
                                  program->deceleration_time_minutes);
            break;
        }
    }

    lv_obj_set_style_image_recolor(
        pdata->image_left_hand, model->run.left_button_pressed ? STYLE_COLOR_GREEN : STYLE_COLOR_RED, LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor(pdata->image_right_hand,
                                   model->run.right_button_pressed ? STYLE_COLOR_GREEN : STYLE_COLOR_RED,
                                   LV_STATE_DEFAULT);
    view_common_set_hidden(pdata->image_emergency, model->run.emergency_ok);

    struct tm done_time = *localtime(&model->run.cycle.done_time);

    switch (cycle_state) {
        case CYCLE_STATE_INTERRUPTED: {
            view_common_set_hidden(pdata->obj_popup_message, 0);
            lv_label_set_text_fmt(pdata->label_popup_message, "THE PROGRAM WAS INTERRUPTED\n%02i:%02i %02i/%02i/%i",
                                  done_time.tm_hour, done_time.tm_min, done_time.tm_mday, done_time.tm_mon + 1,
                                  1900 + done_time.tm_year);
            break;
        }

        case CYCLE_STATE_SUCCESSFUL: {
            view_common_set_hidden(pdata->obj_popup_message, 0);
            lv_label_set_text_fmt(pdata->label_popup_message,
                                  "THE PROGRAM TERMINATED SUCCESSFULLY\n%02i:%02i %02i/%02i/%i", done_time.tm_hour,
                                  done_time.tm_min, done_time.tm_mday, done_time.tm_mon + 1, 1900 + done_time.tm_year);
            break;
        }

        default: {
            view_common_set_hidden(pdata->obj_popup_message, 1);
            break;
        }
    }

    switch (cycle_state) {
        case CYCLE_STATE_ACCELERATING:
            lv_obj_set_style_bg_color(pdata->obj_speed, STYLE_COLOR_RED, LV_STATE_DEFAULT);
            break;

        case CYCLE_STATE_CRUISE:
            lv_obj_set_style_bg_color(pdata->obj_speed, STYLE_COLOR_YELLOW, LV_STATE_DEFAULT);
            break;

        case CYCLE_STATE_DECELERATING:
            lv_obj_set_style_bg_color(pdata->obj_speed, STYLE_COLOR_GREEN, LV_STATE_DEFAULT);
            break;

        default:
            lv_obj_set_style_bg_color(pdata->obj_speed, STYLE_COLOR_WHITE, LV_STATE_DEFAULT);
            break;
    }

    // lv_obj_set_flex_grow(pdata->obj_bar_acceleration_time, model_get_program_acceleration_time_percentage(program));
    // lv_obj_set_flex_grow(pdata->obj_bar_cruise_time, model_get_program_cruise_time_percentage(program));
    // lv_obj_set_flex_grow(pdata->obj_bar_deceleration_time, model_get_program_deceleration_time_percentage(program));

    lv_obj_set_width(pdata->obj_bar_acceleration_time,
                     ((BAR_TIME_WIDTH - 4) * model_get_program_acceleration_time_permillage(program)) / 1000);
    lv_obj_set_width(pdata->obj_bar_cruise_time,
                     ((BAR_TIME_WIDTH - 4) * model_get_program_cruise_time_permillage(program)) / 1000);
    lv_obj_set_width(pdata->obj_bar_deceleration_time,
                     ((BAR_TIME_WIDTH - 4) * model_get_program_deceleration_time_permillage(program)) / 1000);

    lv_obj_align(pdata->obj_bar_acceleration_time, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_align_to(pdata->obj_bar_cruise_time, pdata->obj_bar_acceleration_time, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    lv_obj_align_to(pdata->obj_bar_deceleration_time, pdata->obj_bar_cruise_time, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

    uint16_t time_permillage = model_get_program_time_permillage(model);
    if (time_permillage >= 1000) {
        view_common_set_hidden(pdata->obj_bar_time, 1);
    } else {
        view_common_set_hidden(pdata->obj_bar_time, 0);
        uint32_t time_bar_position = 1 + ((BAR_TIME_WIDTH - 5) * time_permillage) / 1000;
        lv_obj_align(pdata->obj_bar_time, LV_ALIGN_LEFT_MID, time_bar_position, 0);
    }

    struct tm current_time = system_time_get();
    lv_label_set_text_fmt(pdata->label_date, "%02i/%02i/%02i", current_time.tm_mday, current_time.tm_mon + 1,
                          1900 + current_time.tm_year);
    lv_label_set_text_fmt(pdata->label_time, "%02i:%02i", current_time.tm_hour, current_time.tm_min);
}


static void close_page(void *state) {
    struct page_data *pdata = state;
    pman_timer_pause(pdata->timer);
    lv_obj_clean(lv_scr_act());
}


static void destroy_page(void *state, void *extra) {
    (void)extra;
    struct page_data *pdata = state;
    pman_timer_delete(pdata->timer);
    lv_free(pdata);
}


static void activate_click(lv_obj_t *obj) {
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    view_register_object_default_callback(obj, BTN_SCREEN_ID);
}


const pman_page_t page_main = {
    .create        = create_page,
    .destroy       = destroy_page,
    .open          = open_page,
    .close         = close_page,
    .process_event = page_event,
};
