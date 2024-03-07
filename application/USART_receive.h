/**
 * @file USART_receive.h
 * @author dokee (dokee.39@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-28
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef _USART_RECEIVE_H_
#define _USART_RECEIVE_H_

#include "cmsis_os.h"
#include "main.h"
#include "struct_typedef.h"

#include <string.h>
#include "bsp_usart.h"

#define USART_RX_DATA_LEN 8
#define USART_CMD_START   '<'
#define USART_CMD_END     '>'

#define TYPE_OFFSET       1
#define NUM_OFFSET        2
#define CMD_END_OFFSET    4

typedef enum {
    RECYCLABLE = 0, // 可回收垃圾
    KITCHEN    = 1, // 厨余垃圾
    OTHER      = 2, // 其他垃圾
    HARMFUL    = 3, // 有害垃圾
} garbage_type_e;

typedef enum {
    RUNNING = 0, // 垃圾桶正在运行未完成垃圾处理
    READY   = 1, // 垃圾桶完成垃圾处理时并准备好下一次的运行
} state_e;

typedef struct
{
    garbage_type_e type; // 垃圾类型, 0x01~0x04
    uint16_t num;        // 垃圾计数, 0~255
} garbage_t;

extern garbage_t pi_measure;

void usart_ReceiveStart(void);
void usart_SendState(state_e SATAE);

#endif
