
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
struct  Contactor_status_bits   st_KM_bit;

struct  Bat_data      st_bat_data;  
struct  Batcore_data  st_batcore_data;

//extern  struct rt_messagequeue adc_rx_mq; 

/********************************************************************************************
函数申明
********************************************************************************************/
//static void fmod_adc_data_update(void);


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
	st_product_info.u8_bat_num = 0x08;           //电池检测数量  

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
	
   //fmod_adc_data_update( );      // 已经在硬件中断函数中读取了
	
	//........................计算SOC和SOH............................. 
	//......刚上电时对电池进行开路电压对SOC的调整，静止时间大于4小时,且此时充放电电流小于1A..........
	if(run_count < 1)
	{
		run_count = 1;
		get_rtc_second(&now_sec);
   
		if( (now_sec - un_prodinfo_rdata.st_data.u32_time >= 4 * 60 *60) && 
			(st_bat_data.fl_bat_chI + st_bat_data.fl_bat_dischI <= 1 ))
		{
			fmod_open_volt_adj_soc( );
		}
	}
	
	fmod_updata_soh( ); //在浮充状态时会更新SOH	
	fmod_updata_soc( );   
} 

/** * @ : **********************************************************************
 * @name: 逻辑数据的赋值
 * @describe:逻辑数据的赋值 获取ADC采样值，使用硬件定时器采样，不需要使用消息队列
 * @param : 
 * @return: 
 * @  : ************************************************************************/
// static void fmod_get_adc_data_update(void)
// {	
	 
//     // uint16_t re_buf[ADC_RE_LEN] = {0};

// 	// if ( rt_mq_recv(&adc_rx_mq, &re_buf, sizeof(re_buf), 5) == RT_EOK )   //超时时间5ms 单位为ms
// 	// {
//     //  st_bat_data.fl_bat_chI   =   re_buf[0] * 0.01f; 
// 	// 	   st_bat_data.fl_bat_dischI  = re_buf[1] * 0.01f; 
// 	// }	
// }

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
/*********************************************************************************************************
** 函数名称：
** 函数描述：继电器控制
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
void fmod_relay_control()
{
	

	
	
	int power_status=POWER_STATUS_VALUE;
	
	if(power_status==1)
	{
		K7_START_PIN_ON;
		K1_START_PIN_ON;
		K3_START_PIN_OFF;
		K2_START_PIN_OFF;  
		
	}
	else
	{
		K7_START_PIN_ON;
		K1_START_PIN_OFF;
		K3_START_PIN_ON;
		K2_START_PIN_ON;  
	}


}
