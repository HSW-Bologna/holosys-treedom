#ifndef VIEW_H_INCLUDED
#define VIEW_H_INCLUDED


#include <sys/time.h>
#include <time.h>
#include "model/model.h"
#include "page_manager.h"


#define VIEW_LVGL_BUFFER_SIZE                    (BUILD_CONFIG_DISPLAY_HORIZONTAL_RESOLUTION * 30)
#define VIEW_ADD_WATCHED_VARIABLE(ptr, code)     view_add_watched_variable((void *)ptr, sizeof(*ptr), code)
#define VIEW_ADD_WATCHED_ARRAY(ptr, items, code) view_add_watched_variable((void *)ptr, sizeof(*ptr) * (items), code)


typedef struct {
    void (*save_programs)(pman_handle_t handle);
    void (*set_time)(pman_handle_t handle, struct tm new_time);
    void (*beep)(pman_handle_t handle);
} view_gui_protocol_t;


typedef enum {
    VIEW_CONTROLLER_MESSAGE_TAG_NOTHING = 0,
} view_controller_msg_tag_t;

typedef struct {
    view_controller_msg_tag_t tag;
    union {
    } as;
} view_controller_msg_t;


typedef enum {
    VIEW_EVENT_TAG_PAGE_WATCHER,
    VIEW_EVENT_TAG_LEFT_HAND_CLICK,
    VIEW_EVENT_TAG_RIGHT_HAND_CLICK,
    VIEW_EVENT_TAG_BOTH_HANDS_LONGCLICK,
} view_event_tag_t;


typedef struct {
    view_event_tag_t tag;
    union {
        struct {
            int code;
        } page_watcher;
    } as;
} view_event_t;


void view_init(model_t *p_model, pman_user_msg_cb_t controller_cb, lv_display_flush_cb_t flush_cb,
               lv_indev_read_cb_t read_cb, view_gui_protocol_t protocol);

void                 view_change_page(const pman_page_t *page);
void                 view_display_flush_ready(void);
void                 view_event(view_event_t event);
void                 view_add_watched_variable(void *ptr, size_t size, int code);
void                 view_manage(void);
void                 view_register_object_default_callback(lv_obj_t *obj, uint16_t id);
void                 view_register_object_default_callback_with_number(lv_obj_t *obj, uint16_t id, uint16_t number);
uint16_t             view_get_obj_id(lv_obj_t *obj);
uint16_t             view_get_obj_number(lv_obj_t *obj);
view_gui_protocol_t *view_get_protocol(pman_handle_t handle);


extern const pman_page_t page_main, page_menu, page_programs, page_factory, page_datetime;


#endif
