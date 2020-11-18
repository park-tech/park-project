/********************************************************************************************
头文件
********************************************************************************************/
#include "fmod_fault_detect.h"

/********************************************************************************************
宏定义
********************************************************************************************/
/********************************************************************************************
变量定义
********************************************************************************************/
union	Bat_err_regs1      un_bat_err1;
union	Bat_err_regs2      un_bat_err2;
union	Batcore_err_regs  un_batcore_err;
union   Bat_lock_regs     un_bat_lock;
extern union	Contactor_status_regs   un_KM_bit;

/********************************************************************************************
函数申明
********************************************************************************************/

static void fmod_fault_bat_overV(void);			
static void fmod_fault_bat_underV(void);
static void fmod_fault_bat_underV_warn(void);
static void fmod_fault_bat_over_chI(void);
static void fmod_fault_bat_overdischI(void);
static void fmod_fault_bat_overT(void);
static void fmod_fault_bat_underT(void);
static void fmod_fault_sensor_T(void);
static void fmod_fault_batcore_overV (void);
static void fmod_fault_batcore_underV (void);

static void fmod_fault_CAN_com(void);
static void fmod_fault_relay(void);

/******************************************************************************************
函数描述：判断故障
输入参数：无
返回值：      无
*******************************************************************************************/
void fmod_fault_detect(void)
{   
 //........................接触器故障................................
    fmod_fault_relay();

    //........................电池组过充................................
    fmod_fault_bat_overV();

    //........................电池组过放................................
	fmod_fault_bat_underV();

	//........................电池组充电过流................................
	fmod_fault_bat_over_chI();
	
	//........................电池组放电过流................................
	fmod_fault_bat_overdischI();

	//........................电池过温................................
	fmod_fault_bat_overT();

	//........................电池低温................................
	fmod_fault_bat_underT();
	
	//........................电池温度传感器故障................................
	fmod_fault_sensor_T();

	//........................单体电池过压................................
	fmod_fault_batcore_overV ();

	//........................单体电池欠压................................
	fmod_fault_batcore_underV ();

	//........................MVB通讯故障.....................................
	fmod_fault_CAN_com();
}



/******************************************************************************************
** 函数名称：电池过充       
** 函数描述：
** 输入参数：无
** 返回值  ：无
*******************************************************************************************/
static void  fmod_fault_bat_overV (void)
{
	static uint16_t  u16_err_count = 0;


	if(st_bat_data.fl_bat_volt >= (BC_OVER_V *10* TEST_BAT_NUM ))		 //过压值为14V
	{   
		if(u16_err_count <= 10) //1秒
		{
			u16_err_count++; 
		}
		else
		{
			un_bat_err1.st_bat_err_bit1.bat_overV = 1;
			u16_err_count = 10;              //1秒
		}
	}

	if(st_bat_data.fl_bat_volt <= (BC_OVER_V - 0.48) *10* TEST_BAT_NUM )	//回差设置	 //过压值为14V
	{   
		
		if(u16_err_count > 0 )
		{
			u16_err_count--; 	
		}
		else
		{
			un_bat_err1.st_bat_err_bit1.bat_overV = 0;
		}	
	}

}

/******************************************************************************************
** 函数名称：电池过放       
** 函数描述：
** 输入参数：无
** 返回值  ：无
*******************************************************************************************/
static void  fmod_fault_bat_underV (void)
{
	static uint16_t  u16_err_count = 0;
	
	if(st_bat_data.fl_bat_volt < (BC_UNDER_V *10* TEST_BAT_NUM  ) )  
	{		
		if(u16_err_count <= 10)  //1秒
		{
			u16_err_count++; 
		}
		else
		{
			un_bat_err1.st_bat_err_bit1.bat_underV = 1;
			u16_err_count = 10;              
		}
	}

	if(st_bat_data.fl_bat_volt >= ((BC_UNDER_V + 0.09 ) *10* TEST_BAT_NUM ) ) 	  
	{	
		if(u16_err_count > 0 )
		{
			u16_err_count--; 	
		}
		else
		{
			un_bat_err1.st_bat_err_bit1.bat_underV = 0;
		}	
	}
	
}


