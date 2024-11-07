#ifndef BSP_INVERTER_H_INCLUDED
#define BSP_INVERTER_H_INCLUDED


#include <stdint.h>


void bsp_inverter_init(void);
void bsp_inverter_set_speed(uint8_t speed_percentage);


#endif
