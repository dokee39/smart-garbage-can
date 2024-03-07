#include "led_flow_task.h"

#define LIGHT_CHANGE_TIME 300

/**
 * @brief          led RGB任务
 * @param[in]      pvParameters: NULL
 * @retval         none
 */
void led_flow_task(void const *argument)
{
    uint32_t pwm;
    fp32 light;
    uint32_t t;

    bsp_led_init();

    while (1) {
        t = xTaskGetTickCount() % (2 * LIGHT_CHANGE_TIME);
        if (t > LIGHT_CHANGE_TIME) t = 2 * LIGHT_CHANGE_TIME - t;
        light = t / (fp32)LIGHT_CHANGE_TIME;
        pwm   = 65536 * light;

        bsp_led_show(pwm);
        osDelay(1);
    }
}