/******************************************************************************************
** 函数名称：电池充电过流       
** 函数描述：
** 输入参数：无
** 返回值  ：无
*******************************************************************************************/
static void  fmod_fault_bat_over_chI (void)
{
	static uint16_t  u16_err_count = 0;
	static uint16_t  overchI_start_timer_flag=0;  //故障计时开始标志
	static uint16_t overchI_fault_count=0;       //故障计数
	static uint16_t overchI_fault_time=0;       //故障计时
	//.........故障判断
	if(st_bat_data.fl_bat_chI >=  st_product_preset.u8_ch_overI*10) 
	{ 
		if(u16_err_count <= 10)  //延时1秒
		{
			u16_err_count++; 
		}
		else
		{
			if(!un_bat_err1.st_bat_err_bit1.bat_over_chI) 
			{
				un_bat_err1.st_bat_err_bit1.bat_over_chI = 1;
				overchI_fault_count++;
				overchI_start_timer_flag = 1;
			}
			u16_err_count = 300; 
		}

	}
	//.........故障恢复  考虑发生锁死的时候
	if((st_bat_data.fl_bat_chI < 0.9f * st_product_preset.u8_ch_overI*10) &&  0==un_bat_lock.st_bat_lock_bit.bat_over_chI_lock )  
	{ 
		if(un_bat_err1.st_bat_err_bit1.bat_over_chI)
		{
			if(u16_err_count > 0 )
			{
				u16_err_count--;  
			}
			else
			{
				un_bat_err1.st_bat_err_bit1.bat_over_chI = 0;
			} 
		}
		else
		{
			u16_err_count = 0;
			un_bat_err1.st_bat_err_bit1.bat_over_chI = 0;
		}

	}

	//........故障计时
	if(1==overchI_start_timer_flag)
	{
		if(overchI_fault_time<=3500)//防止溢出
		{
			overchI_fault_time++;
		}
	}
	//.......判断该故障是否锁死
	if(overchI_fault_time<3000 && overchI_fault_count>=3) 
	{
		un_bat_lock.st_bat_lock_bit.bat_over_chI_lock  = 1; 
	}
	if(overchI_fault_time>=3000 && overchI_fault_count<3) 
	{
		overchI_fault_count=0;
		overchI_start_timer_flag=0;
	}   
}

/******************************************************************************************
** 函数名称：电池过放       
** 函数描述：
** 输入参数：无
** 返回值  ：无
*******************************************************************************************/
static void  fmod_fault_bat_overdischI (void)
{
	static uint16_t  u16_err_count = 0;

	
	if(st_bat_data.fl_bat_dischI >=  st_product_preset.u8_disch_overI *10 )	
	{	
		if(u16_err_count <= 5)  //1秒
		{
			u16_err_count++; 
		}
		else
		{
			un_bat_err1.st_bat_err_bit1.bat_overdischI = 1;
			u16_err_count = 5;              
		}
	}
	if(st_bat_data.fl_bat_dischI < 0.9f * st_product_preset.u8_disch_overI *10) 	
	{	
		if(u16_err_count > 0 )
		{
			u16_err_count--; 	
		}
		else
		{
			un_bat_err1.st_bat_err_bit1.bat_overdischI = 0;
		}	
	}
}

/******************************************************************************************
** 函数名称：电池过温       
** 函数描述：
** 输入参数：无
** 返回值  ：    无
*******************************************************************************************/
static void  fmod_fault_bat_overT (void)
{
	static uint16_t  u16_err_count = 0;
	static uint16_t  overT_start_timer_flag=0;  //故障计时开始标志
	static uint16_t overT_fault_count=0;       //故障计数
	static uint16_t overT_fault_time=0;       //故障计时
	//.........故障判断
	if(st_batcore_data.u16_batcore_temp[0] >=  (OVER_TEMP + 55) * 10) 
	{ 
		if(u16_err_count <= 10)  //延时1秒
		{
			u16_err_count++; 
		}
		else
		{
			if(!un_bat_err1.st_bat_err_bit1.bat_overT) 
			{
				un_bat_err1.st_bat_err_bit1.bat_overT = 1;
				overT_fault_count++;
				overT_start_timer_flag = 1;
			}
			u16_err_count = 300; 
		}

	}
	//.........故障恢复  考虑发生锁死的时候
	if((st_batcore_data.u16_batcore_temp[0]< (OVER_TEMP + 50) * 10) &&  0==un_bat_lock.st_bat_lock_bit.bat_overT_lock )  
	{ 
		if(un_bat_err1.st_bat_err_bit1.bat_overT)
		{
			if(u16_err_count > 0 )
			{
				u16_err_count--;  
			}
			else
			{
				un_bat_err1.st_bat_err_bit1.bat_overT = 0;
			} 
		}
		else
		{
			u16_err_count = 0;
			un_bat_err1.st_bat_err_bit1.bat_overT = 0;
		}

	}

	//........故障计时
	if(1==overT_start_timer_flag)
	{
		if(overT_fault_time<=3500)//防止溢出
		{
			overT_fault_time++;
		}
	}
	//.......判断该故障是否锁死
	if(overT_fault_time<3000 && overT_fault_count>=3) 
	{
		un_bat_lock.st_bat_lock_bit.bat_overT_lock  = 1; 
	}
	if(overT_fault_time>=3000 && overT_fault_count<3) 
	{
		overT_fault_count=0;
		overT_start_timer_flag=0;
	}   
}

