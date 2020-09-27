#ifndef _BSP_UART_H_
#define _BSP_UART_H_

//..........................头文件.......................................
#include  "basic_uart.h"
#include  "bsp_uart.h"

//..........................函数申明.......................................

extern void bsp_uart1_send_data(uint8_t *buffer, uint16_t len );
extern void bsp_uart4_send_data(uint8_t *buffer, uint16_t len );
extern void bsp_uart5_send_data(uint8_t *buffer, uint16_t len );
extern void bsp_uart6_send_data(uint8_t *buffer, uint16_t len );

#endif
