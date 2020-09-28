#ifndef _THREAD_INT_HWTIMER_H_
#define _THREAD_INT_HWTIMER_H_

//..........................头文件.......................................
#include <rtthread.h>
#include <rtdevice.h>
#include "fmod_data_process.h"
#include "bsp_adc.h"




#define  TIMEBASE_10MS        1
#define  TIMEBASE_20MS        2
#define  TIMEBASE_50MS        5
#define  TIMEBASE_100MS       10
#define  TIMEBASE_200MS       20
#define  TIMEBASE_500MS       50
#define  TIMEBASE_1S        100
#define  TIMEBASE_2S        200
#define  TIMEBASE_5S        500
#define  TIMEBASE_10S       1000
#define  TIMEBASE_20S       2000
#define  TIMEBASE_30S       3000
#define  TIMEBASE_1MIN      6000
#define  TIMEBASE_2MIN      12000
#define  TIMEBASE_5MIN      30000
#define  TIMEBASE_10MIN     60000


#define  MOV_FILT_SIZE_MAX       100	// 滑动滤波器的小缓冲区大
#define  MOV_FILT_SIZE           20	    // 滑动滤波器实际长度

#define  bat_Iin_KP              1.20F     //输入电流数模转换的比例值
#define  bat_Iout1_KP            4.00F      //输出电流1数模转换的比例值
#define  bat_Iout2_KP            4.00F      //输出电流2采样数模转换的比例值
#define  bat_Iout3_KP            1.2F       //输出电流3数模转换的比例值
#define  bat_Vin_KP              10.00F     //输入电压采样数模转换的比例值
#define  batcell_Vin_KP          5.0F     //单体电池输入电压采样数模转换的比例值
#define  batcell_Temp_KP         1.00F     //单体电池温度采样数模转换的比例值

#define Integral_time  0.0001F          //积分时间 0.36s 采样一次 0.36s = 0.0001h
struct  Movefilter     // 滑动滤波器
{
	uint32_t  u32_BufferSum;				 // 滑动滤波总和,初始化必须清0
	uint16_t  u16_Buffer[MOV_FILT_SIZE_MAX]; // 滑动滤波缓冲区,	初始化必须清0
	uint16_t  u16_removeP;					 // 数据移出指针,	初始化必须清0
	uint16_t  u16_get_value;				 // 每次实际采样值,待移入队列
	uint16_t  u16_avg_value;			     // 滑动滤波平均值,	初始化必须清0
	uint16_t  u16_LPFilter;					 // 低通值滤波值,	初始化必须清0
}; 

//.............................................模拟量数据采样............................................



//..........................函数申明.......................................
rt_err_t hwtimeout_cb(rt_device_t dev, rt_size_t size);

#endif

