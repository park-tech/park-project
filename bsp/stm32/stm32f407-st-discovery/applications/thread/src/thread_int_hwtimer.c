/********************************************************************************************
头文件
********************************************************************************************/
#include "thread_int_hwtimer.h"
#include "bsp_gpio.h"
#include <math.h>
/********************************************************************************************
宏定义
********************************************************************************************/
uint32_t u32_timer_tick = 0;
uint32_t ADCchannelindex = 0;
/********************************************************************************************
变量定义
********************************************************************************************/
struct Movefilter Iin;			  //输入电流
struct Movefilter Iout1;		  //负载电流1
struct Movefilter Iout2;		  //负载电流2
struct Movefilter Iout3;		  //负载电流3
struct Movefilter Vin;			  //输入电压
struct Movefilter cell_temp[9];	  //单体电池温度
struct Movefilter cell_vol_b0[9]; //单体电池电压——正
struct Movefilter cell_vol_b1[9]; //单体电池电压——负

/********************************************************************************************
函数申明
********************************************************************************************/
static void timebase_count(void);
static void adc_sample(void);
static void cell_adc_sample(void);
static void bat_Qnow_updata(void);
static int16_t Movefilter(struct Movefilter *MovAvgVar, uint16_t Size);
static float fmod_bat_effect(float bat_i, float tempb);
static void fmod_sbox_chooseVolt(int bt_index);
static void fmod_sbox_chooseTemp(int bt_index);
static void fmod_sbox_choosePassiveEquilibrium(int bt_index);
static double fmod_sbox_Temp_Convert(uint16_t bt_Temp_Volt);

