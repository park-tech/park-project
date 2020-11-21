
/********************************************************************************************
头文件
********************************************************************************************/
#include "fmod_data_process.h"
/********************************************************************************************
宏定义
********************************************************************************************/
/********************************************************************************************
变量定义
********************************************************************************************/
struct  Product_info  st_product_info;
struct  product_preset st_product_preset;
//struct  Bat_err_bits   st_bat_err_bit;
struct  Contactor_status_bits   st_KM_bit;
//struct  Sys_Inout_bits   st_Inout_bits;
struct  Bat_data      st_bat_data;  
struct  Batcore_data  st_batcore_data;

union   Sys_Inout_regs     un_sys_Inout_bit;
//extern  struct rt_messagequeue adc_rx_mq; 

/********************************************************************************************
函数申明
********************************************************************************************/
static void fmod_adc_data_update(void);
static void fmod_inout_data_update(void);

/** * @ : *************************************************************************
 * @name: 
 * @describe: 使用设备初始化
 * @param : 
 * @return: 
 * @  : ***************************************************************************/
void fmod_device_init(void)
{	 	
	bsp_gpio_init( );
	basic_hwtimer_init(0, 10000); //    10000us

	//basic_dac_init( );
	//gpio_exti_init(111);
  //basic_pwm_init(PWM_DEV_CHANNEL2);
}

/** * @ : *************************************************************************
 * @name: 
 * @describe: 变量初始化
 * @param : 
 * @return: 
 * @  : ***************************************************************************/
void fmod_variable_init(void)
{
   	char project_name[] = "haitai";	
	char product_name[] = "BMS";
    char soft_version[] = "1.0";
	//char timestr[20] =    "2019-01-01 00:00:00";
    // uint16_t i = 0;
    
	//.....................产品信息初始化...............................
    strcpy(st_product_info.i8_soft_version, soft_version);
	strcpy(st_product_info.i8_project_name, project_name);  
	strcpy(st_product_info.i8_product_name, product_name);	
	
	st_product_info.u16_product_num = 0x0001;    //产品编号
	st_product_info.u8_car_num = 0x01;           //车号	
	st_product_info.u8_bat_num = 0x09;           //电池检测数量  
	
	st_product_preset.u8_ch_overI=0x02;
	st_product_preset.u8_disch_overI=0x96;
	st_product_preset.u8_charger_underV=0x54;
    
	if((st_product_preset.u8_Charge_I_adjust_Value > 220)||(st_product_preset.u8_Charge_I_adjust_Value < 180))
	{
		st_product_preset.u8_Charge_I_adjust_Value = 200;
	}
	if((st_product_preset.u8_disCharge_I1_adjust_Value > 220)||(st_product_preset.u8_disCharge_I1_adjust_Value < 180))
	{
		st_product_preset.u8_disCharge_I1_adjust_Value = 200;
	}
	if((st_product_preset.u8_disCharge_I2_adjust_Value > 220)||(st_product_preset.u8_disCharge_I2_adjust_Value < 180))
	{
		st_product_preset.u8_disCharge_I2_adjust_Value = 200;
	}
	if((st_product_preset.u8_Outcharger_V1_adjust_Value > 220)||(st_product_preset.u8_Outcharger_V1_adjust_Value < 180))
	{
		st_product_preset.u8_Outcharger_V1_adjust_Value = 200;
	}
	if((st_product_preset.u8_Outcharger_V2_adjust_Value > 220)||(st_product_preset.u8_Outcharger_V2_adjust_Value < 180))
	{
		st_product_preset.u8_Outcharger_V2_adjust_Value = 200;
	}
	if((st_product_preset.u16_batcore_Volt1_adjust_Value > 1200)||(st_product_preset.u16_batcore_Volt1_adjust_Value < 800))
	{
		st_product_preset.u16_batcore_Volt1_adjust_Value = 1000;
	}
	if((st_product_preset.u16_batcore_Volt2_adjust_Value > 1200)||(st_product_preset.u16_batcore_Volt2_adjust_Value < 800))
	{
		st_product_preset.u16_batcore_Volt2_adjust_Value = 1000;
	}
	un_bat_status.st_bit.bat_first_work = 1;     //初始化都当做首次启动

   // strcpy(st_2g_send_data.i8_timestring, timestr);  //设定初始上传时间			
}
/** * @ : *****************************************************************************
 * @name: 
 * @describe: 参数赋值更新
 * @param : 
 * @return: 
 * @  : *******************************************************************************/

void fmod_parameter_update(void)
{   
	static uint8_t  run_count = 0 ;
	uint32_t  now_sec= 0;
	
	
	//.......................电池电流电压温度参数更新............................. 
	
    fmod_adc_data_update( );      // 更新最大、最低单体电压，温度等值
	fmod_inout_data_update( );      // 更新数字量输入输出信号
	
	
	fmod_updata_soh( ); //在浮充状态时会更新SOH	
	fmod_updata_soc( );   
} 

