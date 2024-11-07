
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
uint8_t img_home_map[] = {

    0x00,0x00,0x00,0x00,0x00,0x01,0xab,0x40,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x5f,0xff,0xf9,0x02,0xff,0xe0,0x00,0x00,
    0x00,0x00,0x00,0x00,0x0a,0xff,0xff,0xff,0xb9,0xff,0xf3,0x00,0x00,
    0x00,0x00,0x00,0x01,0xdf,0xff,0xff,0xff,0xff,0xff,0xf2,0x00,0x00,
    0x00,0x00,0x00,0x6f,0xff,0xff,0xa9,0xff,0xff,0xff,0xf2,0x00,0x00,
    0x00,0x00,0x0a,0xff,0xff,0xe1,0x00,0x0c,0xff,0xff,0xf4,0x00,0x00,
    0x00,0x02,0xef,0xff,0xfb,0x00,0xbc,0x30,0x7f,0xff,0xff,0x50,0x00,
    0x00,0x6f,0xff,0xff,0x60,0x4f,0xff,0xf8,0x03,0xff,0xff,0xfa,0x00,
    0x0b,0xff,0xff,0xf1,0x09,0xff,0xff,0xff,0xc0,0x0c,0xff,0xff,0xd1,
    0xaf,0xff,0xfb,0x00,0xdf,0xff,0xff,0xff,0xff,0x30,0x8f,0xff,0xfe,
    0xaf,0xff,0x70,0x4f,0xff,0xff,0xff,0xff,0xff,0xf8,0x03,0xff,0xfe,
    0x0d,0xe2,0x09,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xc0,0x0c,0xf3,
    0x00,0x00,0xdf,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x20,0x00,
    0x00,0x0d,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0x00,
    0x00,0x0f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf4,0x00,
    0x00,0x0f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf4,0x00,
    0x00,0x0f,0xff,0xff,0xff,0xa3,0x44,0x38,0xff,0xff,0xff,0xf4,0x00,
    0x00,0x0f,0xff,0xff,0xff,0x00,0x00,0x00,0xef,0xff,0xff,0xf4,0x00,
    0x00,0x0f,0xff,0xff,0xff,0x10,0x00,0x00,0xdf,0xff,0xff,0xf4,0x00,
    0x00,0x0f,0xff,0xff,0xff,0x10,0x00,0x00,0xef,0xff,0xff,0xf4,0x00,
    0x00,0x0f,0xff,0xff,0xff,0x00,0x00,0x00,0xef,0xff,0xff,0xf4,0x00,
    0x00,0x0f,0xff,0xff,0xff,0x00,0x00,0x00,0xef,0xff,0xff,0xf4,0x00,
    0x00,0x0f,0xff,0xff,0xff,0x00,0x00,0x00,0xdf,0xff,0xff,0xf4,0x00,
    0x00,0x0d,0xff,0xff,0xff,0x10,0x00,0x00,0xef,0xff,0xff,0xf0,0x00,
    0x00,0x00,0xcf,0xee,0xee,0x00,0x00,0x00,0xdf,0xee,0xfd,0x30,0x00,

};

const lv_image_dsc_t img_home = {
  .header.magic = LV_IMAGE_HEADER_MAGIC,
  .header.cf = LV_COLOR_FORMAT_A4,
  .header.flags = 0,
  .header.w = 26,
  .header.h = 25,
  .header.stride = 13,
  .data_size = sizeof(img_home_map),
  .data = img_home_map,
};

