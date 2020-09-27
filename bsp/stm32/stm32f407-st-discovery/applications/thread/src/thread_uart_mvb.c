/********************************************************************************************
头文件
********************************************************************************************/
#include "thread_uart_mvb.h"
/********************************************************************************************
宏定义
********************************************************************************************/
#define  SE_UART1_THREAD_STACK_SZ        512         // 线程栈大小，单位是字节。
#define  SE_UART1_THREAD_PRIO            22         // 优先级 范围是从 0 ～ 255，数值越小优先级越高，0 代表最高优先级
#define  SE_UART1_THREAD_TIMESLICE       15         // 时间片大小：调用一次调度能够运行的最大时间长度

#define  RE_UART1_THREAD_STACK_SZ        512         // 线程栈大小，单位是字节。
#define  RE_UART1_THREAD_PRIO            21          // 优先级 范围是从 0 ～ 255，数值越小优先级越高，0 代表最高优先级
#define  RE_UART1_THREAD_TIMESLICE       15         // 时间片大小：调用一次调度能够运行的最大时间长度

/********************************************************************************************
变量定义
********************************************************************************************/
static struct rt_thread se_uart1_thread;  
static rt_uint8_t se_uart1_thread_stack[SE_UART1_THREAD_STACK_SZ];
static struct rt_thread re_uart1_thread;  
static rt_uint8_t re_uart1_thread_stack[RE_UART1_THREAD_STACK_SZ];

struct rt_messagequeue uart1_se_mq;      //UART1接收端口的消息队列，将给队列发送给其他线程
rt_uint8_t    uart1_se_mq_pool[100];     //发送消息队列的内存大小
uint8_t   outbuf1[UART1_SE_LEN ]={0x00};  //结收的数据的缓冲区
struct rt_messagequeue uart1_rx_mq;      //UART1接收端口的消息队列，将给队列发送给其他线程
rt_uint8_t    uart1_rx_mq_pool[100];     //UART1接收端消息队列的内存大小
uint8_t   inbuf1[UART1_RE_LEN ]={0x00};  //UART1接收端的缓冲区
uint8_t u8_se_mvbprot1_data[MVB_SPORT1_LEN] = {0x00}; //源端口1（发送）数据
uint8_t u8_re_mvbprot1_data[MVB_DPORT1_LEN] = {0x00}; //宿端口1（接收）数据 


uint8_t u8_mvb_configure_flag = 0;
uint8_t u8_mvb_configure_command[250] = {0x00}; //MVB配置信息指令
uint8_t u8_reenable_mvb_command[8] = {0x00};        //读取MVB数据使能指令，只需使能一次就行

uint8_t u8_re_mvbboard_prot1_data [9 + MVB_DPORT1_LEN] = {0x00};   //接收MVB板卡发来的PROT1数据
uint8_t u8_se_mvbboard_prot1_data [10 + MVB_SPORT1_LEN] = {0x00};  //发送给MVB板卡的PROT1数据


const unsigned short crc_table[256] = { 0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
                                     0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
									 0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 
									 0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 
									 0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 
									 0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 
									 0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 
									 0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 
									 0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823, 
									 0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 
									 0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 
									 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, 
									 0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 
									 0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49, 
									 0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 
									 0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 
									 0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 
									 0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
									 0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 
									 0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 
									 0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 
									 0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 
									 0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
									 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634, 
									 0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 
									 0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3, 
									 0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 
									 0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 
									 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 
									 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1, 
									 0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 
									 0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0 };

/********************************************************************************************
函数申明
********************************************************************************************/
static void uart1_variable_init(void);
static void uart1_se_data(void);
static void uart1_se_update(void);
static void uart1_re_data(void);
static uint16_t get_mvb_crc(unsigned char *buf, int len) ;


/*********************************************************************************************************
** 函数名称：
** 函数描述：uart1发送的显示线程入口
** 输入参数：数码管要显示的内容
** 返回值  ：无
*********************************************************************************************************/
static void thread_entry_se_uart1(void *param)
{
   
    
	uart1_variable_init( );
    while(1)
    { 
		
		if(u8_mvb_configure_flag ==1)
		{  
			//.........................发送通讯数据...............................		
			uart1_se_data( );  	 
		}
		else
		{
			//.........................发送MVB配置指令...............................		
			bsp_uart1_send_data(u8_mvb_configure_command, sizeof(u8_mvb_configure_command));
		}
		rt_thread_mdelay(100);  
	}
}

