/********************************************************************************************
头文件
********************************************************************************************/
#include "thread_uart_rs485.h"
/********************************************************************************************
宏定义
********************************************************************************************/

#define  RE_UART5_THREAD_STACK_SZ        512         // 线程栈大小，单位是字节。
#define  RE_UART5_THREAD_PRIO            18          // 优先级 范围是从 0 ～ 255，数值越小优先级越高，0 代表最高优先级
#define  RE_UART5_THREAD_TIMESLICE       50          // 时间片大小：调用一次调度能够运行的最大时间长度

/********************************************************************************************
变量定义
********************************************************************************************/

static struct rt_thread re_uart5_thread;  
static rt_uint8_t re_uart5_thread_stack[RE_UART5_THREAD_STACK_SZ];

struct rt_messagequeue uart5_se_mq;      //UART5接收端口的消息队列，将给队列发送给其他线程
rt_uint8_t    uart5_se_mq_pool[320];     //发送消息队列的内存大小
uint8_t   outbuf6[UART5_SE_LEN ]={0x00}; //结收的数据的缓冲区
struct rt_messagequeue uart5_rx_mq;      //UART5接收端口的消息队列，将给队列发送给其他线程
rt_uint8_t    uart5_rx_mq_pool[100];     //UART5接收端消息队列的内存大小
uint8_t   inbuf6[UART5_RE_LEN ]={0x00};  //UART5接收端的数据的缓冲区

uint8_t u8_se_uart5_data[UART5_SE_LEN] = {0x00};  //发送数据 
uint8_t u8_re_uart5_data[UART5_RE_LEN] = {0x00};  //接收数据 

uint8_t uart5_re_flag = 0; //uart5接收到数据标志
/********************************************************************************************
函数申明
********************************************************************************************/
static void uart5_se_data(void);
static void uart5_se_update(void);
static void uart5_re_data(void);
static void uart5_variable_init(void);

/*********************************************************************************************************
** 函数名称：
** 函数描述：uart5接收的显示线程入口
** 输入参数：数码管要显示的内容
** 返回值  ：无
*********************************************************************************************************/
static void thread_entry_re_uart5(void *param)
{
	basic_uart5_init(BAUD_RATE_115200);
	uart5_variable_init( );
	rt_thread_mdelay(200); 
    while(1)
    { 
		//.......................阻断接收UART5板卡的数据...................
        uart5_re_data( ); 
		if(uart5_re_flag == 1)
		{   
			rt_thread_mdelay(500);  
			//.........................发送通讯数据...............................		
			uart5_se_data( );  
			uart5_re_flag = 0;
			rt_thread_mdelay(1000); 
		}		
	}
}
/********************************************************************************************
** 函数名称：     
** 函数描述：uart5接收线程的初始化
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
int thread_init_re_uart5(void)
{ 

  /* 初始化消息队列 */
	rt_mq_init( &uart5_rx_mq,
				"uart5_rx_mq",               
				uart5_rx_mq_pool,               /* 内存池指向msg_pool */
				UART5_RE_LEN,                /* 每个消息的大小是 1 字节 */
				sizeof(uart5_rx_mq_pool),       /* 内存池的大小是mq_pool的大小 */
				RT_IPC_FLAG_FIFO);           /* 如果有多个线程等待，按照先来先得到的方法分配消息 */

	rt_mq_init( &uart5_se_mq,
				"uart5_se_mq",               
				uart5_se_mq_pool,               /* 内存池指向msg_pool */
				UART5_SE_LEN,                   /* 每个消息的大小是 1 字节 */
				sizeof(uart5_se_mq_pool),       /* 内存池的大小是mq_pool的大小 */
				RT_IPC_FLAG_FIFO);              /* 如果有多个线程等待，按照先来先得到的方法分配消息 */
  
				// .....................................静态线程初始化.......................................
    rt_thread_init( &re_uart5_thread,                                   // 线程结构体
                    "re_uart5",                                         // 线程名称
                    thread_entry_re_uart5, RT_NULL,                     // 线程入口函数名称和参数
                    re_uart5_thread_stack, RE_UART5_THREAD_STACK_SZ,  // 栈的起始位置和大小 
                    RE_UART5_THREAD_PRIO,  RE_UART5_THREAD_TIMESLICE); // 线程优先级，时间片大小
    
    //..................................... 启动线程 ，进入线程入口函数...........................
    rt_thread_startup(&re_uart5_thread);
	return  0;     
}
INIT_APP_EXPORT(thread_init_re_uart5);	

