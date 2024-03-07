#ifndef LED_TRIGGER_TASK_H
#define LED_TRIGGER_TASK_H

#include "cmsis_os.h"
#include "struct_typedef.h"
#include "bsp_led.h"

extern void led_flow_task(void const *argument);

#endif