/******************************************************************************************
** 函数名称：电池低温       
** 函数描述：
** 输入参数：无
** 返回值  ：无
*******************************************************************************************/
static void  fmod_fault_bat_underT (void)
{
	static uint16_t  u16_err_count = 0;

	if(st_bat_data.fl_bat_min_temp <=  (UNDER_TEMP + 55) * 10 ) 
	{	
		if(u16_err_count <= 10)  //1秒
		{
			u16_err_count++; 
		}
		else
		{
			un_bat_err1.st_bat_err_bit1.bat_underT = 1;
			u16_err_count = 10;              
		}
	}
	if(st_bat_data.fl_bat_min_temp >  ((UNDER_TEMP + 5) + 55) * 10)
	{	
		if(u16_err_count > 0 )
		{
			u16_err_count--; 	
		}
		else
		{
			un_bat_err1.st_bat_err_bit1.bat_underT = 0;
		}	
	}
}


/******************************************************************************************
** 函数名称：单体电池过压       
** 函数描述：
** 输入参数：无
** 返回值  ：无
*******************************************************************************************/
static void  fmod_fault_batcore_overV (void)
{
	static uint16_t  u16_err_count = 0;
	uint32_t i = 0;

	un_batcore_err.st_err.un_overV[0].u16_all =0x0000;

	for(i = 0; i < 9; i++ )
	{   
		if(st_batcore_data.u16_batcore_volt[i] >=  (BC_OVER_V + 0.162) * 10)
        {		
			un_batcore_err.st_err.un_overV[i/32].u16_all |= 1 << (i % 32);	
		}
		else
		{
			un_batcore_err.st_err.un_overV[i/32].u16_all &= ~(1 << (i % 32));	
		}
	}
	
    if( (un_batcore_err.st_err.un_overV[0].u16_all >=1) )
   	{
		if(u16_err_count <= 20)  //2秒
		{
			u16_err_count++; 
		}
		else
		{
			un_bat_err1.st_bat_err_bit1.batcore_overV = 1;
			u16_err_count = 20;              
		}
	}   
	else
	{
	    if(u16_err_count > 0 )
		{
			u16_err_count--; 	
		}
		else
		{
			un_bat_err1.st_bat_err_bit1.batcore_overV = 0;
		}
	}   
	   
	
	   
}
/******************************************************************************************
** 函数名称：单体电池欠压       
** 函数描述：
** 输入参数：无
** 返回值  ：无
*******************************************************************************************/
static void  fmod_fault_batcore_underV (void)
{  
	static uint16_t  u16_err_count = 0;
	uint16_t i = 0;

	un_batcore_err.st_err.un_underV[0].u16_all =0x0000;

	for(i = 0; i < 9; i++ )
	{   
		if(st_batcore_data.u16_batcore_volt[i] <=  (BC_UNDER_V -0.05)* 10)
        {		
			un_batcore_err.st_err.un_underV[0].u16_all |= 1 << (i % 16);	
		}
		else
		{
			un_batcore_err.st_err.un_underV[0].u16_all &= ~(1 << (i % 16));	
		}
	}
	
    if( (un_batcore_err.st_err.un_underV[0].u16_all >=1))
   	{
		if(u16_err_count <= 40)  //4秒
		{
			u16_err_count++; 
		}
		else
		{
			un_bat_err1.st_bat_err_bit1.batcore_underV = 1;
			u16_err_count = 40;              
		}
	}   
	else
	{
	    if(u16_err_count > 0 )
		{
			u16_err_count--; 	
		}
		else
		{
			un_bat_err1.st_bat_err_bit1.batcore_underV = 0;
		}
	}   
	     
}
/******************************************************************************************
** 函数名称：单体电池温度传感器故障      
** 函数描述：-45~125为检测温度的范围，超出则判断为温度传感器故障
** 输入参数：无
** 返回值  ：无
*******************************************************************************************/
static void  fmod_fault_sensor_T (void)
{
	static uint16_t  u16_err_count = 0;
	uint16_t i = 0;
	
	un_batcore_err.st_err.un_Terr[0].u16_all = 0x0000;
	
	for(i = 0; i < 9; i++ )
	{   
		if((st_batcore_data.u16_batcore_temp[i] >  (120 + 55)*10) || (st_batcore_data.u16_batcore_temp[i] < (-45 + 55)*10))
        {		
			un_batcore_err.st_err.un_Terr[i/16].u16_all |= 1 << (i % 16);	
		}
		else
		{
			un_batcore_err.st_err.un_Terr[i/16].u16_all &= ~(1 << (i % 16));	
		}
	}
	
    if( (un_batcore_err.st_err.un_Terr[0].u16_all >=1))
   	{
		if(u16_err_count <= 20)  //1秒
		{
			u16_err_count++; 
		}
		else
		{
			un_bat_err1.st_bat_err_bit1.bat_temp_fault = 1;
			u16_err_count = 20;              
		}
	}   
	else
	{
	    if(u16_err_count > 0 )
		{
			u16_err_count--; 	
		}
		else
		{
			un_bat_err1.st_bat_err_bit1.bat_temp_fault = 0;
		}
	}   
 	   
}


