/********************************************************************************************
头文件
********************************************************************************************/
#include "thread_can_sbox.h"

/******************************************************************************************
** 宏定义
*******************************************************************************************/
#define  CAN1_SE_THREAD_STACK_SZ        512   // 线程栈大小，单位是字节。
#define  CAN1_SE_THREAD_PRIO            23    // 优先级 范围是从 0 ～ 255，数值越小优先级越高，0 代表最高优先级
#define  CAN1_SE_THREAD_TIMESLICE       15   // 时间片大小：调用一次调度能够运行的最大时间长度

#define  CAN1_RX_THREAD_STACK_SZ        512   // 线程栈大小，单位是字节。
#define  CAN1_RX_THREAD_PRIO            24    // 优先级 范围是从 0 ～ 255，数值越小优先级越高，0 代表最高优先级
#define  CAN1_RX_THREAD_TIMESLICE       15   // 时间片大小：调用一次调度能够运行的最大时间长度

/********************************************************************************************
** 定义变量
********************************************************************************************/
extern struct rt_semaphore  rx_can1_sem;  /* 用于接收消息的信号量 */

struct rt_messagequeue can1_se_mq;       //CAN发送端消息队列
rt_uint8_t    can1_se_mq_pool[20];       //CAN发送端消息队列的内存大小


uint8_t u8_se_can1_data[CAN1_SE_LEN] = {0x00};  //源端口1（发送）数据 
uint8_t u8_re_can1_data[CAN1_RX_LEN] = {0x00};  //宿端口1（接收）数据 

/********************************************************************************************
** 函数申明
********************************************************************************************/
void basic_CAN_Init(rt_device_t dev, enum CANBAUD baudrate);   // CAN初始化函数
void basic_CAN_Filter_Init(rt_device_t dev, uint32_t id, uint8_t ide, uint32_t mask, uint8_t hdr);// CAN过滤器配置函数

static void can1_se_data(void);
static void can1_se_update(void);

static void can1_rx_data(struct rt_can_msg  *rxmsg);    // CAN1rx数据处理函数

/********************************************************************************************************
** 函数名称：
** 函数描述：can发送的线程入口
** 输入参数：
** 返回值  ：无
*********************************************************************************************************/
static void thread_entry_can1_se(void *param)
{
    rt_thread_mdelay(200);
    while(1)
    {
        can1_se_data( );
        rt_thread_mdelay(500);
    }
}
/********************************************************************************************************
** 函数名称：
** 函数描述：CAN接收线程入口
** 输入参数：
** 返回值  ：无
*********************************************************************************************************/
static void thread_entry_can1_rx(void *param)
{
    static rt_device_t can_dev = RT_NULL;   // can设备句柄
    struct rt_can_msg rxmsg = {0};
    
    if(RT_NULL == can_dev)
    {
        can_dev = rt_device_find("can1");
    }
    while(1)
    {
        if(RT_NULL != can_dev)
        {
            rxmsg.hdr = (rt_uint8_t) - 1;                      /* hdr 值为 - 1，表示直接从 uselist 链表读取数据 */
            rt_sem_take(&rx_can1_sem, RT_WAITING_FOREVER);     /* 阻塞等待接收信号量 */
            rt_device_read(can_dev, 0, &rxmsg, sizeof(rxmsg)); /* 从 CAN 读取一帧数据 */
            {
                can1_rx_data(&rxmsg);                // 处理can1接收到的数据
            }
			
        }
       // rt_thread_mdelay(10);
    }
}

