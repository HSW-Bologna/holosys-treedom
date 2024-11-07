#include <assert.h>
#include <stdlib.h>
#include "lvgl.h"
#include "model/model.h"
#include "../view.h"
#include "../theme/style.h"
#include "../intl/intl.h"
#include "../common.h"
#include <esp_log.h>
#include "config/app_config.h"


LV_IMAGE_DECLARE(img_programs);
LV_IMAGE_DECLARE(img_datetime);
LV_IMAGE_DECLARE(img_factory);


#define BAR_TIME_WIDTH 310


enum {
    BTN_BACK_ID,
    BTN_DATETIME_ID,
    BTN_FACTORY_ID,
    BTN_PROGRAMS_ID,
};


struct page_data {
    uint8_t placeholder;
};

static void update_page(model_t *model, struct page_data *pdata);


static const char *TAG = __FILE_NAME__;


static void *create_page(pman_handle_t handle, void *extra) {
    (void)handle;
    (void)extra;

    struct page_data *pdata = lv_malloc(sizeof(struct page_data));
    assert(pdata != NULL);

    return pdata;
}


static void open_page(pman_handle_t handle, void *state) {
    struct page_data *pdata = state;

    model_t *model = pman_get_user_data(handle);

    lv_obj_t *cont = lv_obj_create(lv_screen_active());
    lv_obj_add_style(cont, &style_background_cont, LV_STATE_DEFAULT);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_center(cont);

    view_common_title_create(cont, "MENU", BTN_BACK_ID, BTN_BACK_ID);

    lv_obj_t *btn_prev = view_common_menu_button(cont, "PROGRAMS", &img_programs, BTN_PROGRAMS_ID);
    lv_obj_align(btn_prev, LV_ALIGN_TOP_MID, 0, 48);

    lv_obj_t *btn = view_common_menu_button(cont, "DATE & TIME", &img_datetime, BTN_DATETIME_ID);
    lv_obj_align_to(btn, btn_prev, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    btn_prev = btn;

    btn = view_common_menu_button(cont, "FACTORY DEFAULT", &img_factory, BTN_FACTORY_ID);
    lv_obj_align_to(btn, btn_prev, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    btn_prev = btn;

    lv_obj_t *label_version = lv_label_create(cont);
    lv_obj_set_style_text_font(label_version, &lv_font_montserrat_10, LV_STATE_DEFAULT);
    lv_label_set_text_fmt(label_version, "v%i.%i.%i", (int)APP_CONFIG_FIRMWARE_VERSION_MAJOR,
                          (int)APP_CONFIG_FIRMWARE_VERSION_MINOR, (int)APP_CONFIG_FIRMWARE_VERSION_PATCH);
    lv_obj_align(label_version, LV_ALIGN_BOTTOM_MID, 0, 0);

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
                case LV_EVENT_CLICKED: {
                    switch (id) {
                        case BTN_BACK_ID: {
                            msg.stack_msg = PMAN_STACK_MSG_BACK();
                            break;
                        }

                        case BTN_PROGRAMS_ID:
                            msg.stack_msg = PMAN_STACK_MSG_PUSH_PAGE(&page_programs);
                            break;

                        case BTN_DATETIME_ID:
                            msg.stack_msg = PMAN_STACK_MSG_PUSH_PAGE(&page_datetime);
                            break;

                        case BTN_FACTORY_ID:
                            msg.stack_msg = PMAN_STACK_MSG_PUSH_PAGE(&page_factory);
                            break;

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
    (void)model;
    (void)pdata;
}


const pman_page_t page_menu = {
    .create        = create_page,
    .destroy       = pman_destroy_all,
    .open          = open_page,
    .close         = pman_close_all,
    .process_event = page_event,
};