uint16_t fmod_Passive_Equilibrium(void)
{
	static uint16_t  u16_err_count = 0;
	uint16_t Passive_Equilibrium_bit= 0x0000;
	
	for(uint16_t i = 0; i < 9; i++ )
	{   
		
		if(st_batcore_data.u16_batcore_volt[i]>st_bat_data.u16_bat_avg_volt+10)
		{	
			Passive_Equilibrium_bit|= 1 << (i % 16);			
		}
		else
		{
			Passive_Equilibrium_bit &= ~(1 << (i % 16));	
		}
		
	}
	 if( (Passive_Equilibrium_bit >=1))
   	{
		if(u16_err_count <= 20)  //1秒
		{
			u16_err_count++; 
		}
		else
		{
			un_bat_err1.st_bat_err_bit1.PassiveEquilibrium=1;
			u16_err_count = 20;              
		}
	}   
	else
	{
	    if(u16_err_count > 0 )
		{
			u16_err_count--; 	
		}
		else
		{
			un_bat_err1.st_bat_err_bit1.PassiveEquilibrium=0;
		}
	}
	
	
	if(1==un_bat_err1.st_bat_err_bit1.PassiveEquilibrium)	
	{
		return Passive_Equilibrium_bit; 
	}
	else
	{
		return 0; 
	}
    

}	
///******************************************************************************************
//** 函数名称：自检故障      
//** 函数描述：
//** 输入参数：无
//** 返回值  ：无
//*******************************************************************************************/
void  fmod_self_test (void)
{
	uint16_t  u16_err_count = 0;
	if( un_KM_bit.st_KM_bit.KM1_work_sign != K1_FEED_VALUE||un_KM_bit.st_KM_bit.KM2_work_sign != K2_FEED_VALUE||
		un_KM_bit.st_KM_bit.KM3_work_sign == K3_FEED_VALUE||un_KM_bit.st_KM_bit.KM7_work_sign != K7_FEED_VALUE)		 
	{   
		if(u16_err_count <= 10) //1秒
		{
			u16_err_count++; 
		}
		else
		{
			un_bat_err2.st_bat_err_bit2.self_check_err=1;
			u16_err_count = 10;              //1秒
		}
		
	}
	else
	{
		un_bat_err2.st_bat_err_bit2.self_check_err=0;
	}
}
/******************************************************************************************
** 函数名称：接触器故障  
** 函数描述：
** 输入参数：无
** 返回值  ：无
*******************************************************************************************/
static void  fmod_fault_relay (void)
{
	static uint16_t  u16_errK1_count = 0;
	static uint16_t  u16_errK2_count = 0;
	static uint16_t  u16_errK3_count = 0;
	static uint16_t  u16_errK7_count = 0;
	static uint16_t  u16_lockK1_count = 0;
	static uint16_t  u16_lockK2_count = 0;
	static uint16_t  u16_lockK3_count = 0;
	static uint16_t  u16_lockK7_count = 0;
	static uint16_t  u16_upK1_count = 0;
	static uint16_t  u16_upK2_count = 0;
	static uint16_t  u16_upK3_count = 0;
	static uint16_t  u16_upK7_count = 0;
	
	
	
	//K1故障判断
	if(u16_lockK1_count >=3)
	{
		un_bat_err2.st_bat_err_bit2.KM1_fault_sign=1;
	}
	else if( un_KM_bit.st_KM_bit.KM1_work_sign != K1_FEED_VALUE)		 
	{   
		if(u16_errK1_count <= 20) 			//2秒
		{
			u16_errK1_count++; 
		}
		else
		{
			if(u16_upK1_count==0)
			{
				u16_lockK1_count++;
			}
			un_bat_err2.st_bat_err_bit2.KM1_fault_sign=1;
			u16_upK7_count++;
			u16_errK1_count = 20;              //2秒
			if(u16_upK1_count>=50)
			{
				u16_upK1_count=50;
			}
		}
	}
	else
	{
		un_bat_err2.st_bat_err_bit2.KM1_fault_sign=0;
		u16_errK1_count = 0;   
		u16_upK1_count=0;
	}
	
	
//K2故障判断
	if(u16_lockK2_count >=3)
	{
		un_bat_err2.st_bat_err_bit2.KM2_fault_sign=1;
	}
	else if( un_KM_bit.st_KM_bit.KM2_work_sign != K2_FEED_VALUE)		 
	{   
		if(u16_errK2_count <= 20) 			//2秒
		{
			u16_errK2_count++; 
		}
		else
		{
			if(u16_upK2_count==0)
			{
				u16_lockK2_count++;
			}
			un_bat_err2.st_bat_err_bit2.KM2_fault_sign=1;
			u16_upK2_count++;
			u16_errK2_count = 20;              //2秒
			if(u16_upK2_count>=50)
			{
				u16_upK2_count=50;
			}
		}
	}
	else
	{
		un_bat_err2.st_bat_err_bit2.KM2_fault_sign=0;
		u16_errK2_count = 0;   
		u16_upK2_count=0;
	}
	
	
	//K3故障判断
	if(u16_lockK3_count >=3)
	{
		un_bat_err2.st_bat_err_bit2.KM3_fault_sign=1;
	}
	else if( un_KM_bit.st_KM_bit.KM3_work_sign == K3_FEED_VALUE)		//K3反馈为常闭节点，所以驱动与反馈应相反 
	{   
		if(u16_errK3_count <= 20) 			//2秒
		{
			u16_errK3_count++; 
		}
		else
		{
			if(u16_upK3_count==0)
			{
				u16_lockK3_count++;
			}
			un_bat_err2.st_bat_err_bit2.KM3_fault_sign=1;
			u16_upK3_count++;
			u16_errK3_count = 20;              //2秒
			if(u16_upK3_count>=50)
			{
				u16_upK3_count=50;
			}
		}
	}
	else
	{
		un_bat_err2.st_bat_err_bit2.KM3_fault_sign=0;
		u16_errK3_count = 0;   
		u16_upK3_count=0;
	}
	
	
	//K7故障判断
	if(u16_lockK7_count >=3)
	{
		un_bat_err2.st_bat_err_bit2.KM7_fault_sign=1;
	}
	else if( un_KM_bit.st_KM_bit.KM7_work_sign != K7_FEED_VALUE)		 
	{   
		if(u16_errK7_count <= 20) 			//2秒
		{
			u16_errK7_count++; 
		}
		else
		{
			if(u16_upK7_count==0)
			{
				u16_lockK7_count++;
			}
			un_bat_err2.st_bat_err_bit2.KM7_fault_sign=1;
			u16_upK7_count++;
			u16_errK7_count = 20;              //2秒
			if(u16_upK7_count>=50)
			{
				u16_upK7_count=50;
			}
		}
	}
	else
	{
		un_bat_err2.st_bat_err_bit2.KM7_fault_sign=0;
		u16_errK7_count = 0;   
		u16_upK7_count=0;
	}
	
}
/******************************************************************************************
** 函数名称：CAN通讯故障       
** 函数描述：
** 输入参数：无
** 返回值  ：无
*******************************************************************************************/
static void  fmod_fault_CAN_com (void)
{
//	static uint8_t mvb_re_life = 0;
//	static uint32_t  mvb_com_count = 0;  
//	
//	if(mvb_re_life != (unsigned char)(un_remvb_data.st_re_mvb.u16_life & 0x00FF))
//	{
//		mvb_re_life = (unsigned char)(un_remvb_data.st_re_mvb.u16_life & 0x00FF);
//	    if(mvb_com_count > 0)
//		{
//			mvb_com_count--;
//		}
//		else
//		{
//			un_bat_err.st_bit.mvb_com_err  = 0;
//		}
//	}
//	else
//	{
//		if(mvb_com_count < 30)	//3秒 
//		{
//			mvb_com_count++;
//		}
//		else
//		{
//			un_bat_err.st_bit.mvb_com_err = 1;
//			mvb_com_count = 6;
//		}
//	}
}
///******************************************************************************************
//** 函数名称：485通讯故障       
//** 函数描述：
//** 输入参数：无
//** 返回值  ：无
//*******************************************************************************************/
//static void  fmod_fault_rs485_com (void)
//{
//	static uint8_t rs485_re_life = 0;
//	static uint32_t  rs485_com_count = 0;  
//	
//	if(rs485_re_life != un_re485_data.st_data.u8_life)
//	{
//		rs485_re_life = un_re485_data.st_data.u8_life;
//	    if(rs485_com_count > 0)
//		{
//			rs485_com_count--;
//		}
//		else
//		{
//			un_bat_err.st_bat_err_bit.CANorMVB_com_err  = 0;
//		}
//	}
//	else
//	{
//		if(rs485_com_count < 30)	//3秒 
//		{
//			rs485_com_count++;
//		}
//		else
//		{
//			un_bat_err.st_bat_err_bit.CANorMVB_com_err = 1;
//			rs485_com_count = 6;
//		}
//	}

