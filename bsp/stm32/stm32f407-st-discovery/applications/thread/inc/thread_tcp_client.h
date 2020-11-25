#ifndef _THREAD_TCP_CLIENT_H_
#define _THREAD_TCP_CLIENT_H_

//..........................头文件.......................................

//#include <arpa/inet.h>         /* 包含 ip_addr_t 等地址相关的头文件 */
//#include <netdev.h>            /* 包含全部的 netdev 相关操作接口函数 */
//#include <netdb.h>
//#include <netinet/in.h>

#include <rtthread.h>
//#include <sys/socket.h> /* 使用BSD socket，需要包含sockets.h头文件 */
#include <string.h>
#include <finsh.h>
#include "fmod_data_process.h"   


#define SE_PCETH_LEN        136
#define RE_PCETH_LEN        32
//..........................函数申明.......................................

//.............................ETH通讯信息................................ 
//.....................接收PC端的ETH数据.....................

struct Re_eth_message
{   
    uint16_t  u16_life;
    uint16_t  u16_resv;
	struct   product_preset  st_product_preset;   //预设置的数据

};

union Re_eth_PC_data
{
	struct   Re_eth_message  st_data;
    uint8_t  u8_data[32];       
};
//.............................以太网发送的电池相关的数据................................ 
struct Bat_data_message   //bat_data为f类型，以太网传输时需16进制传输好实现
{   
	uint16_t     u16_life;              //生命信号
	uint16_t     u16_bat_volt;           //电池总电压*10
	uint16_t	 u16_bat_chI;			 //充电电流 *10
	uint16_t	 u16_bat_dischI;		 //放电电流 *10

	uint16_t	 u16_bat_Qc_max;		 //电池当前可充的最大电池容量	
	uint16_t	 u16_bat_Qnow;			 //电池工作后实时容量 
	uint16_t	 u16_bat_soc;			 //电池剩余容量百分比	
	uint16_t	 u16_bat_soh;			 //电池健康状态百分比
	
	
	uint16_t	 u16_batcore_max_volt;		 //电池最高单体电压mv	
	uint16_t	 u16_batcore_min_volt;		 //电池最低单体电压mv
	uint16_t	 u16_bat_max_temp;		 //电池最高温度 = T*10 + 550
	uint16_t	 u16_bat_min_temp;		 //电池最低温度 = T*10 + 550
	
	uint16_t	 u16_batcore_max_volt_index; //最高电压单体（模组）编号
	uint16_t	 u16_batcore_min_volt_index; //最低电压单体（模组）编号
	uint16_t	 u16_batcore_max_temp_index; //最高温度单体（模组）编号
	uint16_t	 u16_batcore_min_temp_index; //最低温度单体（模组）编号
	
	uint16_t	 u16_bat_Terr_index;    //单体（模组）温度传感器故障编号，与上面几个不同，每一个位对应一个故障
	uint16_t	 resv[3];
	
	
    uint16_t	 u16_Charger_V1;    //车辆充电机电压1
	uint16_t	 u16_Bat_disch_I1;    //负载1电流
	uint16_t	 u16_Bat_disch_I2;    //负载2电流
	uint16_t	 u16_Charger_V2;    //车辆充电机电压2
	
	
	uint16_t	     resv0[2];

};

union  Se_Bat_data
{
	struct  Bat_data_message  Bat_data_1;
    uint8_t u8_data[52];                            //send_mvb_packdata
};

//.....................发送的ETH数据.....................
struct Send_eth_message
{   
	
	struct    Bat_data_message    st_bat_data;             //52字节
	union     Bat_err_regs1        un_bat_err_bit1;         //2个字节
	union     Bat_err_regs2        un_bat_err_bit2;         //2个字节
	union     Bat_lock_regs       un_bat_lock_bit;       //2个字节
	union     Sys_Inout_regs       un_sys_Inout;      //1个字节
	union     Sys_status_regs    un_sys_status;  //1个字节

    struct     Batcore_data        st_batcore_data;           //48字节
	
	uint8_t    u8_uptime[6];                                //系统时间6
	
	uint16_t    u16_bat_num;                                  //电池测试数量
	uint16_t    u16_car_num;                                  //车号
	uint16_t   u16_product_num;                             //产品编号
	
	uint16_t    u16_ch_overI;                                 //充电过流值
	uint16_t    u16_disch_overI;                              //放电过流值
	uint8_t    u8_resv0[12];                                    //预留	
};

union  Se_eth_PC_data
{
	struct   Send_eth_message  st_data;
    uint8_t  u8_data[136];        //send_eth_packdata
};

extern  union   Re_eth_PC_data   un_reeth_PC_data;
extern  union   Se_eth_PC_data   un_seeth_PC_data;


#endif
