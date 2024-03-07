#include "bsp_led.h"
#include "main.h"

/**
 * @brief 开启 LED 输出
 *
 */
void bsp_led_init(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}

/**
 * @brief          aRGB show
 * @param[in]      aRGB: 0xaaRRGGBB, 'aa' is alpha, 'RR' is red, 'GG' is green, 'BB' is blue
 * @retval         none
 */
void bsp_led_show(uint32_t pwm)
{
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, pwm);
}