/*********************************************************************************************************
** 函数名称：
** 函数描述：uart1接收的显示线程入口
** 输入参数：数码管要显示的内容
** 返回值  ：无
*********************************************************************************************************/
static void thread_entry_re_uart1(void *param)
{
    while(1)
    { 
		//.......................阻断接收UART1板卡的数据...................
        uart1_re_data( );  
		rt_thread_mdelay(100);  
	}
}
/********************************************************************************************
** 函数名称：     
** 函数描述：uart1发送线程的初始化
** 输入参数：无
** 返回值  ：无
********************************************************************************************/
int thread_init_se_uart1(void)
{ 

  /* 初始化消息队列 */
	rt_mq_init( &uart1_se_mq,
				"uart1_se_mq",               
				uart1_se_mq_pool,               /* 内存池指向msg_pool */
				UART1_SE_LEN,                   /* 每个消息的大小是 1 字节 */
				sizeof(uart1_se_mq_pool),       /* 内存池的大小是mq_pool的大小 */
				RT_IPC_FLAG_FIFO);              /* 如果有多个线程等待，按照先来先得到的方法分配消息 */

	
    // .....................................静态线程初始化.......................................
    rt_thread_init( &se_uart1_thread,                                   // 线程结构体
                    "se_uart1",                                         // 线程名称
                    thread_entry_se_uart1, RT_NULL,                     // 线程入口函数名称和参数
                    se_uart1_thread_stack, SE_UART1_THREAD_STACK_SZ,  // 栈的起始位置和大小 
                    SE_UART1_THREAD_PRIO,  SE_UART1_THREAD_TIMESLICE); // 线程优先级，时间片大小
    
    //..................................... 启动线程 ，进入线程入口函数...........................
    rt_thread_startup(&se_uart1_thread);
	return  0;     
}
//INIT_APP_EXPORT(thread_init_se_uart1);	

/********************************************************************************************
** 函数名称：     
** 函数描述：uart1接收线程的初始化
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
int thread_init_re_uart1(void)
{ 
	basic_uart1_init(BAUD_RATE_115200);
  /* 初始化消息队列 */
	rt_mq_init( &uart1_rx_mq,
				"uart1_rx_mq",               
				uart1_rx_mq_pool,               /* 内存池指向msg_pool */
				UART1_RE_LEN,                /* 每个消息的大小是 1 字节 */
				sizeof(uart1_rx_mq_pool),       /* 内存池的大小是mq_pool的大小 */
				RT_IPC_FLAG_FIFO);           /* 如果有多个线程等待，按照先来先得到的方法分配消息 */

	
    // .....................................静态线程初始化.......................................
    rt_thread_init( &re_uart1_thread,                                   // 线程结构体
                    "re_uart1",                                         // 线程名称
                    thread_entry_re_uart1, RT_NULL,                     // 线程入口函数名称和参数
                    re_uart1_thread_stack, RE_UART1_THREAD_STACK_SZ,  // 栈的起始位置和大小 
                    RE_UART1_THREAD_PRIO,  RE_UART1_THREAD_TIMESLICE); // 线程优先级，时间片大小
    
    //..................................... 启动线程 ，进入线程入口函数...........................
    rt_thread_startup(&re_uart1_thread);
	return  0;     
}
//INIT_APP_EXPORT(thread_init_re_uart1);	

