#ifndef _FMOD_MVB_COM_H_
#define _FMOD_MVB_COM_H_

//..........................头文件.......................................
#include <rtthread.h>
#include "fmod_data_process.h"
#include "bsp_uart.h"
#include "thread_uart_mvb.h"

//.............................................和Canopen板的UART通讯.............................................

//.............................MVB通讯信息................................ 
// struct Send_mvb_message   //发送给MVB的参数内容
// {   
// 	uint16_t	 u16_life;		         //生命信号
// 	uint16_t     u16_bat_volt;           // *10
// 	uint16_t	 u16_bat_soc;			 //电池剩余容量百分比	
// 	uint16_t	 u16_bat_soh;			 //电池健康状态百分比
	
// 	uint16_t	 u16_bat_chI;			 //充电电流 *10
// 	uint16_t	 u16_bat_dischI;		 //放电电流 *10
// 	uint16_t	 u16_bat_max_volt;		 //电池最高单体电压mv	
// 	uint16_t	 u16_bat_min_volt;		 //电池最低单体电压mv
	
// 	uint16_t	 u16_bat_max_temp;		 //电池最高温度 = T*10 + 550
// 	uint16_t	 u16_bat_min_temp;		 //电池最低温度 = T*10 + 550
// 	uint16_t	 u16_bat_max_R;		     //电池最高内阻 1 = 0.1mΩ
// 	uint16_t	 u16_bat_min_R;		     //电池最低内阻
	
// 	uint8_t      u8_bat_err[2];          //故障状态
// 	uint8_t	     u8_softversion[2];
// 	uint8_t      u8_resv[4];
// };

// union  Send_mvb_data
// {
// 	struct  Send_mvb_message  st_se_mvb;
//     uint8_t u8_data[32];                            //send_mvb_packdata
// };

// struct Re_mvb_bits
// {
// 	uint16_t time_set:1;
// 	uint16_t time_valid:1;
// 	uint16_t resv2:1;
// 	uint16_t resv3:1;
	
// 	uint16_t resv4:1;
// 	uint16_t resv5:1;
// 	uint16_t resv6:1;
// 	uint16_t resv7:1;
	
// 	uint16_t resv8:1;
// 	uint16_t resv9:1;
// 	uint16_t resv10:1;
// 	uint16_t resv11:1;
	
// 	uint16_t mvb_control:1;
// 	uint16_t resv13:1;
// 	uint16_t resv14:1;
// 	uint16_t resv15:1;

// };
// union Re_mvb_reg
// {
// 	struct    Re_mvb_bits  st_bit;		
// 	uint16_t  u16_all;
// };

// struct Re_mvb_message
// {   
// 	uint16_t  u16_life;		                 //2B
// 	union     Re_mvb_reg  un_16b_remvb; //2B
// 	uint8_t   u8_time[6];	                 //6B
// 	uint8_t   u8_data[22];                   //14B	
// };

// union Re_mvb_data
// {
// 	struct  Re_mvb_message st_re_mvb;
//     uint8_t u8_data[32];
// };



extern  union   Se_mvb_data  un_semvb_data;
extern  union   Re_mvb_data  un_remvb_data;
//..........................函数申明.......................................

extern void fmod_mvb_com(void);
#endif