///******************************************************************************************
//** 函数名称：电池短板故障       
//** 函数描述：
//** 输入参数：无
//** 返回值  ：无
//*******************************************************************************************/
//static void  fmod_fault_bat_short_board (void)
//{  
//	static uint16_t  u16_err_count = 0;
//	uint32_t i = 0;

//	for(i = 0; i < 9; i++ )
//	{   
//		if( ((st_batcore_data.u16_batcore_volt[i] <  (st_bat_data.u16_bat_avg_volt -10) )) //小于平均值1V
//            || (st_batcore_data.u16_batcore_volt[i] < 105))                              //低于10.5V
//	    {		
//			un_batcore_err.st_err.un_short_board[i/32].u16_all |= 1 << (i % 32);	
//		}
//		else
//		{
//			un_batcore_err.st_err.un_short_board[i/32].u16_all &= ~(1 << (i % 32));	
//		}	
//	}
//	
//    if( (un_batcore_err.st_err.un_short_board[0].u16_all >=1) )
//   	{
//		if(u16_err_count <= 20)  //2秒
//		{
//			u16_err_count++; 
//		}
//		else
//		{
//			un_bat_err.st_bat_err_bit.batcore_underV = 1;
//            for(i = 0; i < 9; i++ )
//            {
//				if( (un_batcore_err.st_err.un_short_board[i/32].u16_all >>(i % 32)) ==1 )
//                {
//					st_bat_data.u16_err_bat_num = i;
//				}

//			}
//			u16_err_count = 20;              
//		}
//	}   
//	else
//	{
//	    if(u16_err_count > 0 )
//		{
//			u16_err_count--; 	
//		}
//		else
//		{
//			un_bat_err.st_bat_err_bit.batcore_underV = 0;
//			st_bat_data.u16_err_bat_num = 0;
//		}
//	}   
//	     
//}