/********************************************************************************************
** 函数名称：     
** 函数描述：UART1发送的数据
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
static void uart1_se_data(void)
{
	uart1_se_update( );  //发送数据更新
    
	//.........................数据的发送..........................
	bsp_uart1_send_data(u8_se_mvbboard_prot1_data , sizeof(u8_se_mvbboard_prot1_data )); 
}

/********************************************************************************************
** 函数名称：     
** 函数描述：UART1发送的数据
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
static void uart1_se_update(void)
{
    uint8_t re_buf[UART1_SE_LEN] = {0};
    uint8_t i = 0;
	uint16_t mvb_crc = 0;
	
	if( rt_mq_recv(&uart1_se_mq, &re_buf, sizeof(re_buf), 5) == RT_EOK )   //超时时间5ms 单位为ms
	{
		for(i = 0; i < UART1_SE_LEN; i++)
        {
			u8_se_mvbprot1_data[i] = re_buf[i];
		}	
	}

    for(i = 0; i < UART1_SE_LEN; i++)
	{
		u8_se_mvbboard_prot1_data[i+7] = u8_se_mvbprot1_data[i];
	}

	mvb_crc = get_mvb_crc(u8_se_mvbboard_prot1_data , 7 + MVB_SPORT1_LEN);
	u8_se_mvbboard_prot1_data [7 + MVB_SPORT1_LEN] = (mvb_crc & 0xFF00)>>8;
	u8_se_mvbboard_prot1_data [8 + MVB_SPORT1_LEN] =  mvb_crc & 0x00FF;  
}

/*********************************************************************************************************
** 函数名称：MVB数据的初始化
** 函数描述：
** 输入参数：
** 返回值  ：
*********************************************************************************************************///
static void uart1_variable_init(void)
{  
	uint16_t mvb_crc = 0;
	
	//...................初始化MVB配置信息...................
	u8_mvb_configure_command[0] = 0xFE;
	u8_mvb_configure_command[1] = 0xFA;
	u8_mvb_configure_command[2] = 0x05;
	u8_mvb_configure_command[3] = (MVB_DEVICE_ADDR & 0xFF00)>>8;
	u8_mvb_configure_command[4] = MVB_DEVICE_ADDR & 0x00FF;
    
	u8_mvb_configure_command[5] = MVB_SPORT_NUM;
	u8_mvb_configure_command[6] = (MVB_SPORT1_ADDR & 0xFF00)>>8;
	u8_mvb_configure_command[7] = MVB_SPORT1_ADDR & 0x00FF;
    u8_mvb_configure_command[8] = MVB_SPORT1_LEN;
	u8_mvb_configure_command[9] = MVB_CYCLE_TIME;

    u8_mvb_configure_command[126] = MVB_DPORT_NUM;
	u8_mvb_configure_command[127] = (MVB_DPORT1_ADDR & 0xFF00) >> 8;
	u8_mvb_configure_command[128] = MVB_DPORT1_ADDR & 0x00FF;
    u8_mvb_configure_command[129] = MVB_DPORT1_LEN;
	u8_mvb_configure_command[130] = MVB_CYCLE_TIME; 
    
	mvb_crc = get_mvb_crc(u8_mvb_configure_command, 247);
	u8_mvb_configure_command[247] = (mvb_crc & 0xFF00)>>8;
    u8_mvb_configure_command[248] =  mvb_crc & 0x00FF;
	u8_mvb_configure_command[249] =  0xFF;

    //...................初始化NVB板卡向MCU发送其mvb数据使能指令...................
	u8_reenable_mvb_command[0] = 0XFE; //
	u8_reenable_mvb_command[1] = 0X08; //
	u8_reenable_mvb_command[2] = 0X07; //
	u8_reenable_mvb_command[3] = MVB_CYCLE_TIME;
    u8_reenable_mvb_command[4] = 0X01; // 启动NVB板卡向MCU发送其mvb数据
    mvb_crc = get_mvb_crc(u8_reenable_mvb_command, 5);
	u8_mvb_configure_command[5] = (mvb_crc & 0xFF00)>>8;
    u8_mvb_configure_command[6] =  mvb_crc & 0x00FF;
    u8_mvb_configure_command[7] =  0xFF;

    //...................初始化接收mvb数据...................
    u8_re_mvbboard_prot1_data [0] = 0XFE; //
    u8_re_mvbboard_prot1_data [1] = 9 + MVB_DPORT1_LEN; //
	u8_re_mvbboard_prot1_data [2] = 0x08;
	u8_re_mvbboard_prot1_data [4] = (MVB_DPORT1_ADDR & 0xFF00) >> 8;
    u8_re_mvbboard_prot1_data [5] = MVB_DPORT1_ADDR & 0x00FF;
    u8_re_mvbboard_prot1_data [8 + MVB_DPORT1_LEN]= 0xFF;

    //...................MCU向mvb发送数据指令...................
    u8_se_mvbboard_prot1_data [0] = 0XFE;  //
	u8_se_mvbboard_prot1_data [1] = 10 + MVB_SPORT1_LEN; //
	u8_se_mvbboard_prot1_data [2] = 0x09;
	u8_se_mvbboard_prot1_data [3] = 0x80;  //设备数据有效
    
    u8_se_mvbboard_prot1_data [5] = 	(MVB_SPORT1_ADDR & 0xFF00) >> 8;
    u8_se_mvbboard_prot1_data [6] = 	MVB_SPORT1_ADDR & 0x00FF;
    u8_se_mvbboard_prot1_data [9 + MVB_SPORT1_LEN] =  0xFF;    
}

