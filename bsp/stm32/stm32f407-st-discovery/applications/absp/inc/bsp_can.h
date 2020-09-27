#ifndef _BSP_CAN_H_
#define _BSP_CAN_H_

//..........................头文件.......................................
#include "basic_can.h"


#define  DEV_CAN1   1
#define  DEV_CAN2   2

#define  CAN1_SEND_ID   0x0501
#define  CAN1_IDE       RT_CAN_STDID  //标准帧: RT_CAN_STDID ;扩展帧：RT_CAN_EXTID 



void bsp_can_send_sbox_data( uint32_t can_id, uint8_t len, uint8_t *buf );

#endif
