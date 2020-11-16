/******************************************************************************************
@title	             	深圳20BMS M4 thread_tcp_rev.c
@funcation            以太网读取版本信息    (版本信息包含：应用程序版本，bootloader版本，硬件版本等)
@author	            	wentao
@since		            MDK 5.31
@version	            1.0
@remark		                    
@time                 2020/11/09
@site     
_______________________________________________________________________________________________________________________________________________________
|时间                 |     进度                     |            后续进度              |   问题啊！！！                                              |
|————————————————————————————————————————————————————|——————————————————————————————————|—————————————————————————————————————————————————————————————|
|2020.11.09 ：        |             新建             |一大堆 ！！！！                   |一大堆 ！！！！                                              |
|2020.11.10 ：        |     构建完成                 |一大堆 ！！！！                   |                                                             |
|2020.11.11 ：        |         ！！！板子坏了       |                                  |与维护线程不能同时打开！！！                                 |
|2020.11.12 ：        |            数据发送OK！      |待加以太网接收数据“versionread”， |与维护线程不能同时打开！！！                                 |
|                     |                              |后回复版本信息                    |                                                             |
|2020.11.12.16：46：  |功能加班ojbk                  |详细写上版本号，bug               |不晓得是不是看门狗的问题，经常复位重启，                     |
|                     |                              |                                  |再就是与 re_pc_eth_thread_init（）同时开启不能用             |
|           18: 00:   |                              |init_TcpNet()问题导致与另一个     |问题？？？？？？再找找                                       |
|                     |                              |以太网线程不能同时开启，看门狗加入|                                                             |
|                     |                              | 定时喂狗                         |                                                             |
|2020.11.13.08:15:    |                              |                                  |以太网给M4疯狂发数据，一定时间后两个以太网都会连接不上，     |
|           14:30：   |sendbuf动态内存分配问题解决   |                                  |                                                             |
|                     |sendbuf用指针的时候，要判断指 |                                  |                                                             |
|                     |针是不是空的                  |                                  |                                                             |
|                     |                              |                                  |                                                             |
|                     |                              |                                  |                                                             |
|                     |                              |                                  |                                                             |
——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————|


PC端参数信息：   
	 IP地址：192.168.0.10;
	 端口号：6111；	
M4端参数信息：   
	 IP地址：192.168.0.200;
	 端口号：6103；versionread
通讯方式：   
	 问答式，即上位机发送询问M4的版本信号指令，M4板收到该询问指令后回复版本信息
上位机发送指令内容：
				"version_read"
M4板回复版本信息：
	 版本信息包含：应用程序版本，bootloader版本，硬件版本等 
	 具体格式："ZZaxxxxxxxxx;ZZbxxxxxxx;ZZ3.226.xxxx;"    命名详见软件命名规范
	 （每一个版本之间用“;”隔开，如特殊情况还有其他版本信息需要添加，可在“;”后面继续添加）
*******************************************************************************************/
/******************************************************************************************
** 头文件
*******************************************************************************************/

#include "thread_tcp_rev.h"

/******************************************************************************************
** 宏定义
*******************************************************************************************/

#define  REV_PCETH_THREAD_STACK_SZ        4096         // 线程栈大小，单位是字节。
//#define  RE_PCETH_THREAD_PRIO            21           // 优先级 范围是从 0 ～ 255，数值越小优先级越高，0 代表最高优先级
//#define  RE_PCETH_THREAD_TIMESLICE       30           // 时间片大小：调用一次调度能够运行的最大时间长度
#define  rev_LocalPort_NUM                6103
#define  rev_RemotePort_NUM               6111
#define  IP1                              192
#define  IP2                              168
#define  IP3                              0
#define  IP4                              10

#define  BUFSZ                            1024


/********************************************************************************************
** 定义变量
********************************************************************************************/

static const char send_data[] = "ZZa_PD19011_A001.V1.0_20201116;ZZb*******;ZZ3.226.931DLB";   //===瞅瞅瞅！！！！要发生的信息在这

