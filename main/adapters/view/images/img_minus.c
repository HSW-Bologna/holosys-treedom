
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#elif defined(LV_BUILD_TEST)
#include "../lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_DUST
#define LV_ATTRIBUTE_IMG_DUST
#endif

static const
LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_DUST
uint8_t img_minus_map[] = {

    0x00,0x78,0x87,0x77,0x77,0x77,0x90,0x00,
    0x00,0xff,0xff,0xff,0xff,0xff,0xf9,0x00,
    0x00,0xff,0xff,0xff,0xff,0xff,0xf7,0x00,
    0x00,0xff,0xff,0xff,0xff,0xff,0xf7,0x00,
    0x00,0xff,0xff,0xff,0xff,0xff,0xf7,0x00,
    0x00,0xff,0xff,0xff,0xff,0xff,0xf8,0x00,
    0x00,0xff,0xff,0xff,0xff,0xff,0xf4,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

};

const lv_image_dsc_t img_minus = {
  .header.magic = LV_IMAGE_HEADER_MAGIC,
  .header.cf = LV_COLOR_FORMAT_A4,
  .header.flags = 0,
  .header.w = 15,
  .header.h = 8,
  .header.stride = 8,
  .data_size = sizeof(img_minus_map),
  .data = img_minus_map,
};