/********************************************************************************************
** 函数名称：     
** 函数描述：UART5发送的数据
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
static void uart5_se_data(void)
{
	uint8_t i = 0;
	
	
	uart5_se_update( );  //发送数据更新
//	for(i=0;i <132; i++ )
//	{	   
//	    u8_se_uart5_data[i] = i%6;	
//	}
   
	//.........................数据的发送..........................
	bsp_uart5_send_data(u8_se_uart5_data, 132); 
	
}

/********************************************************************************************
** 函数名称：     
** 函数描述：UART5发送的数据
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
static void uart5_se_update(void)
{
    uint8_t re_buf[UART5_SE_LEN] = {0};
    uint8_t i = 0;
	
	if( rt_mq_recv(&uart5_se_mq, &re_buf, sizeof(re_buf), 5) == RT_EOK )   //超时时间5ms 单位为ms
	{
		for(i = 0; i < UART5_SE_LEN; i++)
        {
			u8_se_uart5_data[i] = re_buf[i];
		}	
	}
 
}

/*********************************************************************************************************
** 函数名称：MVB数据的初始化
** 函数描述：
** 输入参数：
** 返回值  ：
*********************************************************************************************************///
static void uart5_variable_init(void)
{  
	u8_se_uart5_data[0] = 0xAE;
	u8_se_uart5_data[1] = 0xA1;
	u8_se_uart5_data[UART5_SE_LEN-2] = 0XEF;

	u8_re_uart5_data[0] = 0xAA;
	u8_re_uart5_data[1] = 0xA1;	
    u8_re_uart5_data[UART5_RE_LEN-2] = 0XEE; 
}	

/********************************************************************************************
** 函数名称：     
** 函数描述：UART5接收UART5板卡的数据
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
static void uart5_re_data(void)
{   
    static  uint16_t  u16_uart5_rx_count = 0;
    uint8_t re_buf = 0; //单次从终端接收的单个数据
	uint8_t uart5_se_buf[UART5_RE_LEN];    //需要发送的消息队列的数据
    uint8_t i = 0;
 
	//..............................阻塞接收数据................................
	while (rt_device_read(serial_uart5, -1, &re_buf, 1) != 1)
	{
		rt_sem_take(&rx_sem5, RT_WAITING_FOREVER);
	}
	
	inbuf6[u16_uart5_rx_count] = re_buf;       
    //............. 接收第一个数据判断起始为..................
    if( inbuf6[0] != u8_re_uart5_data[0])         
    {
        u16_uart5_rx_count = 0;
    }
	else 
    {
        u16_uart5_rx_count++;
    }	
	
    //............. 判断第二个数据是否正确..................	
	if( (!(inbuf6[1] == u8_re_uart5_data[1]) )
		&& (u16_uart5_rx_count >= 2))   //接收到第二个数据时查看是否正确   
    {
        u16_uart5_rx_count = 0;
		for(i = 0; i < UART5_RE_LEN ; i++) //清0接收缓存
		{ 
			inbuf6[i]= 0;
		}	
    }
	
    //...................2 接收数据...................
    if( (inbuf6[1] == u8_re_uart5_data[1]) && (inbuf6[UART5_RE_LEN-2] == u8_re_uart5_data[UART5_RE_LEN-2])
	    && (u16_uart5_rx_count == UART5_RE_LEN)  ) 
	{
		u16_uart5_rx_count = 0;
		uart5_re_flag = 1;
		
		for(i = 0; i < UART5_RE_LEN ; i++)
		{
			uart5_se_buf[i] = inbuf6[i];
		}	

		//.........................发送消息队列.......................................
		if (rt_mq_send(&uart5_rx_mq, &uart5_se_buf, sizeof(uart5_se_buf)) == -RT_ERROR)
		{
			rt_kprintf("uart5_rx_mq send err!\n");
		}
	}  
}

/*********************************************************************************************************
** 函数名称：BCC(异或)校验
** 函数描述：计算校验码 和00异或其值不变
** 输入参数：
** 返回值  ：
*********************************************************************************************************///
uint8_t BCC_XOR_check(uint8_t *data,  uint8_t len)
{  
    uint8_t i = 0;
	uint8_t value = 0;
	
	value = 0x00;
	
	for(i = 0; i < len; i++) 
	{   
		value ^= data[i];		
	}
	return 	value; 	
}



