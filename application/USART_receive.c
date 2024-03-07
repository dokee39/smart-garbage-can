/**
 * @file USART_receive.c
 * @author dokee (dokee.39@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-28
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "USART_receive.h"

#define huart_pi huart1

extern UART_HandleTypeDef huart_pi;

garbage_t pi_measure;

static uint8_t usart_rx_data[USART_RX_DATA_LEN];

void usart_ReceiveStart(void)
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart_pi, usart_rx_data, USART_RX_DATA_LEN);
    __HAL_DMA_DISABLE_IT(huart_pi.hdmarx, DMA_IT_HT);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart == &huart_pi) {
        char *pcmd_start;
        usart_rx_data[USART_RX_DATA_LEN - 1] = '\0';
        pcmd_start                           = strchr((char *)usart_rx_data, USART_CMD_START);
        if (pcmd_start != NULL) {
            if (pcmd_start[CMD_END_OFFSET] == USART_CMD_END) {
                pi_measure.type         = *(pcmd_start + TYPE_OFFSET);
                pi_measure.num = (((uint16_t)(*(pcmd_start + NUM_OFFSET)) << 8) | *(pcmd_start + NUM_OFFSET + 1));
            }
        }

        HAL_UARTEx_ReceiveToIdle_DMA(&huart_pi, usart_rx_data, USART_RX_DATA_LEN);
        __HAL_DMA_DISABLE_IT(huart_pi.hdmarx, DMA_IT_HT);
    }
}

void usart_SendState(state_e SATAE)
{
    usart1_printf("<%c>\r\n", (char)SATAE);
}