/********************************************************************************************
** 函数名称：     
** 函数描述：UART1接收UART1板卡的数据
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
static void uart1_re_data(void)
{   
    static  uint16_t  u16_uart1_rx_count = 0;
    uint8_t re_buf = 0; //单次从终端接收的单个数据
	uint8_t   se_mq_buf[UART1_RE_LEN];    //需要发送的消息队列的数据
    uint8_t i = 0;

    //..............................阻塞接收数据................................
	while (rt_device_read(serial_uart1, -1, &re_buf, 1) != 1)
	{
		rt_sem_take(&rx_sem1, RT_WAITING_FOREVER);
	}
	
    //............. 接收第一个数据判断起始为7E..................
	inbuf1[u16_uart1_rx_count] = re_buf;       //

    if( inbuf1[0] != 0xFE)         
    {
        u16_uart1_rx_count = 0;
    }
	else 
    {
        u16_uart1_rx_count++;
    }	
	
    //............. 判断第二个数据是否正确..................	
	if((!(inbuf1[1] == 0x06 || inbuf1[1] == u8_re_mvbboard_prot1_data [1]))
		&& (u16_uart1_rx_count >= 2))   //接收到第二个数据时查看是否正确   
    {
        u16_uart1_rx_count = 0;
		for(i = 0; i < UART1_RE_LEN ; i++) //清0接收缓存
		{ 
			inbuf1[i]= 0;
		}	
    }
	
	//...................1 MVB配置的应答帧...................
	if( (inbuf1[1] == 0x06) && (inbuf1[5] == 0xFF) && (u16_uart1_rx_count == 6)) 
	{
		u16_uart1_rx_count = 0;
		if(inbuf1[2] == 0x06)  //是否配置完成
		{
			//.....................接收数据使能........................
			u8_mvb_configure_flag = 1;
			bsp_uart1_send_data(u8_reenable_mvb_command, sizeof(u8_reenable_mvb_command));  
		}
	}  
	
    //...................2 接收MVB端口1数据...................
    if( (inbuf1[1] == u8_re_mvbboard_prot1_data [1]) && (inbuf1[2] == u8_re_mvbboard_prot1_data [2]) && (inbuf1[8 + MVB_DPORT1_LEN] == 0xFF)
	    && (u16_uart1_rx_count == 9 + MVB_DPORT1_LEN)  ) 
	{
		u16_uart1_rx_count = 0;

		if( (inbuf1[4] == u8_re_mvbboard_prot1_data [4])&&(inbuf1[5] == u8_re_mvbboard_prot1_data [5]))
		{
			for(i = 0; i < 9 + MVB_DPORT1_LEN ; i++)
			{
				se_mq_buf[i] = inbuf1[i];
			}	

			for(i = 0; i < 32 ; i++)
			{
				u8_re_mvbprot1_data[i] = inbuf1[i + 6];
			}	
			//.........................发送消息队列.......................................
			if (rt_mq_send(&uart1_rx_mq, &se_mq_buf, sizeof(se_mq_buf)) == -RT_ERROR)
			{
				rt_kprintf("uart1_rx_mq send err!\n");
			}
		}
	}   
}
/*********************************************************************************************************
** 函数名称：MVB校验
** 函数描述：计算校验码 和00异或其值不变
** 输入参数：
** 返回值  ：
*********************************************************************************************************///
static uint16_t get_mvb_crc(unsigned char *buf, int len) 
{ 
	int counter;
	unsigned char *p = buf; 
	unsigned short crc = 0x0000; 
	for( counter = 0; counter < len; counter++) 
	{
		crc = (crc<<8) ^ crc_table[( (crc>>8)  ^  *(char *)p++ ) & 0x00FF];
	} 
	return crc;
}