/** * @ : **********************************************************************
 * @name: ADC数据的更新
 * @describe:更新最大、最低单体电压，温度等值
 * @param : 
 * @return: 
 * @  : ************************************************************************/
 static void fmod_adc_data_update(void)
 {	
	
	uint16_t u16_bat_volt=0;
	uint16_t u16_bat_min_volt_temp=st_batcore_data.u16_batcore_volt[0];   //电池最低电压初始化
	uint16_t u16_bat_max_volt_temp=st_batcore_data.u16_batcore_volt[0];   //电池最高电压初始化
	uint16_t fl_bat_min_temp_temp=st_batcore_data.u16_batcore_temp[0];//电池最低温度初始化
	uint16_t fl_bat_max_temp_temp=st_batcore_data.u16_batcore_temp[0];//电池最高温度初始化
	
	for(uint16_t i=0;i<TEST_BAT_NUM;i++)
	{
		//电池总电压
		u16_bat_volt += st_batcore_data.u16_batcore_volt[i];
		
		//电池最高电压
		if(u16_bat_max_volt_temp<st_batcore_data.u16_batcore_volt[i])
		{
			u16_bat_max_volt_temp=st_batcore_data.u16_batcore_volt[i];
			
		}
		
		//电池最低电压
		if(u16_bat_min_volt_temp>st_batcore_data.u16_batcore_volt[i])
		{
			u16_bat_min_volt_temp=st_batcore_data.u16_batcore_volt[i];
		}
		
//		//电池最高温度
//		if(fl_bat_max_temp_temp<st_batcore_data.u16_batcore_temp[i])
//		{
//			fl_bat_max_temp_temp=st_batcore_data.u16_batcore_temp[i];
//		}
//		
//		//电池最低温度
//		if(fl_bat_min_temp_temp>st_batcore_data.u16_batcore_temp[i])
//		{
//			fl_bat_min_temp_temp=st_batcore_data.u16_batcore_temp[i];
//		
//		}
	}


	st_bat_data.fl_bat_volt =u16_bat_volt/10.0f;
	st_bat_data.u16_bat_max_volt=u16_bat_max_volt_temp;
	st_bat_data.u16_bat_min_volt=u16_bat_min_volt_temp;
//	st_bat_data.fl_bat_max_temp=fl_bat_max_temp_temp;
//	st_bat_data.fl_bat_min_temp=fl_bat_min_temp_temp;
	st_bat_data.fl_bat_max_temp=st_batcore_data.u16_batcore_temp[0];
	st_bat_data.fl_bat_min_temp=st_batcore_data.u16_batcore_temp[0];
	st_bat_data.u16_bat_avg_volt=st_bat_data.fl_bat_volt/TEST_BAT_NUM*10.0f;

 }
/** * @ : **********************************************************************
 * @name: 数字量输入输出数据更新
 * @describe:
 * @param : 
 * @return: 
 * @  : ************************************************************************/
 static void fmod_inout_data_update(void)
 {	
	
	if(1==SLEEP_STATUS_VALUE)
	{
		un_sys_Inout_bit.st_Inout_bits.In_Sleep=1;
	}
	else
	{
		un_sys_Inout_bit.st_Inout_bits.In_Sleep=0;
	
	}
	if(1==SPEED0_STATUS_VALUE)
	{
		un_sys_Inout_bit.st_Inout_bits.In_speed0=1;
	}
	else
	{
		un_sys_Inout_bit.st_Inout_bits.In_speed0=0;
	
	}
	if(1==DC_Charger_STATUS_VALUE)
	{
		un_sys_Inout_bit.st_Inout_bits.In_DC_Charger_fault=1;
	}
	else
	{
		un_sys_Inout_bit.st_Inout_bits.In_DC_Charger_fault=0;
	
	}
	if((un_bat_err1.u16_all > 0)||(un_bat_err2.u16_all > 0))
	{
		un_sys_Inout_bit.st_Inout_bits.Out_Sys_fault=1;
	}
	else
	{
		un_sys_Inout_bit.st_Inout_bits.Out_Sys_fault=0;
	
	}

 }
/*********************************************************************************************************
** 函数名称：
** 函数描述：字节序（大小端字节序）的转换short(u16)
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
void endian_swaps(void* data)
{
    uint8_t  temp = 0;
	uint8_t* bdata = (uint8_t*)data;
	
	temp = bdata[0];
	bdata[0] = bdata[1];       //最后一个和第一个对调
	bdata[1] = temp;
		
}
/*********************************************************************************************************
** 函数名称：
** 函数描述：字节序（大小端字节序）的转换(u32)
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
void endian_swapl(void* data)
{
    uint8_t i = 0, temp = 0;
	uint8_t* bdata = (uint8_t*)data;
	 
	for (i = 0; i < 2; ++i)  
	{
		temp = bdata[i];
		bdata[i] = bdata[4-i-1]; //最后一个和第一个对调
		bdata[4-i-1] = temp;
	}
}