/**************************************************************************************************
** 函数名称：
** 函数描述：硬件定时器中断函数
** 输入参数：
** 返回值  ：无
***************************************************************************************************/
rt_err_t hwtimeout_cb(rt_device_t dev, rt_size_t size)
{

	timebase_count();

	//fmod_sbox_chooseVolt(1);
	//fmod_sbox_chooseTemp(1);
	//.......................adc采样.........................
	if (timer_flag.flag_10ms)
	{
		timer_flag.flag_10ms = 0;
		adc_sample();
		//.......................单体电池adc采样.........................
		cell_adc_sample();
		//.......................当前容量累计的计算.........................
		bat_Qnow_updata();
	}
	return 0;
}
/**************************************************************************************************
** 函数名称：时间标志计数
** 函数描述：
** 输入参数：
** 返回值  ：无
***************************************************************************************************/
static void timebase_count(void)
{
	u32_timer_tick++;

	if ((u32_timer_tick % TIMEBASE_10MS) == 0)
		timer_flag.flag_10ms = 1;
	if ((u32_timer_tick % TIMEBASE_20MS) == 0)
		timer_flag.flag_20ms = 1;
	if ((u32_timer_tick % TIMEBASE_50MS) == 0)
		timer_flag.flag_50ms = 1;
	if ((u32_timer_tick % TIMEBASE_100MS) == 0)
		timer_flag.flag_100ms = 1;
	if ((u32_timer_tick % TIMEBASE_200MS) == 0)
		timer_flag.flag_200ms = 1;
	if ((u32_timer_tick % TIMEBASE_500MS) == 0)
		timer_flag.flag_500ms = 1;
	if ((u32_timer_tick % TIMEBASE_1S) == 0)
		timer_flag.flag_1s = 1;
	if ((u32_timer_tick % TIMEBASE_2S) == 0)
		timer_flag.flag_2s = 1;
	if ((u32_timer_tick % TIMEBASE_5S) == 0)
		timer_flag.flag_5s = 1;
	if ((u32_timer_tick % TIMEBASE_10S) == 0)
		timer_flag.flag_10s = 1;
	if ((u32_timer_tick % TIMEBASE_20S) == 0)
		timer_flag.flag_20s = 1;
	if ((u32_timer_tick % TIMEBASE_30S) == 0)
		timer_flag.flag_30s = 1;
	if ((u32_timer_tick % TIMEBASE_1MIN) == 0)
		timer_flag.flag_1min = 1;
	if ((u32_timer_tick % TIMEBASE_2MIN) == 0)
		timer_flag.flag_2min = 1;
	if ((u32_timer_tick % TIMEBASE_5MIN) == 0)
		timer_flag.flag_5min = 1;
	if ((u32_timer_tick % TIMEBASE_10MIN) == 0)
		timer_flag.flag_5min = 1;

	if (u32_timer_tick > TIMEBASE_10MIN)
		u32_timer_tick = 0;
}
/*****************************************************************************************
** 函数名称：
** 函数描述：ADC采样,并发送消息队列
** 输入参数：
** 返回值  ：无
******************************************************************************************/
static void adc_sample(void)
{   
	uint16_t u16_bat_volt=0;
	uint16_t u16_bat_min_volt_temp=st_batcore_data.u16_batcore_volt[0];   //电池最低电压初始化
	uint16_t u16_bat_max_volt_temp=st_batcore_data.u16_batcore_volt[0];   //电池最高电压初始化
	uint16_t fl_bat_min_temp_temp=st_batcore_data.u16_batcore_temp[0];//电池最低温度初始化
	uint16_t fl_bat_max_temp_temp=st_batcore_data.u16_batcore_temp[0];//电池最高温度初始化
	
	
	
	Iin.u16_get_value = bsp_adchannel_Iin();
	Iin.u16_avg_value = Movefilter(&Iin, MOV_FILT_SIZE) * bat_Iin_KP;

	Iout1.u16_get_value = bsp_adchannel_Iout1();
	Iout1.u16_avg_value = Movefilter(&Iout1, MOV_FILT_SIZE) * bat_Iout1_KP;

	Iout2.u16_get_value = bsp_adchannel_Iout2();
	Iout2.u16_avg_value = Movefilter(&Iout2, MOV_FILT_SIZE) * bat_Iout2_KP;

	Iout3.u16_get_value = bsp_adchannel_Iout3();
	Iout3.u16_avg_value = Movefilter(&Iout3, MOV_FILT_SIZE) * bat_Iout3_KP;

	Vin.u16_get_value = bsp_adchannel_Vin();
	Vin.u16_avg_value = Movefilter(&Vin, MOV_FILT_SIZE) * bat_Vin_KP;

	st_bat_data.fl_bat_chI = Iin.u16_avg_value * 0.1f;
	st_bat_data.fl_bat_dischI = (Iout1.u16_avg_value + Iout2.u16_avg_value + Iout3.u16_avg_value) * 0.1f;

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
		
		//电池最高温度
		if(fl_bat_max_temp_temp<st_batcore_data.u16_batcore_temp[i])
		{
			fl_bat_max_temp_temp=st_batcore_data.u16_batcore_temp[i];
		}
		
		//电池最低温度
		if(fl_bat_min_temp_temp>st_batcore_data.u16_batcore_temp[i])
		{
			fl_bat_min_temp_temp=st_batcore_data.u16_batcore_temp[i];
		}
	}
	
	
	st_bat_data.fl_bat_volt =u16_bat_volt;
	st_bat_data.u16_bat_max_volt=u16_bat_max_volt_temp;
	st_bat_data.u16_bat_min_volt=u16_bat_min_volt_temp;
	st_bat_data.fl_bat_max_temp=fl_bat_max_temp_temp;
	st_bat_data.fl_bat_min_temp=fl_bat_min_temp_temp;
		
	st_bat_data.u16_bat_avg_volt=st_bat_data.fl_bat_volt/TEST_BAT_NUM;
}
/*****************************************************************************************
** 函数名称：
** 函数描述：单体电池ADC采样,并发送消息队列
** 输入参数：
** 返回值  ：无
******************************************************************************************/
static void cell_adc_sample(void)
{
	
	//ADCchannelindex=7;
	cell_temp[ADCchannelindex].u16_get_value = bsp_adchannel_cell_temp();
	cell_temp[ADCchannelindex].u16_avg_value = Movefilter(&cell_temp[ADCchannelindex], MOV_FILT_SIZE) * batcell_Temp_KP;

	cell_vol_b0[ADCchannelindex].u16_get_value = bsp_adchannel_cell_vol_b0();
	cell_vol_b0[ADCchannelindex].u16_avg_value = Movefilter(&cell_vol_b0[ADCchannelindex], MOV_FILT_SIZE) * batcell_Vin_KP;

	cell_vol_b1[ADCchannelindex].u16_get_value = bsp_adchannel_cell_vol_b1();
	cell_vol_b1[ADCchannelindex].u16_avg_value = Movefilter(&cell_vol_b1[ADCchannelindex], MOV_FILT_SIZE) * batcell_Vin_KP;

	if (ADCchannelindex % 2==0)
	{
		st_batcore_data.u16_batcore_volt[ADCchannelindex] = cell_vol_b1[ADCchannelindex].u16_avg_value*0.01f ;
	}
	else
	{
		st_batcore_data.u16_batcore_volt[ADCchannelindex] = cell_vol_b0[ADCchannelindex].u16_avg_value*0.01f ;
	}
	st_batcore_data.u16_batcore_temp[ADCchannelindex] = fmod_sbox_Temp_Convert(cell_temp[ADCchannelindex].u16_avg_value);

	ADCchannelindex++;
	
	if (ADCchannelindex == 9)
	{
		ADCchannelindex = 0;
	}
	
	fmod_sbox_chooseVolt(ADCchannelindex + 1);
	fmod_sbox_chooseTemp(ADCchannelindex + 1);
}

