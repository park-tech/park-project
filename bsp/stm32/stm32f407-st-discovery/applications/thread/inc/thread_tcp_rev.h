#ifndef _THREAD_TCP_REV_H_
#define _THREAD_TCP_REV_H_

//..........................头文件.......................................



#include <rtthread.h>
//#include <sys/socket.h> /* 使用BSD socket，需要包含sockets.h头文件 */
#include <string.h>
#include <finsh.h>
#include "fmod_data_process.h"   

/******************************
		电池组总电压
		充电电流
		放电电流
		箱体温度
		电池当前可充电最大容量
		电池当前容量
		SOC电池剩余容量
		SOH电池健康状态
		电池单体(模组)最高电压
		电池单体（模组）最低电压
		电池单体（模组）最高温度
		电池单体（模组）最低温度
		最高电压单体（模组）编号
		最低电压单体（模组）编号
		最高温度单体（模组）编号
		最低温度单体（模组）编号
		单体（模组）温度传感器故障编号
		单体（模组）内阻过高编号
		电压采样线断开编号
		预留
		预留
		电池组过充
		电池组过放
		电池组充电过流
		电池放电过流
		电池组S0C低压预警
		箱体过温
		预留
		预留
		单体（模组）电池过充
		单体（模组）电池过放
		单体（模组）电池过温故障
		单体（模组）电压采样线断开故障
		温度传感器故障
		单体（模组）电池压差故障
		温差超限故障
		单体（模组）内阻过高
		自检故障
		内部通讯故障
		MVB通讯故障
		ETH通讯故障
		电流传感器故障
		预留
		预留
		预留
		预留
		预留
		预留
		预留
		预留
		预留
		预留
		总故障指示
		单体（模组）电池电压(1-52)
		单体（模组）电池温度(1-52)
		单体（模组）电池内阻(1-52)
		测试电池的数量
		车号
		产品编号
		充电过流值
		放电过流值
		年
		月
		日
		时
		分
		秒
***********************/



#define SE_REVETH_LEN       38   //===发送的字节数(临时)
#define RE_PCETH_LEN        32
//................................函数申明.......................................

//...............................ETH通讯信息................................ 
//.............................接收PC端的ETH数据..........................

struct Re_eth_message
{   
	uint16_t  u16_life;
	uint16_t  u16_resv;
	uint8_t  u8_data[8];

};


union Re_eth_PC_data
{
	struct   Re_eth_message  st_data;
  uint8_t  u8_data[32];       
};


//.....................发送的ETH数据.....................
struct Send_eth_message
{   
	uint16_t   u16_product_num;       //产品编号
  uint8_t    u8_softverdtion[2];    //===软件版本号

	
};


union  Se_eth_PC_data
{
	struct   Send_eth_message  st_data;
  uint8_t  u8_data[632];        //send_eth_packdata
};


extern  union   Re_eth_PC_data   un_reeth_PC_data;
extern  union   Se_eth_PC_data   un_seeth_PC_data;


#endif
