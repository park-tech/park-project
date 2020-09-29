/********************************************************************************************
头文件
********************************************************************************************/
#include "fmod_com_rs485.h"
/********************************************************************************************
变量定义
********************************************************************************************/
union   Se_485_data  un_se485_data;
union   Re_485_data  un_re485_data;


/********************************************************************************************
函数申明
********************************************************************************************/
uint8_t BCC_XOR_check(uint8_t *data,  uint8_t len);
static void fmod_send_485_com(void);
static void fmod_send_485_update(void);
static void fmod_re_485_com(void);
static void fmod_485_systime_set(void);


/*******************************************************************************************
** 函数名称：
** 函数描述：和CANOPEN板的UART通讯
** 输入参数：
** 返回值  ：
********************************************************************************************/
void fmod_485_com(void)
{
    fmod_send_485_com( );
	fmod_re_485_com( );
}
/************************************************************************************************
** 函数名称：发送给逆变器板的数据
** 函数描述：
** 输入参数：
** 返回值  ：
************************************************************************************************/
static void fmod_send_485_com(void)
{   
	union   Se_485_data  un_swap_se485_data;

    fmod_send_485_update( );   

	//....................高低字节调换...........................
	un_swap_se485_data = un_se485_data; //
    endian_swaps(&un_swap_se485_data.st_data.u16_bat_volt);
    endian_swaps(&un_swap_se485_data.st_data.u16_bat_chI);
	endian_swaps(&un_swap_se485_data.st_data.u16_bat_dischI);
    endian_swaps(&un_swap_se485_data.st_data.u16_bat_max_volt);
    endian_swaps(&un_swap_se485_data.st_data.u16_bat_min_volt);
	endian_swaps(&un_swap_se485_data.st_data.u16_bat_max_temp);
    endian_swaps(&un_swap_se485_data.st_data.u16_bat_min_temp);
    endian_swaps(&un_swap_se485_data.st_data.u16_bat_avg_volt);

    //.........................发送消息队列.......................................
	if (rt_mq_send(&uart5_se_mq, &un_swap_se485_data.u8_data, sizeof(un_swap_se485_data.u8_data)) == -RT_ERROR)
	{
		rt_kprintf("uart5_se_mq send err!\n");
	}
}

/*********************************************************************************************************
** 函数名称：发送给485板的数据更新
** 函数描述：
** 输入参数：
** 返回值  ：
*********************************************************************************************************///
static void fmod_send_485_update(void)
{   
	unsigned char i = 0;
	static uint8_t u8_485_send_life = 0 ;

    un_se485_data.st_data.u8_head = 0XAE;
	un_se485_data.st_data.u8_device_add = 0XA1;

	if(u8_485_send_life < 255 ) u8_485_send_life++;
	else u8_485_send_life = 0;
	un_se485_data.st_data.u8_life = u8_485_send_life;
    un_se485_data.st_data.err_bat_num = 0X00;

	un_se485_data.st_data.u16_bat_volt = st_bat_data.fl_bat_volt * 10;
	un_se485_data.st_data.u8_bat_soc = st_bat_data.fl_bat_soc; 
	un_se485_data.st_data.u8_bat_soh= st_bat_data.fl_bat_soh;
	un_se485_data.st_data.u16_bat_chI = st_bat_data.fl_bat_chI * 10;
	un_se485_data.st_data.u16_bat_dischI = st_bat_data.fl_bat_dischI * 10;
	un_se485_data.st_data.u16_bat_max_volt = st_bat_data.u16_bat_max_volt;
	un_se485_data.st_data.u16_bat_min_volt = st_bat_data.u16_bat_min_volt;
	un_se485_data.st_data.u16_bat_max_temp = (st_bat_data.fl_bat_max_temp + 55) *10;
    un_se485_data.st_data.u16_bat_min_temp = (st_bat_data.fl_bat_min_temp + 55) *10;

	un_se485_data.st_data.u16_bat_avg_volt = st_bat_data.u16_bat_avg_volt;
	un_se485_data.st_data.u8_bat_err[0] = un_bat_err.u16_all & 0X00FF ;
	un_se485_data.st_data.u8_bat_err[1] = ((un_bat_err.u16_all & 0XFF00) >> 8);
	un_se485_data.st_data.u8_softversion[0] = 0x01;	                                 //V1.0
	un_se485_data.st_data.u8_softversion[1] = 0x00; 

//    for(i = 0 ; i < 52 ; i++)
//    {
//		//...................单体电池内阻...............................
//		if(st_batcore_data.u16_batcore_R[i] > 255)  un_se485_data.st_data.u8_batcore_R[i] = 0XFF;
//		else 	un_se485_data.st_data.u8_batcore_R[i] = st_batcore_data.u16_batcore_R[i] ;	
//  		
//		//...................单体电池电压...............................
//  		un_se485_data.st_data.u8_batcore_volt[i] = (st_batcore_data.u16_batcore_volt[i] / 20);
//    }

    un_se485_data.st_data.u8_end = 0XEF;
    un_se485_data.st_data.u8_check = BCC_XOR_check(un_se485_data.u8_data, 131);//需要计算的长度
    
}

/*********************************************************************************************************
** 函数名称：接收CANOPEN板的数据更新
** 函数描述：
** 输入参数：
** 返回值  ：
*********************************************************************************************************///
static void fmod_re_485_com(void)
{
    uint8_t mq_re_buf[UART5_RE_LEN] = {0};
    uint8_t i = 0;
  
	
	if ( rt_mq_recv(&uart5_rx_mq, mq_re_buf, sizeof(mq_re_buf), 5) == RT_EOK )   //超时时间5ms 单位为ms
	{
		for(i = 0; i < UART5_RE_LEN; i++)
		{
			un_re485_data.u8_data[i] = mq_re_buf[i];
		}
	}
    //.........................判断是否进入时间设定....................
	if(un_re485_data.st_data.un_8b_re485.st_bit.time_valid ==1) //时间有效
	{
		fmod_485_systime_set( );
	}
}

/**********************************************************************************
** 函数名称：
** 函数描述：时间设置
** 输入参数：无
** 返回值  ：
************************************************************************************/
static void fmod_485_systime_set(void)
{   
	struct   tm  now_tm;

	get_rtc_time(&now_tm);
	if( (un_re485_data.st_data.un_8b_re485.st_bit.time_valid == 1) )    //时间有效
	{
		if(  (now_tm.tm_year != un_re485_data.st_data.u8_time[0] + 100) || 
			(now_tm.tm_mon  != un_re485_data.st_data.u8_time[1] - 1) || 
			(now_tm.tm_mday != un_re485_data.st_data.u8_time[2])    || 
			(now_tm.tm_hour != un_re485_data.st_data.u8_time[3]) || 
			(now_tm.tm_min  != un_re485_data.st_data.u8_time[4]) || 
			(abs(now_tm.tm_sec - un_re485_data.st_data.u8_time[5]) >= 10) ) //相差10秒
		{
			now_tm.tm_year = un_re485_data.st_data.u8_time[0] + 100;
			now_tm.tm_mon  = un_re485_data.st_data.u8_time[1] - 1;
			now_tm.tm_mday = un_re485_data.st_data.u8_time[2];
			now_tm.tm_hour = un_re485_data.st_data.u8_time[3];
			now_tm.tm_min  = un_re485_data.st_data.u8_time[4];
			now_tm.tm_sec = un_re485_data.st_data.u8_time[5];
			set_rtc_time(now_tm);
		}
	}
}