static const char rec_data[] = {0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x5f,0x72,0x65,0x61,0x64};   //===“version_read”
/* 发送用到的数据 */   /*版本信息包含：应用程序版本，bootloader版本，硬件版本等*/
static struct rt_thread rev_re_pc_eth_thread;
static rt_uint8_t rev_re_pc_eth_thread_stack[REV_PCETH_THREAD_STACK_SZ];

static rt_uint8_t socket_rev;          //===
static rt_uint8_t remote_rev_ip[4] = {IP1,IP2,IP3,IP4};
static int tcp_connect_flag = 0;       //===定义连接标志位
static int flag = 0;
/********************************************************************************************
** 函数申明
********************************************************************************************/

static rt_uint16_t tcp_callback (rt_uint8_t soc, rt_uint8_t evt, rt_uint8_t *ptr, rt_uint16_t par);  //===??
static void rev_se_eth_PC_data(void);  //===发送数据


/*******************************************************************************
** 函数名称：
** 函数描述：接收udpserv数据发送线程入口
** 输入参数：
** 返回值  ：无
**
*******************************************************************************/
static void thread_entry_rev_re_pc_eth(void *param)
{

    int  res = 0;
//.........................TCP协议初始化.............................
//    init_TcpNet (); 
	rt_thread_mdelay(4000);
 //........................建立socket..................................
	socket_rev = tcp_get_socket (TCP_TYPE_CLIENT | TCP_TYPE_KEEP_ALIVE, 0, 10, tcp_callback);
	if(socket_rev != 0)   //分配同TCP socket成功
	{
			rt_kprintf("TCP Socket rev creat ok!\n");
			res = tcp_connect (socket_rev, remote_rev_ip, rev_RemotePort_NUM, rev_LocalPort_NUM);
			if(res)  rt_kprintf("TCP socket rev creat done! res = %d\r\n", res);
			else rt_kprintf("TCP socket rev creat err! res = %d\r\n", res);
	}

	while (1)
	{
        
		if(tcp_connect_flag) //socket连接成功，接收到数据（接收数据用回调函数）
	
		{		
		//.......................发送数据.........................	
			if(flag)	
			{	
				rev_se_eth_PC_data( );      //===收到那个啥子的时候，发送数据  	
				rt_thread_mdelay(300);		
			}	
  }
		
	else
	{
        tcp_abort (socket_rev);
        tcp_close (socket_rev);
        tcp_release_socket (socket_rev);
        rt_thread_mdelay(1000);
        socket_rev = tcp_get_socket (TCP_TYPE_CLIENT | TCP_TYPE_KEEP_ALIVE, 0, 10, tcp_callback);
        if(socket_rev != 0)
        {
                res = tcp_connect (socket_rev, remote_rev_ip, rev_RemotePort_NUM, rev_LocalPort_NUM);
                if(res)
                {
                        rt_kprintf("TCP Socket rev creat done! res = %d\r\n", res);
                }
        }
        rt_thread_mdelay(800);
	}
	
	rt_thread_mdelay(200);	
							
	}

}


/********************************************************************************************
** 函数名称：     
** 函数描述：线程的初始化
** 输入参数：无
** 返回值  ：    无
**
********************************************************************************************/

int rev_re_pc_eth_thread_init(void)
{

	// .....................................静态线程初始化.......................................
	rt_thread_init( &rev_re_pc_eth_thread,                                   // 线程结构体
									"rev_pc_eth",                                         // 线程名称
									thread_entry_rev_re_pc_eth, RT_NULL,                     // 线程入口函数名称和参数
									rev_re_pc_eth_thread_stack, REV_PCETH_THREAD_STACK_SZ,      // 栈的起始位置和大小 
									23,  30);    // 线程优先级，时间片大小
	 //..................................... 启动线程 ，进入线程入口函数...........................
	rt_thread_startup(&rev_re_pc_eth_thread);
    return 0 ;    
}
INIT_COMPONENT_EXPORT(rev_re_pc_eth_thread_init);

