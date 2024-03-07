/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       can_receive.c/h
  * @brief      there is CAN interrupt function  to receive motor data,
  *             and CAN send function to send motor current to control motor.
  *             这里是CAN中断接收函数，接收电机数据,CAN发送函数发送电机电流控制电机.
  * @note
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. done
  *  V1.1.0     Nov-11-2019     RM              1. support hal lib
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 DJI****************************
  */

#include "CAN_Receive.h"

extern CAN_HandleTypeDef hcan;

// 电机报文数据，数组元素对应的电机见枚举 motor_id_e
motor_measure_t motor_measure[MOTOR_NUM];

// 电机发送数据的 buffer
static CAN_TxHeaderTypeDef can_tx_message;
static uint8_t can_send_data[8];

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[8];

    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data);

    // if (rx_header.StdId == CAN_MOTOR_ALL_ID) {
        uint8_t id = (uint8_t)(rx_header.ExtId >> 8);

        switch (id) {
            case CAN_ASHBIN_ID:
            case CAN_BOARD_ID: {
                volatile uint8_t i       = 0;
                volatile uint8_t len     = (uint8_t)rx_header.DLC - 1;
                volatile uint8_t packNum = (uint8_t)rx_header.ExtId;

                if (packNum == 0) {
                    motor_measure[id - 1].func_code = rx_data[0];
                    for (i = 0; i < len; i++)
                        motor_measure[id - 1].data[i] = rx_data[i + 1];
                } else {
                    if (motor_measure[id - 1].func_code == rx_data[0]) {
                        for (i = 0; i < len; i++)
                            motor_measure[id - 1].data[packNum * 7 + i] = rx_data[i + 1];
                    } else {
                        motor_measure[id - 1].data[0] = 0x6B; // 当接收到非第 0 号数据包但是发现之前的数据包没有接收到时直接向 data[0] 写入 0x6B
                    }
                }
                break;
            }
            default:
                break;
        }
    // }
}

/**
 * @brief          返回指定的电机报文数据指针
 * @param[in]      id
 * @retval         电机数据指针
 */
const motor_measure_t *get_motor_measure_point(motor_id_e id)
{
    return &motor_measure[id];
}

/**
 * @brief   CAN发送多个字节
 * @param   无
 * @retval  无
 */
void can_SendCmd(volatile uint8_t *cmd, uint8_t len)
{
    volatile uint8_t i = 0, j = 0, k = 0, l = 0, packNum = 0;
    uint32_t send_mail_box;

    // 除去ID地址和功能码后的数据长度
    j = len - 2;

    // 发送数据
    while (i < j) {
        // 数据个数
        k = j - i;

        // 填充缓存
        can_tx_message.StdId = CAN_MOTOR_ALL_ID;
        can_tx_message.ExtId = ((uint32_t)cmd[0] << 8) | (uint32_t)packNum;
        can_tx_message.IDE   = CAN_ID_EXT;
        can_tx_message.RTR   = CAN_RTR_DATA;

        can_send_data[0] = cmd[1];
        // 小于8字节命令
        if (k < 8) {
            for (l = 0; l < k; l++, i++) { can_send_data[l + 1] = cmd[i + 2]; }
            can_tx_message.DLC = k + 1;
        }
        // 大于8字节命令，分包发送，每包数据最多发送8个字节
        else {
            for (l = 0; l < 7; l++, i++) { can_send_data[l + 1] = cmd[i + 2]; }
            can_tx_message.DLC = 8;
        }

        // 发送数据
        HAL_CAN_AddTxMessage(&hcan, &can_tx_message, can_send_data, &send_mail_box);

        // 记录发送的第几包的数据
        ++packNum;
    }
}
