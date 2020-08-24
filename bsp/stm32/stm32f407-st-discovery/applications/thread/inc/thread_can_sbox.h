#ifndef _THREAD_CAN_SBOX_H_
#define _THREAD_CAN_SBOX_H_
//..........................头文件.......................................

#include <rtthread.h>
#include "rtdevice.h"
#include "bsp_can.h"
#include "basic_can.h"
#include "rtdbg.h"
#include "fmod_data_process.h"


#define  CAN1_BAUD_RATE     CAN250kBaud
#define  CAN1_RX_LEN   (SUM_BAT_NUM_MAX * 8)
#define  CAN1_SE_LEN   8
#define  CAN_SEND_SBOX_ID  0x0501 
           
extern struct rt_messagequeue can1_rx_mq;      //发送消息队列
extern struct rt_messagequeue can1_se_mq;       //CAN发送端消息队列
//..........................函数申明.......................................

#endif
