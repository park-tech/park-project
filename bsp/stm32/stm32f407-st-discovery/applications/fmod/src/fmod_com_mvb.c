/********************************************************************************************
头文件
********************************************************************************************/
#include "fmod_com_mvb.h"
/********************************************************************************************
变量定义
********************************************************************************************/
union   Se_mvb_data  un_semvb_data;
union   Re_mvb_data  un_remvb_data;


/********************************************************************************************
函数申明
********************************************************************************************/
static void fmod_send_mvb_com(void);
static void fmod_send_mvb_update(void);
static void fmod_re_mvb_com(void);
static void fmod_mvb_systime_set(void);


/*******************************************************************************************
** 函数名称：
** 函数描述：和CANOPEN板的UART通讯
** 输入参数：
** 返回值  ：
********************************************************************************************/
void fmod_mvb_com(void)
{
    fmod_send_mvb_com( );
	fmod_re_mvb_com( );
}
/************************************************************************************************
** 函数名称：发送给逆变器板的数据
** 函数描述：
** 输入参数：
** 返回值  ：
************************************************************************************************/
static void fmod_send_mvb_com(void)
{   
	union   Se_mvb_data  un_swap_semvb_data;

    fmod_send_mvb_update( );   

	//....................高低字节调换...........................
	un_swap_semvb_data = un_semvb_data; //
   
    endian_swaps(&un_swap_semvb_data.st_se_mvb.u16_bat_volt);
	endian_swaps(&un_swap_semvb_data.st_se_mvb.u16_bat_soc);
    endian_swaps(&un_swap_semvb_data.st_se_mvb.u16_bat_soh);
    endian_swaps(&un_swap_semvb_data.st_se_mvb.u16_bat_chI);
	endian_swaps(&un_swap_semvb_data.st_se_mvb.u16_bat_dischI);
    endian_swaps(&un_swap_semvb_data.st_se_mvb.u16_bat_max_volt);
    endian_swaps(&un_swap_semvb_data.st_se_mvb.u16_bat_min_volt);
	endian_swaps(&un_swap_semvb_data.st_se_mvb.u16_bat_max_temp);
    endian_swaps(&un_swap_semvb_data.st_se_mvb.u16_bat_min_temp);
    endian_swaps(&un_swap_semvb_data.st_se_mvb.u16_bat_max_R);
	endian_swaps(&un_swap_semvb_data.st_se_mvb.u16_bat_min_R);

    //.........................发送消息队列.......................................
	if (rt_mq_send(&uart1_se_mq, &un_swap_semvb_data.u8_data, sizeof(un_swap_semvb_data.u8_data)) == -RT_ERROR)
	{
		rt_kprintf("uart1_se_mq send err!\n");
	}
}

/*********************************************************************************************************
** 函数名称：发送给mvb板的数据更新
** 函数描述：
** 输入参数：
** 返回值  ：
*********************************************************************************************************///
static void fmod_send_mvb_update(void)
{   
	unsigned char i = 0;
	static uint16_t mvb_life = 0 ;

	if(mvb_life < 255 ) mvb_life++;
	else mvb_life = 0;
	
    un_semvb_data.st_se_mvb.u8_life = mvb_life;
	 un_semvb_data.st_se_mvb.u8_err_bat_num = st_bat_data.u16_err_bat_num & 0X00FF;
    un_semvb_data.st_se_mvb.u16_bat_volt = st_bat_data.fl_bat_volt * 10;
    un_semvb_data.st_se_mvb.u16_bat_soc = st_bat_data.fl_bat_soc;
    un_semvb_data.st_se_mvb.u16_bat_soh = st_bat_data.fl_bat_soh;
    un_semvb_data.st_se_mvb.u16_bat_chI = st_bat_data.fl_bat_chI * 10;
    un_semvb_data.st_se_mvb.u16_bat_dischI = st_bat_data.fl_bat_dischI * 10;
    un_semvb_data.st_se_mvb.u16_bat_max_volt = st_bat_data. u16_bat_max_volt;
    un_semvb_data.st_se_mvb.u16_bat_min_volt = st_bat_data.u16_bat_min_volt;
    un_semvb_data.st_se_mvb.u16_bat_max_temp = (st_bat_data.fl_bat_max_temp + 55) *10;
    un_semvb_data.st_se_mvb.u16_bat_min_temp = (st_bat_data.fl_bat_min_temp + 55) *10;
    un_semvb_data.st_se_mvb.u16_bat_max_R = st_bat_data.u16_bat_max_R;
    un_semvb_data.st_se_mvb.u16_bat_min_R = st_bat_data.u16_bat_min_R;
    un_semvb_data.st_se_mvb.u16_bat_avg_volt = st_bat_data.u16_bat_avg_volt;
    un_semvb_data.st_se_mvb.u16_bat_avg_R = st_bat_data.u16_bat_avg_R;
	un_semvb_data.st_se_mvb.u8_bat_err[0] = un_bat_err.u16_all & 0X00FF ;
	un_semvb_data.st_se_mvb.u8_bat_err[1] =((un_bat_err.u16_all & 0XFF00) >> 8);
	un_semvb_data.st_se_mvb.u8_softversion[0] = 0x01;	 //V1.0
	un_semvb_data.st_se_mvb.u8_softversion[1] = 0x00; 
}

