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

#ifndef CAN_RECEIVE_H
#define CAN_RECEIVE_H

#include "cmsis_os.h"
#include "main.h"
#include "struct_typedef.h"

/* CAN send and receive ID */
typedef enum {
    CAN_MOTOR_ALL_ID = 0,
    CAN_ASHBIN_ID    = 1,
    CAN_BOARD_ID     = 2,

} can_msg_id_e;

typedef enum {
    MOTOR_ASHBIN_ID = 0,
    MOTOR_BOARD_ID, // 1

    MOTOR_NUM, // 电机总个数，也是电机报文数组的长度
} motor_id_e;

// rm motor data
typedef struct
{
    uint8_t func_code; // 功能码
    uint8_t data[64];  // 指令数据
} motor_measure_t;

extern motor_measure_t motor_measure[MOTOR_NUM];

extern void can_SendCmd(volatile uint8_t *cmd, uint8_t len);
extern const motor_measure_t *get_motor_measure_point(motor_id_e i);

#endif
