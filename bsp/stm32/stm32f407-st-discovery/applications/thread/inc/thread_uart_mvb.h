#ifndef _THREAD_UART_MVB_H_
#define _THREAD_UART_MVB_H_
//..........................头文件.......................................
//..........................头文件.......................................
#include <rtthread.h>
#include "basic_uart.h"
#include "basic_uart.h"
#include "bsp_uart.h"

#define  MVB_DEVICE_ADDR        0x001B    //mvb设备地址
#define  MVB_CYCLE_TIME         31        //MVB刷新周期为：（31+1）*16=512ms

#define  MVB_SPORT_NUM          1         //MVB通讯 设备的源端口数量（设备的端口）
#define  MVB_SPORT1_ADDR        0x01B0    //源端口1地址
#define  MVB_SPORT1_LEN         0x04      //源端口数据长度 00:2BYTE,01:4BYTE,02:8BYTE,03:16BYTE,04:32BYTE

#define  MVB_DPORT_NUM          1         //MVB通讯 设备的宿端口数量（TCMS的端口）  
#define  MVB_DPORT1_ADDR        0x0001    //宿端口地址1高字节
#define  MVB_DPORT1_LEN         0x04      //源端口数据长度 00:2BYTE,01:4BYTE,02:8BYTE,03:16BYTE,04:32BYTE,


#define  UART1_RE_LEN        (9 + MVB_DPORT1_LEN)  //UART1的接收数据长度
#define  UART1_SE_LEN        MVB_SPORT1_LEN        //UART1的接收数据长度

extern struct rt_messagequeue uart1_rx_mq;      //发送消息队列
extern struct rt_messagequeue uart1_se_mq;      //发送消息队列
//..........................函数申明.......................................



#endif
