#include <stdlib.h>
#include "page_manager.h"
#include "config/app_config.h"
#include "view.h"
#include "theme/style.h"
#include "theme/theme.h"
#include "esp_log.h"
#include "watcher.h"
#include "services/timestamp.h"
#ifdef BUILD_CONFIG_SIMULATED_APP
#include "sdl_window.h"
#endif


static void    watcher_cb(void *old_value, const void *memory, uint16_t size, void *user_ptr, void *arg);
static void    clear_watcher(pman_handle_t handle);
static uint8_t event_callback(pman_handle_t handle, pman_event_t event);


static const char *TAG = "View";

static struct {
    pman_t              pman;
    lv_display_t       *display;
    watcher_t           watcher;
    view_gui_protocol_t protocol;
} state = {
    .pman    = {},
    .display = NULL,
    .watcher = {},
};


void view_init(model_t *p_model, pman_user_msg_cb_t controller_cb, lv_display_flush_cb_t flush_cb,
               lv_indev_read_cb_t read_cb, view_gui_protocol_t protocol) {
    (void)TAG;

    state.protocol = protocol;
    lv_init();

    WATCHER_INIT_STD(&state.watcher, NULL);

#ifdef BUILD_CONFIG_SIMULATED_APP
    (void)flush_cb;
    (void)read_cb;

    state.display =
        sdl_window_create(BUILD_CONFIG_DISPLAY_HORIZONTAL_RESOLUTION, BUILD_CONFIG_DISPLAY_VERTICAL_RESOLUTION);
    lv_indev_t *touch_indev = lv_sdl_mouse_create();

#else

    state.display =
        lv_display_create(BUILD_CONFIG_DISPLAY_HORIZONTAL_RESOLUTION, BUILD_CONFIG_DISPLAY_VERTICAL_RESOLUTION);

    /*Static or global buffer(s). The second buffer is optional*/
    static lv_color_t buf_1[VIEW_LVGL_BUFFER_SIZE] = {0};

    /*Initialize `disp_buf` with the buffer(s). With only one buffer use NULL instead buf_2 */
    lv_display_set_buffers(state.display, buf_1, NULL, VIEW_LVGL_BUFFER_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(state.display, flush_cb);

    style_init();
    theme_init(state.display);

    lv_indev_t *touch_indev = lv_indev_create();
    lv_indev_set_type(touch_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(touch_indev, read_cb);

#endif

    pman_init(&state.pman, (void *)p_model, touch_indev, controller_cb, clear_watcher, event_callback);
}


view_gui_protocol_t *view_get_protocol(pman_handle_t handle) {
    (void)handle;
    return &state.protocol;
}


void view_manage(void) {
    watcher_watch(&state.watcher, timestamp_get());
}


void view_change_page(const pman_page_t *page) {
    pman_change_page(&state.pman, *page);
}


void view_display_flush_ready(void) {
    if (state.display) {
        lv_display_flush_ready(state.display);
    }
}


void view_add_watched_variable(void *ptr, size_t size, int code) {
    watcher_add_entry(&state.watcher, ptr, size, watcher_cb, (void *)(uintptr_t)code);
}


void view_event(view_event_t event) {
    pman_event(&state.pman, (pman_event_t){.tag = PMAN_EVENT_TAG_USER, .as = {.user = &event}});
}


uint16_t view_get_obj_id(lv_obj_t *obj) {
    return (((uint32_t)(uintptr_t)lv_obj_get_user_data(obj)) & 0xFFFF);
}


uint16_t view_get_obj_number(lv_obj_t *obj) {
    return ((((uint32_t)(uintptr_t)lv_obj_get_user_data(obj)) >> 16) & 0xFFFF);
}


void view_register_object_default_callback_with_number(lv_obj_t *obj, uint16_t id, uint16_t number) {
    lv_obj_set_user_data(obj, (void *)(uintptr_t)((id & 0xFFFF) | ((number & 0xFFFF) << 16)));

    pman_unregister_obj_event(obj);
    pman_register_obj_event(&state.pman, obj, LV_EVENT_CLICKED);
    pman_register_obj_event(&state.pman, obj, LV_EVENT_VALUE_CHANGED);
    pman_register_obj_event(&state.pman, obj, LV_EVENT_RELEASED);
    pman_register_obj_event(&state.pman, obj, LV_EVENT_PRESSED);
    pman_register_obj_event(&state.pman, obj, LV_EVENT_PRESSING);
    pman_register_obj_event(&state.pman, obj, LV_EVENT_LONG_PRESSED);
    pman_register_obj_event(&state.pman, obj, LV_EVENT_LONG_PRESSED_REPEAT);
    pman_register_obj_event(&state.pman, obj, LV_EVENT_CANCEL);
    pman_register_obj_event(&state.pman, obj, LV_EVENT_READY);
}


void view_register_object_default_callback(lv_obj_t *obj, uint16_t id) {
    view_register_object_default_callback_with_number(obj, id, 0);
}


static uint8_t event_callback(pman_handle_t handle, pman_event_t event) {
    switch (event.tag) {
        case PMAN_EVENT_TAG_LVGL: {
            switch (event.as.lvgl->code) {
                case LV_EVENT_LONG_PRESSED:
                case LV_EVENT_CLICKED:
                    view_get_protocol(handle)->beep(handle);
                    break;
                default:
                    break;
            }

            break;
        }

        default:
            break;
    }

    return 0;
}


static void watcher_cb(void *old_value, const void *memory, uint16_t size, void *user_ptr, void *arg) {
    (void)old_value;
    (void)memory;
    (void)size;
    (void)user_ptr;
    view_event((view_event_t){.tag = VIEW_EVENT_TAG_PAGE_WATCHER, .as.page_watcher.code = (uintptr_t)arg});
}


static void clear_watcher(pman_handle_t handle) {
    (void)handle;
    watcher_destroy(&state.watcher);
    WATCHER_INIT_STD(&state.watcher, NULL);
}
