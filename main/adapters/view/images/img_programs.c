
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
uint8_t img_programs_map[] = {

    0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x33,0x10,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x18,0xef,0xff,0xfc,0x40,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x24,0x53,0x00,0xcf,0xff,0xff,0xff,0xf8,0x01,0x45,0x30,0x00,0x00,
    0x00,0x09,0xff,0xff,0xd4,0x3f,0xff,0xff,0xff,0xff,0x64,0xff,0xfd,0x50,0x00,
    0x00,0xcf,0xff,0xff,0xff,0x54,0xff,0xff,0xff,0xff,0xf1,0xcf,0xff,0xf6,0x00,
    0x08,0xff,0xff,0xff,0xff,0xf1,0xbf,0xff,0xff,0xff,0xf7,0x7f,0xff,0xff,0x20,
    0x0e,0xff,0xff,0xff,0xff,0xf7,0x5f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x80,
    0x0f,0xff,0xff,0xff,0xff,0xfa,0x3f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x90,
    0x0f,0xff,0xff,0xff,0xff,0xfd,0x9f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x90,
    0x0e,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x80,
    0x09,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x40,
    0x01,0xef,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf9,0x00,
    0x00,0x2c,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x90,0x00,
    0x00,0x00,0x4b,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x82,0x00,0x00,
    0x00,0x00,0x04,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0x00,0x00,0x00,
    0x00,0x00,0x02,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfb,0x00,0x00,0x00,
    0x00,0x00,0x00,0xef,0xfe,0xcf,0xff,0xff,0xff,0xec,0xff,0xf9,0x00,0x00,0x00,
    0x00,0x00,0x00,0xcf,0xfc,0x3f,0xff,0xce,0xff,0xb3,0xff,0xf6,0x00,0x00,0x00,
    0x00,0x00,0x00,0x9f,0xfd,0x2f,0xff,0x49,0xff,0xa4,0xff,0xf4,0x00,0x00,0x00,
    0x00,0x00,0x00,0x7f,0xfe,0x1e,0xff,0x49,0xff,0x95,0xff,0xf2,0x00,0x00,0x00,
    0x00,0x00,0x00,0x4f,0xff,0x2d,0xff,0x49,0xff,0x75,0xff,0xd0,0x00,0x00,0x00,
    0x00,0x00,0x00,0x2f,0xff,0x2c,0xff,0x49,0xff,0x66,0xff,0xb0,0x00,0x00,0x00,
    0x00,0x00,0x00,0x0f,0xff,0x4c,0xff,0x5a,0xff,0x68,0xff,0x90,0x00,0x00,0x00,
    0x00,0x00,0x00,0x06,0x98,0x15,0x98,0x14,0x99,0x24,0x99,0x40,0x00,0x00,0x00,
    0x00,0x00,0x00,0x03,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x10,0x00,0x00,0x00,
    0x00,0x00,0x00,0x0b,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x50,0x00,0x00,0x00,
    0x00,0x00,0x00,0x0a,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x50,0x00,0x00,0x00,
    0x00,0x00,0x00,0x0b,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x50,0x00,0x00,0x00,
    0x00,0x00,0x00,0x05,0x87,0x77,0x77,0x77,0x77,0x77,0x78,0x20,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

};

const lv_image_dsc_t img_programs = {
  .header.magic = LV_IMAGE_HEADER_MAGIC,
  .header.cf = LV_COLOR_FORMAT_A4,
  .header.flags = 0,
  .header.w = 30,
  .header.h = 30,
  .header.stride = 15,
  .data_size = sizeof(img_programs_map),
  .data = img_programs_map,
};