/********************************************************************************************
** 函数名称：     
** 函数描述：线程的初始化
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
static int thread_creat_can1_se(void)
{
    rt_thread_t thread;

      //.................... 初始化消息队列 ...........................
    rt_mq_init( &can1_se_mq,
				"can1_se_mq",               
				can1_se_mq_pool,               /* 内存池指向msg_pool */
				CAN1_SE_LEN,                  /* 每个消息的大小 */
				sizeof(can1_se_mq_pool),       /* 内存池的大小是mq_pool的大小 */
				RT_IPC_FLAG_FIFO);          /* 如果有多个线程等待，按照先来先得到的方法分配消息 */

    //.................... 创建数据发送线程 ...........................
    thread = rt_thread_create("can1_se",    \
                              thread_entry_can1_se, \
                              RT_NULL,  \
                              CAN1_SE_THREAD_STACK_SZ,     \
                              CAN1_SE_THREAD_PRIO,       \
                              CAN1_SE_THREAD_TIMESLICE);
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);  // 线程创建成功,启动线程
    }
    else
    {
        LOG_D("create %s thread failed!\n", __func__);
    }

    return 0;
}
INIT_APP_EXPORT(thread_creat_can1_se);
/********************************************************************************************
** 函数名称：     
** 函数描述：CAN接收线程的初始化
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
static int thread_creat_can1_rx(void)
{
    rt_thread_t thread;
    uint8_t i;
  
    static rt_device_t can_dev = RT_NULL;   // can设备句柄
     /* 查找 CAN 设备 */
    can_dev = rt_device_find("can1");
    if (RT_NULL == can_dev)
    {
        LOG_D("find %s failed!\n", "can1");
    }
    
    rt_sem_init(&rx_can1_sem, "ca1_rx_sem", 0, RT_IPC_FLAG_FIFO);   /* 初始化 CAN 接收信号量 */
    //.................设置CAN波特率.........................
    basic_CAN_Init(can_dev, CAN1_BAUD_RATE);
    //.................设置CANid............................
    for(i = 0; i < TEST_BAT_NUM; i++)
    {
        basic_CAN_Filter_Init(can_dev, 0x601+i, RT_CAN_STDID, 0x7FF, 0);  //掩码的位数为0时表示不对比该位;0是硬件过滤号
    }
    /* 创建数据接收线程 */
    thread = rt_thread_create("can1_rx",    \
                              thread_entry_can1_rx, \
                              RT_NULL,  \
                              CAN1_RX_THREAD_STACK_SZ,     \
                              CAN1_RX_THREAD_PRIO,       \
                              CAN1_RX_THREAD_TIMESLICE);
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);  // 线程创建成功,启动线程
    }
    else
    {
        LOG_D("create %s thread failed!\n", __func__);
    }
    return 0;
}
INIT_APP_EXPORT(thread_creat_can1_rx);

/********************************************************************************************
** 函数名称：     
** 函数描述：CAN1发送的数据
** 输入参数：无
** 返回值 ： 无
********************************************************************************************/
static void can1_se_data(void)
{
	can1_se_update( );  //发送数据更新
    
	//.........................数据的发送..........................
	bsp_can_send_sbox_data( CAN_SEND_SBOX_ID, 8, u8_se_can1_data );
}

/********************************************************************************************
** 函数名称：     
** 函数描述：UART1发送的数据
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
static void can1_se_update(void)
{
    uint8_t re_buf[CAN1_SE_LEN] = {0};
    uint8_t i = 0;
	
	if( rt_mq_recv(&can1_se_mq, &re_buf, sizeof(re_buf), 5) == RT_EOK )   //超时时间5ms 单位为ms
	{
		for(i = 0; i < CAN1_SE_LEN; i++)
        {
			u8_se_can1_data[i] = re_buf[i];
		}	
	}
}


/*********************************************************************************************************
** 函数名称：
** 函数描述：用消息队列的方式，发送接收的数据
** 输入参数：
** 返回值  ：
*********************************************************************************************************///
static void can1_rx_data(struct rt_can_msg  *rxmsg)
{	
    //static uint8_t can1_se_mq_buf[SUM_BAT_NUM_MAX][8] = {0}; //需发送的消息队列
	static uint8_t re_count = 0;
    uint8_t i = 0, j = 0;
    
    
    if((rxmsg->id >= 0x0601 ) && (rxmsg->id <= 0x0600 + TEST_BAT_NUM ))
    {
        i = rxmsg->id - 0x0601;

        for(j = 0; j < 8; j++)
        { 
            //un_swap_re_sbox_data[i].u8_data[j] =	rxmsg->data[j]; 
        }	
    }

    // if(re_count < TEST_BAT_NUM)
    // {
    //     re_count++;
    // }
    // else
    // {
    //     re_count = 0;
    //     //采样盒通讯数据接收完成后发送消息队列
    //     if (rt_mq_send(&can1_rx_mq, &can1_se_mq_buf, TEST_BAT_NUM*8) == -RT_ERROR)
    //     {
    //         rt_kprintf("can1_rx_mq send err!\n");
    //     }
    // } 
}

