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
union	Bat_err_regs      un_bat_err;
union	Batcore_err_regs  un_batcore_err;

/********************************************************************************************
函数申明
********************************************************************************************/
static void fmod_fault_relay(void);
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
static void fmod_fault_batcore_overR (void);
static void  fmod_fault_bat_short_board (void);
static void fmod_fault_eth_com(void);	
static void fmod_fault_mvb_com(void);
static void fmod_fault_rs485_com (void);
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

	//........................电池组低压预警................................
	//fmod_fault_bat_underV_warn();

	//........................电池组充电过流................................
	fmod_fault_bat_over_chI();
	
	//........................电池组放电过流................................
	fmod_fault_bat_overdischI();

	//........................电池过温................................
	fmod_fault_bat_overT();

	//........................电池低温................................
	fmod_fault_bat_underT();
	
	//........................电池温度传感器故障................................
	//fmod_fault_sensor_T();

	//........................单体电池过压................................
	fmod_fault_batcore_overV ();

	//........................单体电池欠压................................
	fmod_fault_batcore_underV ();

	//........................单体电池内阻过高................................
	//fmod_fault_batcore_overR ();
	
	//........................单体电池短板故障................................
	fmod_fault_bat_short_board ();

	//........................ETH通讯故障..................................
	//fmod_fault_eth_com();	
		
	//........................MVB通讯故障.....................................
	//fmod_fault_mvb_com();
    // ........................RS485通讯故障................................
    fmod_fault_rs485_com();
