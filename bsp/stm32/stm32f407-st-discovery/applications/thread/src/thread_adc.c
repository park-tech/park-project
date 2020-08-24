// /******************************************************************************************
// ** 头文件
// *******************************************************************************************/
// #include "thread_adc.h"

// /******************************************************************************************
// ** 宏定义
// *******************************************************************************************/
// #define  ADC_THREAD_STACK_SZ        512         // 线程栈大小，单位是字节。
// #define  ADC_THREAD_PRIO            19          // 优先级 范围是从 0 ～ 255，数值越小优先级越高，0 代表最高优先级
// #define  ADC_THREAD_TIMESLICE       100         // 时间片大小：调用一次调度能够运行的最大时间长度

// /********************************************************************************************
// ** 定义变量
// ********************************************************************************************/
// static struct rt_thread adc_thread;  
// static rt_uint8_t  adc_thread_stack[ADC_THREAD_STACK_SZ];

// struct rt_messagequeue adc_rx_mq;      //发送消息队列
// rt_uint16_t    adc_mq_pool[100];       //发送消息队列的内存大小
// struct	Adc_data  st_adc_data;

// //.............................................函数申明...................................
// void     adc_sample(void);

// static void adc_sample_single_batV(void);
// static void adc_sample_single_battemp(void);
// static void set_bat_volt_channel(uint8_t ch);
// static void set_bat_temp_channel(uint8_t ch);

// /**************************************************************************************************
// ** 函数名称：
// ** 函数描述：接收canopen的显示线程入口
// ** 输入参数：数码管要显示的内容
// ** 返回值  ：无
// ***************************************************************************************************/
// static void thread_entry_adc(void *param)
// {
//     while(1)
//     {
//          adc_sample( );

//          rt_thread_mdelay(100);
//     }

// }
// /********************************************************************************************
// ** 函数名称：     
// ** 函数描述：线程的初始化
// ** 输入参数：无
// ** 返回值  ：    无
// ********************************************************************************************/
// int thread_init_adc(void)
// {
    
// 	/* 初始化消息队列 */
// 	rt_mq_init( &adc_rx_mq,
// 				"adc_rx_mq",               
// 				adc_mq_pool,               /* 内存池指向msg_pool */
// 				ADC_RE_LEN * 2,                /* 每个消息的大小是 1 字节 */
// 				sizeof(adc_mq_pool),       /* 内存池的大小是mq_pool的大小 */
// 				RT_IPC_FLAG_FIFO);           /* 如果有多个线程等待，按照先来先得到的方法分配消息 */

	
// 	// .....................................静态线程初始化.......................................
//     rt_thread_init( &adc_thread,                                   // 线程结构体
//                     "adc",                                         // 线程名称
//                     thread_entry_adc, RT_NULL,                     // 线程入口函数名称和参数
//                     adc_thread_stack, ADC_THREAD_STACK_SZ,  // 栈的起始位置和大小 
//                     ADC_THREAD_PRIO, ADC_THREAD_TIMESLICE); // 线程优先级，时间片大小
//      //..................................... 启动线程 ，进入线程入口函数...........................
      
// 	rt_thread_startup(&adc_thread); 
//     return 0;    
// }
// //INIT_COMPONENT_EXPORT(thread_init_adc);	

// /*****************************************************************************************
// ** 函数名称：
// ** 函数描述：ADC采样,并发送消息队列
// ** 输入参数：
// ** 返回值  ：无
// ******************************************************************************************/
// void  adc_sample(void)
// {   
// 	uint16_t   send_buf[ADC_RE_LEN]; 
	
// 	uint16_t adc_value = 0; 

//     bat_chI.u16_get_value = bsp_adchannel_bat_chI( ); //1000倍
//     bat_chI.u16_avg_value = Movefilter( &bat_chI, MOV_FILT_SIZE) * bat_chI_KP; //100倍

//     bat_dischI.u16_get_value = bsp_adchannel_bat_dischI( );
//     bat_dischI.u16_avg_value = Movefilter( &bat_dischI, MOV_FILT_SIZE) * bat_dischI_KP;

// 	send_buf[0] = bat_chI.u16_avg_value;
// 	send_buf[1] = bat_dischI.u16_avg_value;
	
// 	//.........................发送消息队列.......................................
// 	if (rt_mq_send(&adc_rx_mq, &send_buf, sizeof(send_buf)) != RT_EOK)
// 	{
// 		rt_kprintf("adc_rx_mq send err!\n");
// 	}
// }

// **********************************************************************************
//  * @funcation  :
//  * @author     :
//  * @date       : 
//  * @brief      :滑动滤波函数
//  * @attention  :Size:滑动滤波队列大小,Size 必须 <= MOV_FILT_SIZE_MAX
//  *             
//  **********************************************************************************
//  */
// static int16 Movefilter(tagMoveFilter *MovAvgVar, Uint16 Size)
// {
//     float kp = 0;
	
// 	if (Size > MOV_FILT_SIZE_MAX ) /* 缓冲器越界处理 */
//     {
//         Size = MOV_FILT_SIZE_MAX;
//     }

//     kp = 1.0 / Size; 
//     //..........................滑动滤波总和........................
//     MovAvgVar->u32_BufferSum = MovAvgVar->u32_BufferSum - MovAvgVar->u16_Buffer[MovAvgVar->u16_removeP] + MovAvgVar->u16_get_value; /* 减去即将出列值,加入当前采样值 */
    
//     //当前采样数据送入队列滑动滤波缓冲区，MovAvgVar->u16_removeP++为数据移出的指针
//     MovAvgVar->u16_Buffer[MovAvgVar->u16_removeP++] = MovAvgVar->u16_get_value; /* 当前采样数据送入队列 */
//     if (MovAvgVar->u16_removeP >= Size)   //最新的采样值存放在0-size中的任何一个位置，而不是一直在最后的位置
//     {
//         MovAvgVar->u16_removeP -= Size;
//     }

//     //........................ 返回滑动滤波后的平均值 .........................
//     return (MovAvgVar->u32_BufferSum * kp); 
// }
// /******************************************************************************************
//     end file
// *******************************************************************************************/
