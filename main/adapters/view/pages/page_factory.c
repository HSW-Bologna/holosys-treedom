#include <assert.h>
#include <stdlib.h>
#include "lvgl.h"
#include "model/model.h"
#include "../view.h"
#include "../theme/style.h"
#include "../intl/intl.h"
#include "../common.h"
#include <esp_log.h>


LV_IMAGE_DECLARE(img_factory_big);
LV_IMAGE_DECLARE(img_warning);
LV_IMAGE_DECLARE(img_minus);
LV_IMAGE_DECLARE(img_plus);


#define PARAM_WINDOW_WIDTH 230


enum {
    BTN_BACK_ID,
    BTN_HOME_ID,
    BTN_HIDDEN_ID,
    BTN_FACTORY_RESET_ID,
    BTN_AUTO_MODE_ID,
};


struct page_data {
    lv_obj_t *label_auto_mode;

    lv_obj_t *image_warning;

    lv_obj_t *button_minus;
    lv_obj_t *button_plus;

    lv_obj_t *obj_auto_mode;

    uint8_t     show_auto_mode;
    timestamp_t timestamp;
};


static void      update_page(model_t *model, struct page_data *pdata);
static lv_obj_t *param_window_create(lv_obj_t *parent, lv_obj_t **label);
static lv_obj_t *icon_button_create(lv_obj_t *parent, const lv_img_dsc_t *img_dsc, uint16_t id, uint16_t number);


static const char *TAG = __FILE_NAME__;


static void *create_page(pman_handle_t handle, void *extra) {
    (void)extra;

    struct page_data *pdata = lv_malloc(sizeof(struct page_data));
    assert(pdata != NULL);

    model_t *model        = pman_get_user_data(handle);
    pdata->show_auto_mode = model->config.auto_mode;
    pdata->timestamp      = 0;

    return pdata;
}


static void open_page(pman_handle_t handle, void *state) {
    struct page_data *pdata = state;

    model_t *model = pman_get_user_data(handle);

    lv_obj_t *cont = lv_obj_create(lv_screen_active());
    lv_obj_add_style(cont, &style_background_cont, LV_STATE_DEFAULT);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_center(cont);

    view_common_title_create(cont, "FACTORY DEFAULT", BTN_HOME_ID, BTN_BACK_ID);

    lv_obj_t *image_factory = lv_image_create(cont);
    lv_image_set_src(image_factory, &img_factory_big);
    lv_obj_add_flag(image_factory, LV_OBJ_FLAG_CLICKABLE);
    view_register_object_default_callback(image_factory, BTN_HIDDEN_ID);
    lv_obj_align(image_factory, LV_ALIGN_TOP_MID, 0, 56);

    lv_obj_t *param        = param_window_create(cont, &pdata->label_auto_mode);
    lv_obj_t *button_left  = icon_button_create(cont, &img_minus, BTN_AUTO_MODE_ID, -1);
    lv_obj_t *button_right = icon_button_create(cont, &img_plus, BTN_AUTO_MODE_ID, +1);

    lv_obj_align(param, LV_ALIGN_BOTTOM_MID, 0, -56);
    lv_obj_align_to(button_left, param, LV_ALIGN_OUT_LEFT_MID, -2, 0);
    lv_obj_align_to(button_right, param, LV_ALIGN_OUT_RIGHT_MID, +2, 0);
    pdata->obj_auto_mode = param;
    pdata->button_minus  = button_left;
    pdata->button_plus   = button_right;

    lv_obj_t *image_warning = lv_image_create(cont);
    lv_image_set_src(image_warning, &img_warning);
    lv_obj_add_style(image_warning, &style_icon, LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor(image_warning, STYLE_COLOR_RED, LV_STATE_DEFAULT);
    lv_obj_align_to(image_warning, param, LV_ALIGN_OUT_TOP_MID, 0, -2);
    pdata->image_warning = image_warning;

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

            switch (lv_event_get_code(lv_event)) {
                case LV_EVENT_PRESSED: {
                    switch (id) {
                        case BTN_HIDDEN_ID: {
                            pdata->timestamp = timestamp_get();
                            break;
                        }

                        default:
                            break;
                    }

                    break;
                }

                case LV_EVENT_PRESSING: {
                    switch (id) {
                        case BTN_HIDDEN_ID: {
                            if (timestamp_is_expired(pdata->timestamp, 15000UL)) {
                                pdata->show_auto_mode = 1;
                            }
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
                            msg.stack_msg = PMAN_STACK_MSG_BACK();
                            break;
                        }

                        case BTN_HOME_ID: {
                            msg.stack_msg = PMAN_STACK_MSG_REBASE(&page_main);
                            break;
                        }

                        case BTN_AUTO_MODE_ID: {
                            model->config.auto_mode = !model->config.auto_mode;
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
    lv_label_set_text_fmt(pdata->label_auto_mode, "Auto Mode: %s", model->config.auto_mode ? "ON" : "OFF");
    view_common_set_hidden(pdata->obj_auto_mode, !pdata->show_auto_mode);
    view_common_set_hidden(pdata->button_minus, !pdata->show_auto_mode);
    view_common_set_hidden(pdata->button_plus, !pdata->show_auto_mode);
    view_common_set_hidden(pdata->image_warning, !pdata->show_auto_mode);
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



const pman_page_t page_factory = {
    .create        = create_page,
    .destroy       = pman_destroy_all,
    .open          = open_page,
    .close         = pman_close_all,
    .process_event = page_event,
};
