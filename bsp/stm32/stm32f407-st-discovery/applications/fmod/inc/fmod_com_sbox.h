#ifndef _FMOD_COM_SBOX_H_
#define _FMOD_COM_SBOX_H_

//..........................头文件.......................................
#include <rtthread.h>
#include "fmod_data_process.h"
#include "thread_can_sbox.h"

//.............................采样盒通讯信息................................ 
struct Can_send_sbox
{   
	uint16_t	 u16_bat_chI;			 //电池充电电流         0.1A
	uint16_t	 u16_bat_dischI;		 //放电电流
	uint16_t	 u16_batcore_ageV;		 //单体平均电压  mv
	uint16_t     u16_resv;
};

union  Can_se_sbox_data
{   
	struct   Can_send_sbox   st_send_sbox;
	uint8_t	 u8_data[8];		        //send_sbox_data
};


struct Can_re_sbox
{   
	uint16_t	 u16_batcore_volt;		 //单体电池电压 1 mv
	uint16_t	 u16_batcore_temp;		 //(T+55)*10 
	uint16_t	 u16_batcore_R;		     //单体电池内阻 0.1 m欧
	uint16_t     u16_resv;
};

union  Can_re_sbox_data
{   
	struct   Can_re_sbox   st_re_sbox;
	uint8_t	 u8_data[9];	               	
};



extern void fmod_sbox_com(void);

#endif



/******************************************************************************************
    end file
*******************************************************************************************/