/*******************************************************************************
** 函数名称：
** 函数描述：tcp回调函数
** 输入参数：
** 返回值  ：无
**
*******************************************************************************/
static rt_uint16_t tcp_callback (rt_uint8_t soc, rt_uint8_t evt, rt_uint8_t *ptr, rt_uint16_t par)
{

    char buf[50];
    rt_uint16_t i;

/* 确保是socket_rev的回调 */

    if (soc != socket_rev) 
    {
        return (0);
    }
	/*
		远程客户端连接消息
			1、数组ptr存储远程设备的IP地址，par中存储端口号。
			2、返回数值1允许连接，返回数值0禁止连接。
*/
switch (evt) 
{
	case TCP_EVT_CONREQ:
		sprintf(buf, "[remote client][IP: %d.%d.%d.%d] connectting ... ", ptr[0], ptr[1], ptr[2], ptr[3]);
		rt_kprintf("IP:%s  port:%d\r\n", buf, par);
		return (1);
	
	/* 连接终止 */
	case TCP_EVT_ABORT:
		tcp_connect_flag =0;
		break;
	
	/* Socket远程连接已经建立 */
	case TCP_EVT_CONNECT:
		rt_kprintf("tcp Socket rev connected remote success!\r\n"); 
			tcp_connect_flag =1;
		break;
	
	/* 连接断开 */
	case TCP_EVT_CLOSE:
			rt_kprintf("tcp rev connection has been closed!\r\n");
			tcp_connect_flag =0;
		break;
	
	/* 发送的数据收到远程设备应答 */
	case TCP_EVT_ACK:
		break;
	/* 接收到TCP数据帧，ptr指向数据地址，par记录数据长度，单位字节 */
	case TCP_EVT_DATA:
        
//===rt_kprintf("Data length = %d\r\n", par);
//===rt_kprintf("[hex]:");
    
     if(par==12)
	 {
		for(i = 0; i < par; i++)
		{       
				if(ptr[i]!=rec_data[i])       //===判读接收的数据是不是version_read
				break;
		}
		if(i<12)
		{
			flag = 0;
		}
				
		else
		{
			flag = 1;
		}
					
	  }
							
				
		break;
}

return (0);
}

/*******************************************************************************
** 函数名称：se_eth_PC_rev(void)
** 函数描述：发送PC端的版本信息
** 输入参数：
** 返回值  ：无
*******************************************************************************/
//static void se_eth_PC_data(void)
static void rev_se_eth_PC_data(void)
{
 
	rt_uint8_t *sendbuf;
 
	//.................发送数据................................................
	sendbuf = tcp_get_buf(SE_REVETH_LEN );     //===发送的字节数（同时申请动态内存）
	if(RT_NULL != sendbuf)                     //===用指针的时候，要判断指针是不是空的
	{
		memcpy (sendbuf, send_data, SE_REVETH_LEN );
		tcp_send (socket_rev, sendbuf, SE_REVETH_LEN );     //===发送数据 （同时释放动态内存）         
	}
	else
	{
		sendbuf = rt_malloc(26);
		__asm("nop");
		rt_free(sendbuf);
	}
   flag=0;
/*
memcpy ： 内存拷贝函数
函数原型为void *memcpy(void *destin, void *source, unsigned n)；
函数的功能是从源内存地址的起始位置开始拷贝若干个字节到目标内存地址中，
即从源source中拷贝n个字节到目标destin中
参数：
destin-- 指向用于存储复制内容的目标数组，类型强制转换为 void* 指针。
source-- 指向要复制的数据源，类型强制转换为 void* 指针。
n-- 要被复制的字节数。
所需头文件：
C语言：#include<string.h>
*/
//	tcp_send (socket_rev, sendbuf, SE_REVETH_LEN );     //===发送数据

/*
socket_tcp:   从TCP套接字发送数据包
sendbuf:      指向包含要发送的数据的缓冲区的指针
SE_PCETH_LEN: 要发送的数据字节数
*/

}