//	printf("un_bat_err.u16_all = %04x\n", un_bat_err.u16_all );
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

	if( st_KM_bit.KM1_work_sign != K1_FEED_VALUE)		 
	{   
		if(u16_errK1_count <= 10) //1秒
		{
			u16_errK1_count++; 
		}
		else
		{
			st_KM_bit.KM1_fault_sign=1;
			u16_errK1_count = 10;              //1秒
		}
	}
	
	if( st_KM_bit.KM2_work_sign != K2_FEED_VALUE)		 
	{   
		if(u16_errK2_count <= 10) //1秒
		{
			u16_errK2_count++; 
		}
		else
		{
			st_KM_bit.KM2_fault_sign=1;
			u16_errK2_count = 10;              //1秒
		}
	}
	if( st_KM_bit.KM3_work_sign != K3_FEED_VALUE)		 
	{   
		if(u16_errK3_count <= 10) //1秒
		{
			u16_errK3_count++; 
		}
		else
		{
			st_KM_bit.KM3_fault_sign=1;
			u16_errK3_count = 10;              //1秒
		}
	}
	if( st_KM_bit.KM7_work_sign != K7_FEED_VALUE)		 
	{   
		if(u16_errK7_count <= 10) //1秒
		{
			u16_errK7_count++; 
		}
		else
		{
			st_KM_bit.KM7_fault_sign=1;
			u16_errK7_count = 10;              //1秒
		}
	}
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
	

	if(st_bat_data.fl_bat_volt >= (BC_OVER_V * TEST_BAT_NUM ))		 //过压值为14.7V
	{   
		if(u16_err_count <= 10) //1秒
		{
			u16_err_count++; 
		}
		else
		{
			un_bat_err.st_bit.bat_overV = 1;
			u16_err_count = 10;              //1秒
		}
	}

	if(st_bat_data.fl_bat_volt <= (BC_OVER_V - 0.08) * TEST_BAT_NUM)	//回差设置	 //过压值为14.7V
	{   
		
		if(u16_err_count > 0 )
		{
			u16_err_count--; 	
		}
		else
		{
			un_bat_err.st_bit.bat_overV = 0;
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
	
	if(st_bat_data.fl_bat_volt < (BC_UNDER_V * TEST_BAT_NUM ) )  
	{		
		if(u16_err_count <= 10)  //1秒
		{
			u16_err_count++; 
		}
		else
		{
			un_bat_err.st_bit.bat_underV = 1;
			u16_err_count = 10;              
		}
	}

	if(st_bat_data.fl_bat_volt >= ((BC_UNDER_V + 0.09 ) * TEST_BAT_NUM) ) 	  
	{	
		if(u16_err_count > 0 )
		{
			u16_err_count--; 	
		}
		else
		{
			un_bat_err.st_bit.bat_underV = 0;
		}	
	}
	
}


/******************************************************************************************
** 函数名称：电池低压预警       
** 函数描述：
** 输入参数：无
** 返回值  ：无
*******************************************************************************************/
static void  fmod_fault_bat_underV_warn  (void)
{
    static uint16_t  u16_err_count = 0;
	
	if(st_bat_data.fl_bat_volt < BC_LOW_PREWARN_V * TEST_BAT_NUM )		
	{   
		if(u16_err_count <= 10)  //1秒
		{
			u16_err_count++; 
		}
		else
		{
			un_bat_err.st_bit.bat_underV_warn = 1;
			u16_err_count = 10;              
		}
	}
	if(st_bat_data.fl_bat_volt >=((BC_LOW_PREWARN_V + 0.1) * TEST_BAT_NUM))		
	{   
		if(u16_err_count > 0 )
		{
			u16_err_count--; 	
		}
		else
		{
			un_bat_err.st_bit.bat_underV_warn = 0;
		}	
	}


}

/******************************************************************************************
** 函数名称：电池过充       
** 函数描述：
** 输入参数：无
** 返回值  ：无
*******************************************************************************************/
static void  fmod_fault_bat_over_chI (void)
{
	static uint16_t  u16_err_count = 0;

	if(st_bat_data.fl_bat_chI >=  st_product_preset.u8_ch_overI )	
	{	
		if(u16_err_count <= 10)  //1秒
		{
			u16_err_count++; 
		}
		else
		{
			un_bat_err.st_bit.bat_over_chI = 1;
			u16_err_count = 10;              
		}
	}
	if(st_bat_data.fl_bat_chI < 0.9f * st_product_preset.u8_ch_overI) 	
	{	
		if(u16_err_count > 0 )
		{
			u16_err_count--; 	
		}
		else
		{
			un_bat_err.st_bit.bat_over_chI = 0;
		}	
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

	
	if(st_bat_data.fl_bat_dischI >=  st_product_preset.u8_disch_overI )	
	{	
		if(u16_err_count <= 5)  //1秒
		{
			u16_err_count++; 
		}
		else
		{
			un_bat_err.st_bit.bat_overdischI = 1;
			u16_err_count = 5;              
		}
	}
	if(st_bat_data.fl_bat_dischI < 0.9f * st_product_preset.u8_disch_overI) 	
	{	
		if(u16_err_count > 0 )
		{
			u16_err_count--; 	
		}
		else
		{
			un_bat_err.st_bit.bat_overdischI = 0;
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
	uint32_t i = 0;

	un_batcore_err.st_err.un_overT[0].u32_all = 0x00000000;
	un_batcore_err.st_err.un_overT[1].u32_all = 0x00000000;
	un_batcore_err.st_err.un_overT[2].u32_all = 0x00000000;

	for(i = 0; i < TEST_BAT_NUM; i++ )
	{   
		if(st_batcore_data.u16_batcore_temp[i] >=  (OVER_TEMP + 55) * 10 )
        {		
			 un_batcore_err.st_err.un_overT[i/32].u32_all |= 1 << (i % 32);	
		}
		else
		{
			un_batcore_err.st_err.un_overT[i/32].u32_all &= ~(1 << (i % 32));	
		}
	}
	
	if(st_bat_data.fl_bat_max_temp >=  OVER_TEMP ) 
	{	
		if(u16_err_count <= 10)  //1秒
		{
			u16_err_count++; 
		}
		else
		{
			un_bat_err.st_bit.bat_overT = 1;
			u16_err_count = 10;              
		}
	}
	if(st_bat_data.fl_bat_max_temp < 0.95 * OVER_TEMP)	
	{	
		if(u16_err_count > 0 )
		{
			u16_err_count--; 	
		}
		else
		{
			un_bat_err.st_bit.bat_overT = 0;
		}	
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

	if(st_bat_data.fl_bat_min_temp <=  UNDER_TEMP ) 
	{	
		if(u16_err_count <= 10)  //1秒
		{
			u16_err_count++; 
		}
		else
		{
			un_bat_err.st_bit.bat_underT = 1;
			u16_err_count = 10;              
		}
	}
	if(st_bat_data.fl_bat_min_temp >  (UNDER_TEMP + 5))
	{	
		if(u16_err_count > 0 )
		{
			u16_err_count--; 	
		}
		else
		{
			un_bat_err.st_bit.bat_underT = 0;
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
	uint32_t i = 0;
	
	un_batcore_err.st_err.un_Terr[0].u32_all = 0x00000000;
	un_batcore_err.st_err.un_Terr[1].u32_all = 0x00000000;
	un_batcore_err.st_err.un_Terr[2].u32_all = 0x00000000;


	for(i = 0; i < TEST_BAT_NUM; i++ )
	{   
		if((st_batcore_data.u16_batcore_temp[i] >  (120 + 55)*10) || (st_batcore_data.u16_batcore_temp[i] < (-45 + 55)*10))
        {		
			un_batcore_err.st_err.un_Terr[i/32].u32_all |= 1 << (i % 32);	
		}
		else
		{
			un_batcore_err.st_err.un_Terr[i/32].u32_all &= ~(1 << (i % 32));	
		}
	}
	
    if( (un_batcore_err.st_err.un_Terr[0].u32_all >=1) ||
   	   (un_batcore_err.st_err.un_Terr[1].u32_all >=1) ||
	   (un_batcore_err.st_err.un_Terr[2].u32_all >=1) )
   	{
		if(u16_err_count <= 20)  //1秒
		{
			u16_err_count++; 
		}
		else
		{
			un_bat_err.st_bit.bat_temp_fault = 1;
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
			un_bat_err.st_bit.bat_temp_fault = 0;
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

	un_batcore_err.st_err.un_overV[0].u32_all =0x00000000;
	un_batcore_err.st_err.un_overV[1].u32_all =0x00000000;
	un_batcore_err.st_err.un_overV[2].u32_all =0x00000000;

	for(i = 0; i < TEST_BAT_NUM; i++ )
	{   
		if(st_batcore_data.u16_batcore_volt[i] >=  (BC_OVER_V + 0.162) * 1000)
        {		
			un_batcore_err.st_err.un_overV[i/32].u32_all |= 1 << (i % 32);	
		}
		else
		{
			un_batcore_err.st_err.un_overV[i/32].u32_all &= ~(1 << (i % 32));	
		}
	}
	
    if( (un_batcore_err.st_err.un_overV[0].u32_all >=1) ||
   	   (un_batcore_err.st_err.un_overV[1].u32_all >=1) ||
	   (un_batcore_err.st_err.un_overV[2].u32_all >=1) )
   	{
		if(u16_err_count <= 20)  //2秒
		{
			u16_err_count++; 
		}
		else
		{
			un_bat_err.st_bit.batcore_overV = 1;
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
			un_bat_err.st_bit.batcore_overV = 0;
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
	uint32_t i = 0;

	un_batcore_err.st_err.un_underV[0].u32_all =0x00000000;
	un_batcore_err.st_err.un_underV[1].u32_all =0x00000000;

	for(i = 0; i < TEST_BAT_NUM; i++ )
	{   
		if(st_batcore_data.u16_batcore_volt[i] <=  (BC_UNDER_V -0.05)* 1000)
        {		
			un_batcore_err.st_err.un_underV[i/32].u32_all |= 1 << (i % 32);	
		}
		else
		{
			un_batcore_err.st_err.un_underV[i/32].u32_all &= ~(1 << (i % 32));	
		}
	}
	
    if( (un_batcore_err.st_err.un_underV[0].u32_all >=1) ||
   	    (un_batcore_err.st_err.un_underV[1].u32_all >=1) ||
	    (un_batcore_err.st_err.un_underV[2].u32_all >=1) )
   	{
		if(u16_err_count <= 20)  //2秒
		{
			u16_err_count++; 
		}
		else
		{
			un_bat_err.st_bit.batcore_underV = 1;
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
			un_bat_err.st_bit.batcore_underV = 0;
		}
	}   
	     
}


/******************************************************************************************
** 函数名称：单体电池内阻过高       
** 函数描述：
** 输入参数：无
** 返回值  ：无
*******************************************************************************************/
static void  fmod_fault_batcore_overR (void)
{   
	static uint16_t  u16_err_count = 0;
	uint32_t i = 0;

	un_batcore_err.st_err.un_overR[0].u32_all =0x00000000;
	un_batcore_err.st_err.un_overR[1].u32_all =0x00000000;
	un_batcore_err.st_err.un_overR[2].u32_all =0x00000000;

	for(i = 0; i < TEST_BAT_NUM; i++ )
	{	
		if(st_batcore_data.u16_batcore_R[i] >=  BC_OVER_R)
		{		
			un_batcore_err.st_err.un_overR[i/32].u32_all |= 1 << (i % 32);	
		}
		else
		{
			un_batcore_err.st_err.un_overR[i/32].u32_all &= ~(1 << (i % 32));	
		}
	}
	
	if( (un_batcore_err.st_err.un_overR[0].u32_all >=1) ||
	    (un_batcore_err.st_err.un_overR[1].u32_all >=1) ||
	    (un_batcore_err.st_err.un_overR[2].u32_all >=1) )
	{	
		if(u16_err_count <= 20)  //2秒
		{
			u16_err_count++; 
		}
		else
		{
			un_bat_err.st_bit.bat_overR = 1;
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
			un_bat_err.st_bit.bat_overR = 0;
		}
	}	
	   	   
}

/******************************************************************************************
** 函数名称：电池短板故障       
** 函数描述：
** 输入参数：无
** 返回值  ：无
*******************************************************************************************/
static void  fmod_fault_bat_short_board (void)
{  
	static uint16_t  u16_err_count = 0;
	uint32_t i = 0;

	for(i = 0; i < TEST_BAT_NUM; i++ )
	{   
		if( ((st_batcore_data.u16_batcore_volt[i] <  (st_bat_data.u16_bat_avg_volt -0.55) * 1000) && (st_batcore_data.u16_batcore_R[i] >=  BC_OVER_R) ) 
            || (st_batcore_data.u16_batcore_volt[i] < 1.2 * 1000))
	    {		
			un_batcore_err.st_err.un_short_board[i/32].u32_all |= 1 << (i % 32);	
		}
		else
		{
			un_batcore_err.st_err.un_short_board[i/32].u32_all &= ~(1 << (i % 32));	
		}	
	}
	
    if( (un_batcore_err.st_err.un_short_board[0].u32_all >=1) ||
   	    (un_batcore_err.st_err.un_short_board[1].u32_all >=1) ||
	    (un_batcore_err.st_err.un_short_board[2].u32_all >=1) )
   	{
		if(u16_err_count <= 20)  //2秒
		{
			u16_err_count++; 
		}
		else
		{
			un_bat_err.st_bit.batcore_underV = 1;
            for(i = 0; i < TEST_BAT_NUM; i++ )
            {
				if( (un_batcore_err.st_err.un_short_board[i/32].u32_all >>(i % 32)) ==1 )
                {
					st_bat_data.u16_err_bat_num = i;
				}

			}
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
			un_bat_err.st_bit.batcore_underV = 0;
			st_bat_data.u16_err_bat_num = 0;
		}
	}   
	     
}
/******************************************************************************************
** 函数名称：ETH通讯故障    
** 函数描述：
** 输入参数：无
** 返回值  ：无
*******************************************************************************************/
static void  fmod_fault_eth_com(void)
{
	
}
/******************************************************************************************
** 函数名称：mvb通讯故障       
** 函数描述：
** 输入参数：无
** 返回值  ：无
*******************************************************************************************/
static void  fmod_fault_mvb_com (void)
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
/******************************************************************************************
** 函数名称：mvb通讯故障       
** 函数描述：
** 输入参数：无
** 返回值  ：无
*******************************************************************************************/
static void  fmod_fault_rs485_com (void)
{
	static uint8_t rs485_re_life = 0;
	static uint32_t  rs485_com_count = 0;  
	
	if(rs485_re_life != un_re485_data.st_data.u8_life)
	{
		rs485_re_life = un_re485_data.st_data.u8_life;
	    if(rs485_com_count > 0)
		{
			rs485_com_count--;
		}
		else
		{
			un_bat_err.st_bit.rs485_com_err  = 0;
		}
	}
	else
	{
		if(rs485_com_count < 30)	//3秒 
		{
			rs485_com_count++;
		}
		else
		{
			un_bat_err.st_bit.rs485_com_err = 1;
			rs485_com_count = 6;
		}
	}
}