/*********************************************************************************************************
** 函数名称：接收CANOPEN板的数据更新
** 函数描述：
** 输入参数：
** 返回值  ：
*********************************************************************************************************///
static void fmod_re_mvb_com(void)
{
    uint8_t mq_re_buf[UART1_RE_LEN] = {0};
    uint8_t i = 0;
    union   Re_mvb_data  un_swap_remvb_data;
	
	if ( rt_mq_recv(&uart1_rx_mq, mq_re_buf, sizeof(mq_re_buf), 5) == RT_EOK )   //超时时间5ms 单位为ms
	{
		for(i = 0; i < UART1_RE_LEN; i++)
		{
			un_swap_remvb_data.u8_data[i] = mq_re_buf[i];
		}
	}

    endian_swaps(&un_swap_remvb_data.st_re_mvb.u16_life);

    un_remvb_data = un_swap_remvb_data;

    //.........................判断是否进入时间设定....................
	if(un_remvb_data.st_re_mvb.un_16b_remvb.st_bit.time_set ==1) //时间设定
	{
		fmod_mvb_systime_set( );
	}
}

/**********************************************************************************
** 函数名称：
** 函数描述：时间设置
** 输入参数：无
** 返回值  ：
************************************************************************************/
static void fmod_mvb_systime_set(void)
{   
	struct   tm  now_tm;
	
	get_rtc_time(&now_tm);
	
	if( (un_bat_err.st_bit.rs485_com_err == 0) &&     //通讯正常
	(un_remvb_data.st_re_mvb.un_16b_remvb.st_bit.mvb_control == 1) && //MVB控车
	(un_remvb_data.st_re_mvb.un_16b_remvb.st_bit.time_valid ==1) )    //时间有效
	{
		if(  (now_tm.tm_year != un_remvb_data.st_re_mvb.u8_time[0] + 100) || 
			(now_tm.tm_mon  != un_remvb_data.st_re_mvb.u8_time[1] - 1) || 
			(now_tm.tm_mday != un_remvb_data.st_re_mvb.u8_time[2])    || 
			(now_tm.tm_hour != un_remvb_data.st_re_mvb.u8_time[3]) || 
			(now_tm.tm_min  != un_remvb_data.st_re_mvb.u8_time[4]) || 
			(abs(now_tm.tm_sec - un_remvb_data.st_re_mvb.u8_time[5]) >= 10) ) //相差10秒
		{
			now_tm.tm_year = un_remvb_data.st_re_mvb.u8_time[0] + 100;
			now_tm.tm_mon  = un_remvb_data.st_re_mvb.u8_time[1] - 1;
			now_tm.tm_mday = un_remvb_data.st_re_mvb.u8_time[2];
			now_tm.tm_hour = un_remvb_data.st_re_mvb.u8_time[3];
			now_tm.tm_min  = un_remvb_data.st_re_mvb.u8_time[4];
			now_tm.tm_sec = un_remvb_data.st_re_mvb.u8_time[5];
			set_rtc_time(now_tm);
		}
	}
}

