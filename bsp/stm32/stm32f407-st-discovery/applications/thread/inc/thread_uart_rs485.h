#ifndef _THREAD_UART_RS485_H_
#define _THREAD_UART_RS485_H_

//..........................头文件.......................................
#include <rtthread.h>
#include "basic_uart.h"
#include "bsp_uart.h"


#define  UART5_RE_LEN        12   //UART1的接收数据长度
#define  UART5_SE_LEN        132  //UART1的接收数据长度

extern struct rt_messagequeue uart5_rx_mq;      //发送消息队列
extern struct rt_messagequeue uart5_se_mq;      //发送消息队列
//..........................函数申明.......................................


#endif