/**********************************************************************************
 * @funcation  :
 * @author     :
 * @date       : 
 * @brief      :滑动滤波函数
 * @attention  :Size:滑动滤波队列大小,Size 必须 <= MOV_FILT_SIZE_MAX
 *             
 **********************************************************************************
 */
static int16_t Movefilter(struct Movefilter *MovAvgVar, uint16_t Size)
{
	float kp = 0;

	if (Size > MOV_FILT_SIZE_MAX) /* 缓冲器越界处理 */
	{
		Size = MOV_FILT_SIZE_MAX;
	}

	kp = 1.0 / Size;
	//..........................滑动滤波总和........................
	MovAvgVar->u32_BufferSum = MovAvgVar->u32_BufferSum - MovAvgVar->u16_Buffer[MovAvgVar->u16_removeP] + MovAvgVar->u16_get_value; /* 减去即将出列值,加入当前采样值 */

	//当前采样数据送入队列滑动滤波缓冲区，MovAvgVar->u16_removeP++为数据移出的指针
	MovAvgVar->u16_Buffer[MovAvgVar->u16_removeP++] = MovAvgVar->u16_get_value; /* 当前采样数据送入队列 */
	if (MovAvgVar->u16_removeP >= Size)											//最新的采样值存放在0-size中的任何一个位置，而不是一直在最后的位置
	{
		MovAvgVar->u16_removeP -= Size;
	}

	//........................ 返回滑动滤波后的平均值 .........................
	return (MovAvgVar->u32_BufferSum * kp);
}

/******************************************************************************************
** 函数名称：当前容量的计算
** 函数描述：
** 输入参数：
** 返回值  ：无
******************************************************************************************/
static void bat_Qnow_updata(void)
{
	static float fl_bat_all_I = 0;	   //当前总电流
	static float fl_bat_lastall_I = 0; //上次总电流
	double Q_unit = 0;

	fl_bat_all_I = st_bat_data.fl_bat_chI - st_bat_data.fl_bat_dischI;																					//当前总电流
	Q_unit = ((double)(fl_bat_all_I + fl_bat_lastall_I) / 2) * Integral_time * fmod_bat_effect(st_bat_data.fl_bat_dischI, st_bat_data.fl_bat_max_temp); //本次积分的容量

	st_bat_data.fl_bat_Qnow = st_bat_data.fl_bat_Qnow + Q_unit; //当前容量

	if (st_bat_data.fl_bat_Qnow >= Bat_Qb * 1.1)
	{
		st_bat_data.fl_bat_Qnow = Bat_Qb * 1.1;
	}

	//printf("Q_unit=%.7f,  st_adc_data.fl_bat_Qnow=%.4f\n", Q_unit, st_bat_data.fl_bat_Qnow);
	fl_bat_lastall_I = fl_bat_all_I; //上次总电流
}

/******************************************************************************************
** 函数描述：温度和放电电流系数 （丰日）
** 输入参数：
** 返回值  ：
*******************************************************************************************/
static float fmod_bat_effect(float bat_i, float tempb)
{
	float current_k = 0, temp_k = 0;
	float bat_IC; //电流的倍率

	float bat_k = 0; //整体电池的比例系数

	//................不同放电电流对应的与0.2C时放电的系数值................
	bat_IC = bat_i / Bat_Qb;

	if (bat_IC <= 0.2f) //小于0.2C放电，或充电时
	{
		current_k = 1;
	}
	if (bat_IC > 0.2f && bat_IC <= 0.5f)
		current_k = 100 / (-46.67f * bat_IC + 109.33f);
	if (bat_IC > 0.5f && bat_IC <= 1)
		current_k = 100 / (-21 * bat_IC + 96.5f);
	if (bat_IC > 1)
		current_k = 100 / (-11 * bat_IC + 86.5f);

	if (tempb <= -40)
		temp_k = 100 / 37.5f;
	if ((tempb > -40) && (tempb <= -15))
		temp_k = 100 / (1.3f * tempb + 89.5f); //与25度时100容量为100时对应的比值
	if ((tempb > -15) && (tempb <= 0))
		temp_k = 100 / (tempb + 85);
	if ((tempb > 0) && (tempb <= 15))
		temp_k = 100 / (0.66667f * tempb + 85);
	if ((tempb > 15) && (tempb <= 30))
		temp_k = 100 / (0.5333f * tempb + 87);
	if ((tempb > 30) && (tempb <= 45))
		temp_k = 100 / (0.6f * tempb + 85);
	if (tempb > 45)
		temp_k = 100 / 109;
	bat_k = current_k * temp_k;

	//rt_printf("current_k=%.4f temp_k=%.4f bat_k=%.4f\n", current_k,temp_k,bat_k);
	return bat_k;
}

