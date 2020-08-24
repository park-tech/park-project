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


#define SE_PCETH_LEN        628
#define RE_PCETH_LEN        32
//..........................函数申明.......................................

//.............................ETH通讯信息................................ 
//.....................接收PC端的ETH数据.....................

struct Re_eth_message
{   
    uint16_t  u16_life;
    uint16_t  u16_resv;
	struct   product_preset  st_product_preset;   //20字节  预设置的数据
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
	uint16_t   u16_life;		                        //2字节
    uint16_t   u16_resv;                                    //2字节
	struct     Send_mvb_message     st_se_mvb;              //32字节
	
    struct     Batcore_data      st_batcore_data;           //504字节
	union      Batcore_err_regs  un_batcore_err;            //72字节
	
	uint8_t    u8_uptime[6];                                //系统时间6
	uint8_t    u8_resv0[2];                                 //2 预留

	uint8_t    u8_bat_num;                                  //电池测试数量
	uint8_t    u8_car_num;                                  //车号
	uint16_t   u16_product_num;                             //产品编号
	
	struct     Contactor_status_bits    st_KM_bit;          //
	uint8_t    u8_ch_overI;                                 //充电过流值
	uint8_t    u8_disch_overI;                              //放电过流值
	uint8_t    u8_resv1;                                    //预留	
};

union  Se_eth_PC_data
{
	struct   Send_eth_message  st_data;
    uint8_t  u8_data[628];        //send_eth_packdata
};

extern  union   Re_eth_PC_data   un_reeth_PC_data;
extern  union   Se_eth_PC_data   un_seeth_PC_data;


#endif
