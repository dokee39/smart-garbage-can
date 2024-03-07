/**
 * @file ctrl_task.c
 * @author dokee (dokee.39@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-27
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ctrl_task.h"

/*
    TODO 现有问题
    > 光电开关和碰触开关的安装
    > 顶上 putter 位置装错了
    > 开合板阻挡打印件加长
    > 底下步进电机力太小
    > 步进电机轴套购买以及重新设计打印件
    > 法兰盘安装问题 (上一个解决了这一个应该也解决了)
    > 树莓派串口问题
    > 程序上推杆的添加及根据实际情况优化
    > 画板子及供电设计
*/ 

#define ASHBIN_QUA_DISTANCE 800
#define ASHBIN_SPEED        12
#define ASHBIN_ADD          1
#define ASHBIN_COMPRESS_POS HARMFUL

#define BOARD_DISTANCE     8000
#define BOARD_SPEED         150
#define BOARD_ADD           200

typedef enum {
    ALL_READY = 0,      // 垃圾桶准备就绪
    ASHBIN_TO_ENTRANCE, // 垃圾桶转到开口处
    BOARD_OPEN,         // 打开开合板
    ASHBIN_TO_COMPRESS, // 垃圾桶转到压缩处
    COMPRESS,           // 压缩
    BOARD_CLOSE,        // kai
} run_state_e;
// TODO 开合板打开后停留 500ms

typedef enum {
    CLOSE = 0,
    OPEN  = 1,
} board_pos_e;

typedef enum {
    RETRACTED    = 0, // 缩回态
    STRTCHING    = 1, // 正在伸长
    OVER_CURRENT = 2, // 压缩阻力导致过流
    STRTCHED     = 3, // 完全伸出
    RETRACTING   = 4, // 正在缩回
} putter_pos_e;

typedef struct
{
    garbage_type_e ashnin;
    board_pos_e board;
    putter_pos_e putter;
} motor_pos_t;

typedef struct
{
    run_state_e run_state;
    garbage_t garbage;
    motor_pos_t motor_pos;
} garbage_can_t;

static void ctrl_ashbin_run(garbage_type_e target_pos);
static void ctrl_board_run(board_pos_e target_pos);
static uint8_t ctrl_is_in_place(motor_id_e MOTOR_ID);
static void ctrl_compress(void);
static uint8_t ctrl_is_compressed(void);

static garbage_can_t garbage_can;

void ctrl_task(void const *argument)
{
    osDelay(200);
    usart_ReceiveStart();
    osDelay(2000); // 上电延时 2 秒等待 Emm_V5.0 闭环初始化完毕

    // TODO 检测树莓派是否在线？
    // TODO garbage_can 结构体的初始化
    // TODO ashbin 光电开关检测零位
    // TODO board 碰触开关检测零位

    while (1) {

        switch (garbage_can.run_state) {
            case ALL_READY:
                if (pi_measure.num > garbage_can.garbage.num) {
                    garbage_can.garbage = pi_measure;
                    ctrl_ashbin_run(garbage_can.garbage.type);
                    garbage_can.run_state++;
                    Emm_V5_Read_Sys_Params((uint8_t)CAN_ASHBIN_ID, S_FLAG);
                }
                break;
            case ASHBIN_TO_ENTRANCE:
                if (ctrl_is_in_place(MOTOR_ASHBIN_ID)) {
                    ctrl_board_run(OPEN);
                    garbage_can.run_state++;
                    Emm_V5_Read_Sys_Params((uint8_t)CAN_BOARD_ID, S_FLAG);
                } else
                    Emm_V5_Read_Sys_Params((uint8_t)CAN_ASHBIN_ID, S_FLAG);
                break;
            case BOARD_OPEN:
                if (ctrl_is_in_place(MOTOR_BOARD_ID)) {
                    if (garbage_can.garbage.type == RECYCLABLE) {
                        garbage_can.run_state = ASHBIN_TO_COMPRESS;
                        ctrl_ashbin_run(ASHBIN_COMPRESS_POS);
                    } else
                        garbage_can.run_state = BOARD_CLOSE;
                    ctrl_board_run(CLOSE);
                }
                Emm_V5_Read_Sys_Params((uint8_t)CAN_BOARD_ID, S_FLAG);
                break;
            case ASHBIN_TO_COMPRESS:
                if (ctrl_is_in_place(MOTOR_ASHBIN_ID)) {
                    ctrl_compress(); // TODO
                    garbage_can.run_state++;
                } else
                    Emm_V5_Read_Sys_Params((uint8_t)CAN_ASHBIN_ID, S_FLAG);
                break;
            case COMPRESS:
                // TODO
                garbage_can.run_state++;
                break;
            case BOARD_CLOSE:
                if (ctrl_is_in_place(MOTOR_BOARD_ID))
                    garbage_can.run_state = ALL_READY;
                else
                    Emm_V5_Read_Sys_Params((uint8_t)CAN_BOARD_ID, S_FLAG);
                break;
            default:
                break;
        }

        if (xTaskGetTickCount() % 50 == 0)
        {
            if (garbage_can.run_state == ALL_READY)
                usart_SendState(READY);
            else
                usart_SendState(RUNNING);
        }

        osDelay(1);
    }
}

static void ctrl_ashbin_run(garbage_type_e target_pos)
{
    int32_t journey = (int32_t)target_pos - (int32_t)garbage_can.motor_pos.ashnin;
    if (journey == 3)
        journey = -1;
    else if (journey == -3)
        journey = 1;

    if (journey >= 0)
        Emm_V5_Pos_Control((uint8_t)CAN_ASHBIN_ID, 1, ASHBIN_SPEED, ASHBIN_ADD, ASHBIN_QUA_DISTANCE * journey, 0, 0);
    else
        Emm_V5_Pos_Control((uint8_t)CAN_ASHBIN_ID, 0, ASHBIN_SPEED, ASHBIN_ADD, -ASHBIN_QUA_DISTANCE * journey, 0, 0);

    // 检测返回命令 // 可选 错误命令判断以及避免死循环
    while (motor_measure[MOTOR_ASHBIN_ID].func_code != 0xFD);

    garbage_can.motor_pos.ashnin = target_pos;
}

static void ctrl_board_run(board_pos_e target_pos)
{
    if (garbage_can.motor_pos.board != target_pos) {
        if (target_pos == OPEN)
            Emm_V5_Pos_Control((uint8_t)CAN_BOARD_ID, 0, BOARD_SPEED, BOARD_ADD, BOARD_DISTANCE, 0, 0);
        else
            Emm_V5_Pos_Control((uint8_t)CAN_BOARD_ID, 1, BOARD_SPEED, BOARD_ADD, BOARD_DISTANCE, 0, 0);
    }

    // 检测返回命令 // 可选 错误命令判断以及避免死循环
    while (motor_measure[MOTOR_BOARD_ID].func_code != 0xFD);

    garbage_can.motor_pos.board = target_pos;
}

/**
 * @brief 到位检测
 *
 * @param MOTOR_ID
 * @return uint8_t 0 not in place; 1 in place
 */
static uint8_t ctrl_is_in_place(motor_id_e MOTOR_ID)
{
    if (motor_measure[MOTOR_ID].func_code == 0x3A)
        if (motor_measure[MOTOR_ID].data[1] == 0x6B && (motor_measure[MOTOR_ID].data[0] & 0x02) == 0x02)
            return 1;
    // 可选 清除 func_code 以及 0x6B
    return 0;
}

static void ctrl_compress(void)
{
}

static uint8_t ctrl_is_compressed(void)
{
    return 1;
}