static void fmod_sbox_chooseVolt(int bt_index)
{
	switch (bt_index)
	{
	case 1:
	{
		MCU_38E1_PIN_OFF;
		MCU_38A1_PIN_OFF;
		MCU_38B1_PIN_ON;
		MCU_38C1_PIN_ON;

		MCU_38E2_PIN_OFF;
		MCU_38A2_PIN_OFF;
		MCU_38B2_PIN_ON;
		MCU_38C2_PIN_ON;
	}
	break;
	case 2:
	{
		MCU_38E1_PIN_OFF;
		MCU_38A1_PIN_ON;
		MCU_38B1_PIN_OFF;
		MCU_38C1_PIN_ON;

		MCU_38E2_PIN_OFF;
		MCU_38A2_PIN_OFF;
		MCU_38B2_PIN_ON;
		MCU_38C2_PIN_ON;
	}
	break;
	case 3:
	{
		MCU_38E1_PIN_OFF;
		MCU_38A1_PIN_ON;
		MCU_38B1_PIN_OFF;
		MCU_38C1_PIN_ON;

		MCU_38E2_PIN_OFF;
		MCU_38A2_PIN_ON;

		MCU_38B2_PIN_OFF;
		MCU_38C2_PIN_ON;
	}
	break;
	case 4:
	{
		MCU_38E1_PIN_OFF;
		MCU_38A1_PIN_OFF;
		MCU_38B1_PIN_OFF;
		MCU_38C1_PIN_ON;

		MCU_38E2_PIN_OFF;
		MCU_38A2_PIN_ON;
		MCU_38B2_PIN_OFF;
		MCU_38C2_PIN_ON;
	}
	break;
	case 5:
	{
		MCU_38E1_PIN_OFF;
		MCU_38A1_PIN_OFF;
		MCU_38B1_PIN_OFF;
		MCU_38C1_PIN_ON;

		MCU_38E2_PIN_OFF;
		MCU_38A2_PIN_OFF;
		MCU_38B2_PIN_OFF;
		MCU_38C2_PIN_ON;
	}
	break;
	case 6:
	{
		MCU_38E1_PIN_OFF;
		MCU_38A1_PIN_ON;
		MCU_38B1_PIN_ON;
		MCU_38C1_PIN_OFF;

		MCU_38E2_PIN_OFF;
		MCU_38A2_PIN_OFF;
		MCU_38B2_PIN_OFF;
		MCU_38C2_PIN_ON;
	}
	break;
	case 7:
	{
		MCU_38E1_PIN_OFF;
		MCU_38A1_PIN_ON;
		MCU_38B1_PIN_ON;
		MCU_38C1_PIN_OFF;

		MCU_38E2_PIN_OFF;
		MCU_38A2_PIN_ON;
		MCU_38B2_PIN_ON;
		MCU_38C2_PIN_OFF;
	}
	break;
	case 8:
	{
		MCU_38E1_PIN_OFF;
		MCU_38A1_PIN_OFF;
		MCU_38B1_PIN_ON;
		MCU_38C1_PIN_OFF;

		MCU_38E2_PIN_OFF;
		MCU_38A2_PIN_ON;
		MCU_38B2_PIN_ON;
		MCU_38C2_PIN_OFF;
	}
	break;
	case 9:
	{
		MCU_38E1_PIN_OFF;
		MCU_38A1_PIN_OFF;
		MCU_38B1_PIN_ON;
		MCU_38C1_PIN_OFF;

		MCU_38E2_PIN_OFF;
		MCU_38A2_PIN_OFF;
		MCU_38B2_PIN_ON;
		MCU_38C2_PIN_OFF;
	}
	break;
	case 10:
	{
		MCU_38E1_PIN_OFF;
		MCU_38A1_PIN_ON;
		MCU_38B1_PIN_OFF;
		MCU_38C1_PIN_OFF;

		MCU_38E2_PIN_OFF;
		MCU_38A2_PIN_OFF;
		MCU_38B2_PIN_ON;
		MCU_38C2_PIN_OFF;
	}
	break;
	case 11:
	{
		MCU_38E1_PIN_OFF;
		MCU_38A1_PIN_ON;
		MCU_38B1_PIN_OFF;
		MCU_38C1_PIN_OFF;

		MCU_38E2_PIN_OFF;
		MCU_38A2_PIN_ON;
		MCU_38B2_PIN_OFF;
		MCU_38C2_PIN_OFF;
	}
	break;
	case 12:
	{
		MCU_38E1_PIN_OFF;
		MCU_38A1_PIN_OFF;
		MCU_38B1_PIN_OFF;
		MCU_38C1_PIN_OFF;

		MCU_38E2_PIN_OFF;
		MCU_38A2_PIN_ON;
		MCU_38B2_PIN_OFF;
		MCU_38C2_PIN_OFF;
	}
	break;
	default:
		break;
	}
}

