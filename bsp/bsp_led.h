#ifndef BSP_LED_H
#define BSP_LED_H

#include "struct_typedef.h"
#include "tim.h"

extern void bsp_led_init(void);
extern void bsp_led_show(uint32_t pwm);

#endif
