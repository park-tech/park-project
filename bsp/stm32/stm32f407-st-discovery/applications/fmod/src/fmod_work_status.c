/********************************************************************************************
头文件
********************************************************************************************/
#include "fmod_work_status.h"

/********************************************************************************************
宏定义
********************************************************************************************/

/********************************************************************************************
变量定义
********************************************************************************************/

union	Bat_status_regs        un_bat_status ;

union   Contactor_status_regs     un_KM_bit;
uint16_t  u16_In_sleep_count1 = 0;
uint16_t  u16_K7_delay_count1 = 0;  
uint16_t  u16_In_sleep_count2 = 0;
uint16_t  u16_K7_delay_count2 = 0;  
uint16_t  u16_min_count2 = 0;  //用于计时24h

/********************************************************************************************
函数申明
********************************************************************************************/


/******************************************************************************************
** 函数名称：soc_soh_intiset        
** 函数描述：电池第一次上电的初始化设置
** 输入参数：无
** 返回值  ：    无
*******************************************************************************************/
void fmod_soc_soh_intiset(void)
{        
	st_bat_data.fl_bat_soc = 100;            //第一连接电池认为soc是100%   
	st_bat_data.fl_bat_soh = 100.1;          //第一连接电池认为soh是100% 
	st_bat_data.fl_bat_Qc_max =  Bat_Qb;     //第一连接电池认为最大容量是初始容量   为实际值的10000倍
	st_bat_data.fl_bat_Qnow = st_bat_data.fl_bat_soc * 0.01f * Bat_Qb ; //第一连接电池的当前容量
}
///******************************************************************************************
//** 函数名称：soc_soh_intiset        
//** 函数描述：根据开路电压校正SOC的值
//** 输入参数：无
//** 返回值  ：    无
//*******************************************************************************************/
//void fmod_open_volt_adj_soc(void)
//{        
//	st_bat_data.fl_bat_soc = 400 * (st_bat_data.fl_bat_volt /TEST_BAT_NUM) - 756;         //一个电池开路电压时，电压和容量之间都有一个特定的对应曲线
//	st_bat_data.fl_bat_Qnow = st_bat_data.fl_bat_soc * 0.01f * st_bat_data.fl_bat_Qc_max ; //第一连接电池的当前容量
//}

/******************************************************************************************
** 函数名称：根据静态电压调节SOC        
** 函数描述：电池第一次上电的初始化设置
** 输入参数：无
** 返回值  ：    无
*******************************************************************************************/
void fmod_static_volt_adj_soc(void)
{        
	st_bat_data.fl_bat_Qc_max =  Bat_Qb;     //第一连接电池认为最大容量是初始容量   为实际值的10000倍
	st_bat_data.fl_bat_Qnow = st_bat_data.fl_bat_soc * 0.01f * Bat_Qb ; //第一连接电池的当前容量
}
/******************************************************************************************
** 函数名称：calculate_soc	 
** 函数描述：实时计算soc
** 输入参数：无
** 返回值  ：	 无
*******************************************************************************************/
void fmod_updata_soc(void)
{
	//..................根据当前容量计算SOC的值...............................
	st_bat_data.fl_bat_soc = st_bat_data.fl_bat_Qnow * 100 / st_bat_data.fl_bat_Qc_max ;	   //计算soc
	if (st_bat_data.fl_bat_soc >= 100)  st_bat_data.fl_bat_soc = 100;
	if (st_bat_data.fl_bat_soc <= 2)	st_bat_data.fl_bat_soc = 2;
}