static void fmod_sbox_chooseTemp(int bt_index)
{
	switch (bt_index)
	{
	case 1:
	{
		MCU_67E_PIN_OFF;
		MCU_67S0_PIN_ON;
		MCU_67S1_PIN_ON;
		MCU_67S2_PIN_ON;
		MCU_67S3_PIN_OFF;
	}
	break;
	case 2:
	{
		MCU_67E_PIN_OFF;
		MCU_67S0_PIN_OFF;
		MCU_67S1_PIN_ON;
		MCU_67S2_PIN_ON;
		MCU_67S3_PIN_OFF;
	}
	break;
	case 3:
	{
		MCU_67E_PIN_OFF;
		MCU_67S0_PIN_ON;
		MCU_67S1_PIN_OFF;
		MCU_67S2_PIN_ON;
		MCU_67S3_PIN_OFF;
	}
	break;
	case 4:
	{
		MCU_67E_PIN_OFF;
		MCU_67S0_PIN_OFF;
		MCU_67S1_PIN_OFF;
		MCU_67S2_PIN_ON;
		MCU_67S3_PIN_OFF;
	}
	break;
	case 5:
	{
		MCU_67E_PIN_OFF;
		MCU_67S0_PIN_ON;
		MCU_67S1_PIN_ON;
		MCU_67S2_PIN_OFF;
		MCU_67S3_PIN_OFF;
	}
	break;
	case 6:
	{
		MCU_67E_PIN_OFF;
		MCU_67S0_PIN_OFF;
		MCU_67S1_PIN_ON;
		MCU_67S2_PIN_OFF;
		MCU_67S3_PIN_OFF;
	}
	break;
	case 7:
	{
		MCU_67E_PIN_OFF;
		MCU_67S0_PIN_ON;
		MCU_67S1_PIN_OFF;
		MCU_67S2_PIN_OFF;
		MCU_67S3_PIN_OFF;
	}
	break;
	case 8:
	{
		MCU_67E_PIN_OFF;
		MCU_67S0_PIN_OFF;
		MCU_67S1_PIN_OFF;
		MCU_67S2_PIN_OFF;
		MCU_67S3_PIN_OFF;
	}
	break;
	case 9:
	{
		MCU_67E_PIN_OFF;
		MCU_67S0_PIN_OFF;
		MCU_67S1_PIN_OFF;
		MCU_67S2_PIN_OFF;
		MCU_67S3_PIN_ON;
	}
	break;
	case 10:
	{
		MCU_67E_PIN_OFF;
		MCU_67S0_PIN_ON;
		MCU_67S1_PIN_OFF;
		MCU_67S2_PIN_OFF;
		MCU_67S3_PIN_ON;
	}
	break;
	case 11:
	{
		MCU_67E_PIN_OFF;
		MCU_67S0_PIN_OFF;
		MCU_67S1_PIN_ON;
		MCU_67S2_PIN_OFF;
		MCU_67S3_PIN_ON;
	}
	break;
	case 12:
	{
		MCU_67E_PIN_OFF;
		MCU_67S0_PIN_ON;
		MCU_67S1_PIN_ON;
		MCU_67S2_PIN_OFF;
		MCU_67S3_PIN_ON;
	}
	break;
	default:
		break;
	}
}

static double fmod_sbox_Temp_Convert(uint16_t bt_Temp_Volt)
{
	double real_bt_Temp_Volt = bt_Temp_Volt / 1000.000;
	double R_NTC = (real_bt_Temp_Volt * 20000) / (3.3 - real_bt_Temp_Volt);
	double temp = 1 / (log(R_NTC / 10000) / 3960 + 1 / 298.15) - 273.15;
	double temp1 = ((temp+55 ) * 10);
	return temp1;
}
