#ifndef _FMOD_RS485_COM_H_
#define _FMOD_RS485_COM_H_

//..........................头文件.......................................
#include "fmod_data_process.h"
#include <rtthread.h>
#include "bsp_uart.h"
#include "thread_uart_rs485.h"

//.............................................和Canopen板的UART通讯.............................................

//.............................RS485发送的通讯信息................................ 
struct Send_rs485_message   //发送给MVB的参数内容
{   
	uint8_t      u8_head;                //帧头= 0XAE
	uint8_t      u8_device_add;          //设备地址A1
	uint8_t      u8_life;                //生命信号
    uint8_t      err_bat_num;            //预留

	uint16_t     u16_bat_volt;           // *10
	uint8_t	     u8_bat_soc;			 //电池剩余容量百分比	
	uint8_t	     u8_bat_soh;			 //电池健康状态百分比
	
	uint16_t	 u16_bat_chI;			 //充电电流 *10
	uint16_t	 u16_bat_dischI;		 //放电电流 *10
	
	uint16_t	 u16_bat_max_volt;		 //电池最高单体电压mv	
	uint16_t	 u16_bat_min_volt;		 //电池最低单体电压mv
	
	uint16_t	 u16_bat_max_temp;		 //电池最高温度 = T*10 + 550
	uint16_t	 u16_bat_min_temp;		 //电池最低温度 = T*10 + 550

	uint16_t	 u16_bat_avg_volt;		 //电池平均单体电压 	1mv
	uint8_t	     u8_bat_err[2]; //电池故障状态

	uint8_t	     u8_softversion[2];      //软件版本
	uint8_t	     u8_batcore_R[52];       //单体电池内阻
    uint8_t	     u8_batcore_volt[52];    //单体电池内阻
    uint8_t      u8_end;                 //帧尾0XEF
	uint8_t      u8_check;
};

union  Se_485_data
{
	struct  Send_rs485_message  st_data;
    uint8_t u8_data[132];                        //send_rs485_packdata
};

//.............................RS485发送的通讯信息................................
struct Re_rs485_bits
{
	uint8_t time_valid:1;
	uint8_t bat_recoverch_valid:1;
	uint8_t bat_recoverch:1;
	uint8_t resv3:1;
	
	uint8_t resv4:1;
	uint8_t resv5:1;
	uint8_t resv6:1;
	uint8_t resv7:1;
};
union Re_rs485_reg
{
	struct   Re_rs485_bits  st_bit;		
	uint8_t  u8_all;
};

struct Re_rs485_message
{   
	uint8_t   u8_head;		              // = 0XAA
    uint8_t   u8_device_add;              //设备地址 = 0XA1
   
    uint8_t   u8_life;                //生命信号
	union     Re_rs485_reg  un_8b_re485;    // = 0XA1
	
	uint8_t   u8_time[6];	                 // 6B
	
	uint8_t   u8_end;                       //=0xEE	
	uint8_t   u8_check;                     //14B	
};

union Re_485_data
{
	struct  Re_rs485_message st_data;
    uint8_t u8_data[12];
};

extern  union   Se_485_data    un_se485_data;
extern  union   Re_485_data    un_re485_data;


//..........................函数申明.......................................

extern void fmod_485_com(void);
#endif

