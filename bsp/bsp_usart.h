#ifndef BSP_USART_H
#define BSP_USART_H
#include "struct_typedef.h"

#define USART1_TX_BUF_LEN 128
#define USART2_TX_BUF_LEN 128

void usart1_printf(const char *fmt, ...);
void usart2_printf(const char *fmt, ...);

#endif
