/**
 * @file ctrl_task.h
 * @author dokee (dokee.39@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-27
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef _CTRL_TASK_
#define _CTRL_TASK_

#include "struct_typedef.h"
#include "cmsis_os.h"
#include "main.h"

#include "Emm_V5.h"
#include "USART_receive.h"

extern void ctrl_task(void const *argument);

#endif