/******************************************************************************************
** 函数名称：更新soh         
** 函数描述：在充满和放空时更新soh
** 输入参数：无
** 返回值  ：无
*******************************************************************************************/
void fmod_updata_soh(void)
{
    uint8_t i = 0, float_charge_sign = 0;
	float fl_bat_ago_soh = 0;
	static float fl_bat_agochI[10]  = {2};

	float_charge_sign = 0;
	for(i = 0; i < 10; i++)
	{
		if(fl_bat_agochI[i] >= 1*10) //连续10次都小于1A
		{
        	float_charge_sign++;
		}
	}
	
    //......电池充满时对soh的校正...判定依据：电压大于充满电压，放电电流小于1A	两次充电电流都小于充满电流，		 
	if( (st_bat_data.fl_bat_volt >= Charge_full_V*10) && (st_bat_data.fl_bat_dischI <= 1*10) &&
		(st_bat_data.fl_bat_chI < Charge_full_I*10) && (float_charge_sign == 0) )
	{
		if(!un_bat_status.st_bit.bat_full)						  //每次充电的第一次进入充满状态
		{	 
			un_bat_status.st_bit.bat_full = 1;					  //表示电池已经进入过充满状态了
			st_bat_data.fl_bat_Qc_max = st_bat_data.fl_bat_Qnow ; //此时当前剩余的电池容量为充满的最大容量

			if(st_bat_data.fl_bat_Qc_max >= Bat_Qb)         st_bat_data.fl_bat_Qc_max = Bat_Qb;				
			if(st_bat_data.fl_bat_Qc_max <= (Bat_Qb * 0.5)) st_bat_data.fl_bat_Qc_max = Bat_Qb * 0.5; //检测的最低容量为标称容量的0.5	
			
			fl_bat_ago_soh = st_bat_data.fl_bat_soh;                          //记录更新之前的SOH
			st_bat_data.fl_bat_soh = 100 * st_bat_data.fl_bat_Qc_max / Bat_Qb; //计算新的SOH 
			
			if(st_bat_data.fl_bat_soh >= fl_bat_ago_soh + 0.1f)
			{
				st_bat_data.fl_bat_soh = fl_bat_ago_soh + 0.1f;
			}
			if(st_bat_data.fl_bat_soh <= fl_bat_ago_soh - 0.1f)
			{
				st_bat_data.fl_bat_soh = fl_bat_ago_soh - 0.1f;
			}
			
			if(st_bat_data.fl_bat_soh >= 100) st_bat_data.fl_bat_soh = 100;
			if(st_bat_data.fl_bat_soh <= 70)  st_bat_data.fl_bat_soh = 70;

			st_bat_data.fl_bat_Qc_max = st_bat_data.fl_bat_soh * 0.01f * Bat_Qb ;  //根据新的SOH校正当前可充满的最大容量
			st_bat_data.fl_bat_soc = 100;
				
        	st_bat_data.fl_bat_Qnow = st_bat_data.fl_bat_Qc_max * st_bat_data.fl_bat_soc * 0.01f ;  //跟新当前容量
		}
		else					 			  
		{
			 st_bat_data.fl_bat_soc = 100;			 
		} 
	}

	//.................充满标志清0(电压小于13.6)，表示下次又可以进行第一次冲满状态..........
	if(st_bat_data.fl_bat_volt <= (Charge_full_V - 0.8)*10)   
	{
		un_bat_status.st_bit.bat_full = 0; 		 
	}

    //............................更新充电电流值.........................
	for(i = 0; i < 9; i++)
	{
		fl_bat_agochI[i] = fl_bat_agochI[i+1];
	}
	fl_bat_agochI[9] = st_bat_data.fl_bat_chI; 
	
	
	//st_KM_status.bat_full = un_bat_status.st_bit.bat_full;

}


/*********************************************************************************************************
** 函数名称：
** 函数描述：继电器控制
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
void fmod_relay_control()
{

	if(un_bat_lock.st_bat_lock_bit.bat_overT_lock ||un_bat_lock.st_bat_lock_bit.bat_over_chI_lock)
	{
		K1_START_PIN_OFF;
		un_KM_bit.st_KM_bit.KM1_work_sign=0;
	
	}
	else
	{
		if(un_bat_err1.st_bat_err_bit1.batcore_overV == 1 ||  un_bat_err1.st_bat_err_bit1.bat_overT == 1
			||  un_bat_err1.st_bat_err_bit1.bat_over_chI == 1)
		{
		
			K1_START_PIN_OFF;
			un_KM_bit.st_KM_bit.KM1_work_sign=0;
		
		}
		else
		{
			K1_START_PIN_ON;
			un_KM_bit.st_KM_bit.KM1_work_sign=1;
		}
	
	}
	
	
	
	
	
	
	if(un_bat_err2.st_bat_err_bit2.charger_underV==0)        //外部充电机电压大于限值时
	{
		K2_START_PIN_OFF;										//断开K2
		un_KM_bit.st_KM_bit.KM2_work_sign=0;
		u16_In_sleep_count2 = 0;								//外部充电机电压低于限值的计时器清零
		u16_K7_delay_count2 = 0;								//外部充电机电压低于限值的计时器清零
		
		if(un_sys_Inout_bit.st_Inout_bits.In_Sleep==1)
		{
			if(u16_In_sleep_count1 <= 600) //1min
			{
				u16_In_sleep_count1++; 
				K7_START_PIN_ON;
				un_KM_bit.st_KM_bit.KM7_work_sign=1;
			}
			else
			{
				u16_In_sleep_count1=600;
				if(u16_K7_delay_count1 <=600)
				{
					u16_K7_delay_count1++;
					K7_START_PIN_ON;
					un_KM_bit.st_KM_bit.KM7_work_sign=1;
				}
				else
				{
					K7_START_PIN_OFF;
					un_KM_bit.st_KM_bit.KM7_work_sign=0;
					u16_K7_delay_count1=600;
				}
			}
		}
		else
		{	
			K7_START_PIN_ON;
			un_KM_bit.st_KM_bit.KM7_work_sign=1;
			u16_In_sleep_count1=0;
			u16_K7_delay_count1=0;
		}
	}
	
	else	
	{
		u16_In_sleep_count1 = 0;								//外部充电机电压高于限值的计时器清零
		u16_K7_delay_count1 = 0;								//外部充电机电压高于限值的计时器清零
		
	
		if(u16_min_count2 < 1440)                                  //24小时
		{
			K2_START_PIN_ON;										
			un_KM_bit.st_KM_bit.KM2_work_sign=1;
			K7_START_PIN_ON;
			un_KM_bit.st_KM_bit.KM7_work_sign=1;
			
			if(u16_In_sleep_count2>=600)
			{
				u16_min_count2++;
				u16_In_sleep_count2=0;
			}
			u16_In_sleep_count2++;
		}
		else
		{
			u16_min_count2=1440;
			K2_START_PIN_OFF;										
			un_KM_bit.st_KM_bit.KM2_work_sign=0;
			K7_START_PIN_OFF;
			un_KM_bit.st_KM_bit.KM7_work_sign=0;
			
		}			
	
	}


}
