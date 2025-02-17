#ifndef HARDWAREPROFILE_H_INCLUDED
#define HARDWAREPROFILE_H_INCLUDED


#include <driver/gpio.h>


/*
 * Definizioni dei pin da utilizzare
 */

#define BSP_HAP_UC_SDA  GPIO_NUM_1
#define BSP_HAP_UC_SCL  GPIO_NUM_2
#define BSP_HAP_TUC_RST GPIO_NUM_4
#define BSP_HAP_DIS_CS  GPIO_NUM_5
#define BSP_HAP_DIS_RS  GPIO_NUM_6
#define BSP_HAP_DIS_SCL GPIO_NUM_7
#define BSP_HAP_DIS_SDI GPIO_NUM_8
#define BSP_HAP_DIS_RST GPIO_NUM_9
#define BSP_HAP_BKL_PWM GPIO_NUM_10
#define BSP_HAP_BUZ     GPIO_NUM_11
#define BSP_HAP_SDA_CLK GPIO_NUM_40
#define BSP_HAP_SCL_CLK GPIO_NUM_41
#define BSP_HAP_IN1     GPIO_NUM_16
#define BSP_HAP_IN2     GPIO_NUM_17
#define BSP_HAP_IN3     GPIO_NUM_18
#define BSP_HAP_IN4     GPIO_NUM_21
#define BSP_HAP_OUT_OI  GPIO_NUM_33
#define BSP_HAP_PWM_OI  GPIO_NUM_34


#endif
