/******************************************************************************************
 * 程序清单：udp 服务端
 *
 * 这是一个 udp 服务端的例程
 * 导出 udpserv 命令到控制终端
 * 命令调用格式：udpserv
 * 无参数
 * 程序功能：作为一个服务端，接收并显示客户端发来的数据 ，接收到 exit 退出程序
*******************************************************************************************/

/******************************************************************************************
** 头文件
*******************************************************************************************/
#include "thread_tcp_client.h"

/******************************************************************************************
** 宏定义
*******************************************************************************************/
#define  RE_PCETH_THREAD_STACK_SZ        1024         // 线程栈大小，单位是字节。
#define  RE_PCETH_THREAD_PRIO            21         // 优先级 范围是从 0 ～ 255，数值越小优先级越高，0 代表最高优先级
#define  RE_PCETH_THREAD_TIMESLICE       30          // 时间片大小：调用一次调度能够运行的最大时间长度

#define LocalPort_NUM       6101
#define RemotePort_NUM      6109
#define IP1                 192
#define IP2                 168
#define IP3                 0
#define IP4                 10

#define BUFSZ            1024

/********************************************************************************************
** 定义变量
********************************************************************************************/
static struct rt_thread re_pceth_thread;  
static rt_uint8_t re_pceth_thread_stack[RE_PCETH_THREAD_STACK_SZ];

rt_uint8_t remote_ip[4] = {IP1,IP2,IP3,IP4};
rt_uint8_t socket_tcp;
int tcp_connect_flag = 0;
int tcp_redata_flag = 0;
char re_buf[BUFSZ];	//接收的缓冲区  

uint8_t  product_preset_flag = 0; //1表示有数据要设置

union   Se_eth_PC_data   un_seeth_PC_data;
union   Re_eth_PC_data   un_reeth_PC_data;
		     
/********************************************************************************************
** 函数申明
********************************************************************************************/
static rt_uint16_t tcp_callback (rt_uint8_t soc, rt_uint8_t evt, rt_uint8_t *ptr, rt_uint16_t par);
static void re_eth_PC_data(char *data);
static void se_eth_PC_data(void);
static void se_eth_PC_update(void);
static void re_eth_PC_data_time_set( void);
static void re_eth_PC_data_product_preset( void);
/*******************************************************************************
** 函数名称：
** 函数描述：接收udpserv数据发送线程入口
** 输入参数：
** 返回值  ：无
*******************************************************************************/
static void thread_entry_re_pceth(void *param)
{

	int  res = 0;
   
	//.................TCP协议初始化.........................
    init_TcpNet (); 
    rt_thread_mdelay(10000);
	
   //........................建立socket..................................
    socket_tcp = tcp_get_socket (TCP_TYPE_CLIENT | TCP_TYPE_KEEP_ALIVE, 0, 10, tcp_callback);
    if(socket_tcp != 0)   //分配同TCP socket成功
    {
        rt_kprintf("TCP Socket creat ok!\n");
        res = tcp_connect (socket_tcp, remote_ip, RemotePort_NUM, LocalPort_NUM);
//        if(res)  rt_kprintf("TCP socket creat done! res = %d\r\n", res);
//        else rt_kprintf("TCP socket creat err! res = %d\r\n", res);
    }

    {
    while (1)
        if(tcp_connect_flag) //socket连接成功，接收到数据（接收数据用回调函数）
		{		
			//...............发送数据.........................
			se_eth_PC_data( );      
			rt_thread_mdelay(500);		 
        }
		else
		{
            tcp_abort (socket_tcp);
            tcp_close (socket_tcp);
            tcp_release_socket (socket_tcp);
            rt_thread_mdelay(1000);
            socket_tcp = tcp_get_socket (TCP_TYPE_CLIENT | TCP_TYPE_KEEP_ALIVE, 0, 10, tcp_callback);
            if(socket_tcp != 0)
            {
                res = tcp_connect (socket_tcp, remote_ip, RemotePort_NUM, LocalPort_NUM);
                if(res)
                {
                    //rt_kprintf("TCP Socket creat done! res = %d\r\n", res);
                }
            }
            rt_thread_mdelay(1000);
		}
                
    }

	return;
}


/********************************************************************************************
** 函数名称：     
** 函数描述：线程的初始化
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
int re_pceth_thread_init(void)
{
 
    // .....................................静态线程初始化.......................................
    rt_thread_init( &re_pceth_thread,                                   // 线程结构体
                    "re_pceth",                                         // 线程名称
                    thread_entry_re_pceth, RT_NULL,                     // 线程入口函数名称和参数
                    re_pceth_thread_stack, RE_PCETH_THREAD_STACK_SZ,      // 栈的起始位置和大小 
                    RE_PCETH_THREAD_PRIO,  RE_PCETH_THREAD_TIMESLICE);    // 线程优先级，时间片大小
     //..................................... 启动线程 ，进入线程入口函数...........................
    rt_thread_startup(&re_pceth_thread);
	return 0 ;    
}
INIT_COMPONENT_EXPORT(re_pceth_thread_init);

/*******************************************************************************
** 函数名称：
** 函数描述：tcp回调函数
** 输入参数：
** 返回值  ：无
*******************************************************************************/
static rt_uint16_t tcp_callback (rt_uint8_t soc, rt_uint8_t evt, rt_uint8_t *ptr, rt_uint16_t par)
{
	char buf[50];
	rt_uint16_t i;
	
	/* 确保是socket_tcp的回调 */
	if (soc != socket_tcp) 
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
			rt_kprintf("tcp Socket connected remote success!\r\n"); 
		    tcp_connect_flag =1;
			break;
		
		/* 连接断开 */
		case TCP_EVT_CLOSE:
		   	rt_kprintf("tcp connection has been closed!\r\n");       
		    tcp_connect_flag =0;
			break;
		
		/* 发送的数据收到远程设备应答 */
		case TCP_EVT_ACK:
			break;
		
		/* 接收到TCP数据帧，ptr指向数据地址，par记录数据长度，单位字节 */
		case TCP_EVT_DATA:
//			rt_kprintf("Data length = %d\r\n", par);
//            rt_kprintf("[hex]:");
//			for(i = 0; i < par; i++)
//			{
//				rt_kprintf("0x%02x ", ptr[i]);
//			}
//            rt_kprintf("\r\n");

            //.............接收数据.....................
            if(par == RE_PCETH_LEN)
            {		
                re_eth_PC_data(ptr);
            }
			break;
	}
	
	return (0);
}
/*******************************************************************************
** 函数名称：
** 函数描述：接收PC端的ETH数据
** 输入参数：
** 返回值  ：无
*******************************************************************************/
static void  re_eth_PC_data(char *data )
{
    uint8_t i = 0;
    union   Re_eth_PC_data    un_swap_reeth_PC_data;

    for(i = 0; i < RE_PCETH_LEN ; i++)
    {
        un_swap_reeth_PC_data.u8_data[i] = data[i];
    }

    endian_swaps(&un_swap_reeth_PC_data.st_data.u16_life);
    endian_swaps(&un_swap_reeth_PC_data.st_data.u16_resv);
    endian_swaps(&un_swap_reeth_PC_data.st_data.st_product_preset.u16_product_num);
    
    un_reeth_PC_data = un_swap_reeth_PC_data ;
   //st_product_preset = un_reeth_PC_data.st_data.st_product_preset;

    re_eth_PC_data_time_set(); 
    re_eth_PC_data_product_preset( );

}
/*******************************************************************************
** 函数名称：
** 函数描述：接收PC端的ETH数据后产品信息的设置
** 输入参数：
** 返回值  ：无
*******************************************************************************/
static void re_eth_PC_data_time_set( void)
{
    struct tm now_tm;
	if(1 == un_reeth_PC_data.st_data.st_product_preset.un_preset.st_bit.set_time_valid)
	{
        get_rtc_time(&now_tm);
        if( (now_tm.tm_year != un_reeth_PC_data.st_data.st_product_preset.u8_time[0] + 100) || 
            (now_tm.tm_mon  != un_reeth_PC_data.st_data.st_product_preset.u8_time[1] - 1) || 
            (now_tm.tm_mday != un_reeth_PC_data.st_data.st_product_preset.u8_time[2])    || 
            (now_tm.tm_hour != un_reeth_PC_data.st_data.st_product_preset.u8_time[3]) || 
            (now_tm.tm_min  != un_reeth_PC_data.st_data.st_product_preset.u8_time[4]) || 
            (abs(now_tm.tm_sec - un_reeth_PC_data.st_data.st_product_preset.u8_time[5]) >= 10) ) //相差10秒
        {
            now_tm.tm_year = un_reeth_PC_data.st_data.st_product_preset.u8_time[0] + 100;
            now_tm.tm_mon  = un_reeth_PC_data.st_data.st_product_preset.u8_time[1] - 1;
            now_tm.tm_mday = un_reeth_PC_data.st_data.st_product_preset.u8_time[2];
            now_tm.tm_hour = un_reeth_PC_data.st_data.st_product_preset.u8_time[3];
            now_tm.tm_min  = un_reeth_PC_data.st_data.st_product_preset.u8_time[4];
            now_tm.tm_sec = un_reeth_PC_data.st_data.st_product_preset.u8_time[5];
            set_rtc_time(now_tm);
        }
    }

}
/*******************************************************************************
** 函数名称：
** 函数描述：接收PC端的ETH数据后产品信息的设置
** 输入参数：
** 返回值  ：无
*******************************************************************************/
static void re_eth_PC_data_product_preset(void)
{
    if(1 == un_reeth_PC_data.st_data.st_product_preset.un_preset.st_bit.set_product_num_valid)
	{   
        if(st_product_preset.u16_product_num != un_reeth_PC_data.st_data.st_product_preset.u16_product_num)
		{
            st_product_preset.u16_product_num = un_reeth_PC_data.st_data.st_product_preset.u16_product_num;
            st_product_info.u16_product_num = st_product_preset.u16_product_num;
            product_preset_flag = 1;
        }
	}

    if(1 == un_reeth_PC_data.st_data.st_product_preset.un_preset.st_bit.set_car_num_valid)
	{ 
		if(st_product_preset.u8_car_num != un_reeth_PC_data.st_data.st_product_preset.u8_car_num)
        {
            st_product_preset.u8_car_num = un_reeth_PC_data.st_data.st_product_preset.u8_car_num;
            st_product_info.u8_car_num = st_product_preset.u8_car_num;
            product_preset_flag = 1;
        }
	}

    if(1 == un_reeth_PC_data.st_data.st_product_preset.un_preset.st_bit.set_product_num_valid)
	{ 
		if(st_product_preset.u8_bat_num != un_reeth_PC_data.st_data.st_product_preset.u8_bat_num)
        { 
            st_product_preset.u8_bat_num = un_reeth_PC_data.st_data.st_product_preset.u8_bat_num;
            st_product_info.u8_bat_num = st_product_preset.u8_bat_num;
            product_preset_flag = 1;
        }
	}

   //.................查看SOH是否重新设置（SOH影响可存储最大容量，即Qc_max需相应更新）...............................
	if( 1 == un_reeth_PC_data.st_data.st_product_preset.un_preset.st_bit.set_soh_valid)
	{
       if(st_product_preset.u8_preset_soh != un_reeth_PC_data.st_data.st_product_preset.u8_preset_soh)
	   {
            st_product_preset.u8_preset_soh = un_reeth_PC_data.st_data.st_product_preset.u8_preset_soh;
            st_bat_data.fl_bat_soh = st_product_preset.u8_preset_soh;
            st_bat_data.fl_bat_Qc_max = st_bat_data.fl_bat_soh * 0.01f * Bat_Qb ;
            if(st_bat_data.fl_bat_Qc_max < 0.5f * Bat_Qb)  st_bat_data.fl_bat_Qc_max = 0.5f* Bat_Qb;  //充满的最小值不能小于0.5标称容量
            product_preset_flag = 1;
       }
    }
	
	//.................查看SOC是否重新设置（SOC影响当前容量，即bat_Qnow需相应更新）...............................	
	if(1 == un_reeth_PC_data.st_data.st_product_preset.un_preset.st_bit.set_soc_valid)
	{
        if(st_product_preset.u8_preset_soc != (uint8_t)st_bat_data.fl_bat_soc)
        {
            st_product_preset.u8_preset_soc = un_reeth_PC_data.st_data.st_product_preset.u8_preset_soc;
	        st_bat_data.fl_bat_soc = st_product_preset.u8_preset_soc;
            st_bat_data.fl_bat_Qnow = st_bat_data.fl_bat_Qc_max * st_bat_data.fl_bat_soc * 0.01f;
            product_preset_flag = 1;
        }    
	}
    
    //.......................充电过流和放电过流值的设定.........................................................
    if(1 == un_reeth_PC_data.st_data.st_product_preset.un_preset.st_bit.set_ch_overI_valid)
	{ 
		st_product_preset.u8_ch_overI = un_reeth_PC_data.st_data.st_product_preset.u8_ch_overI;
	}

    if(1 == un_reeth_PC_data.st_data.st_product_preset.un_preset.st_bit.set_disch_overI_valid)
	{ 		
        st_product_preset.u8_disch_overI = un_reeth_PC_data.st_data.st_product_preset.u8_disch_overI;
	} 
    
}





/*******************************************************************************
** 函数名称：
** 函数描述：发送PC端的ETH数据
** 输入参数：
** 返回值  ：无
*******************************************************************************/
static void se_eth_PC_data(void)
{
   
    union   Se_eth_PC_data    un_swap_seeth_PC_data;
	uint8_t i = 0 ;
	rt_uint8_t *sendbuf;
   
	
	//.......................发送数据更新..........................
	se_eth_PC_update( );
	
    un_swap_seeth_PC_data = un_seeth_PC_data;

    endian_swaps(&un_swap_seeth_PC_data.st_data.u16_life);
    endian_swaps(&un_swap_seeth_PC_data.st_data.u16_resv);

    endian_swaps(&un_swap_seeth_PC_data.st_data.st_se_mvb.u16_bat_volt);
    endian_swaps(&un_swap_seeth_PC_data.st_data.st_se_mvb.u16_bat_soc);
    endian_swaps(&un_swap_seeth_PC_data.st_data.st_se_mvb.u16_bat_soh);
    endian_swaps(&un_swap_seeth_PC_data.st_data.st_se_mvb.u16_bat_chI);
    endian_swaps(&un_swap_seeth_PC_data.st_data.st_se_mvb.u16_bat_dischI);
    endian_swaps(&un_swap_seeth_PC_data.st_data.st_se_mvb.u16_bat_max_volt);
    endian_swaps(&un_swap_seeth_PC_data.st_data.st_se_mvb.u16_bat_min_volt);
    endian_swaps(&un_swap_seeth_PC_data.st_data.st_se_mvb.u16_bat_max_temp);
    endian_swaps(&un_swap_seeth_PC_data.st_data.st_se_mvb.u16_bat_min_temp);
    endian_swaps(&un_swap_seeth_PC_data.st_data.st_se_mvb.u16_bat_max_R);
    endian_swaps(&un_swap_seeth_PC_data.st_data.st_se_mvb.u16_bat_min_R);
    endian_swaps(&un_swap_seeth_PC_data.st_data.st_se_mvb.u16_bat_avg_volt);
    endian_swaps(&un_swap_seeth_PC_data.st_data.st_se_mvb.u16_bat_avg_R);

   
    for ( i = 0; i < TEST_BAT_NUM; i++)
    {
        endian_swaps(&un_swap_seeth_PC_data.st_data.st_batcore_data.u16_batcore_volt[i]);
        endian_swaps(&un_swap_seeth_PC_data.st_data.st_batcore_data.u16_batcore_temp[i]);
        endian_swaps(&un_swap_seeth_PC_data.st_data.st_batcore_data.u16_batcore_R[i]);
    }
   // un_swap_seeth_PC_data.st_data.un_batcore_err = un_batcore_err;
 
	
	//.................发送数据................................................
	sendbuf = tcp_get_buf(SE_PCETH_LEN );
	memcpy (sendbuf, un_swap_seeth_PC_data.u8_data, SE_PCETH_LEN );
    tcp_send (socket_tcp, sendbuf, SE_PCETH_LEN );
}

/*******************************************************************************
** 函数名称：
** 函数描述：发送PC端的ETH数据更新
** 输入参数：
** 返回值  ：无
*******************************************************************************/
static void se_eth_PC_update(void)
{
    struct  tm    now_tm;
    uint8_t i = 0;

    if(un_seeth_PC_data.st_data.u16_life < 0XFFFF)
    {
        un_seeth_PC_data.st_data.u16_life++;
    }
    else  un_seeth_PC_data.st_data.u16_life = 0;
    un_seeth_PC_data.st_data.u16_resv = 0;

    un_seeth_PC_data.st_data.st_se_mvb.u8_life = 0;
	 un_seeth_PC_data.st_data.st_se_mvb.u8_err_bat_num = st_bat_data.u16_err_bat_num;
    un_seeth_PC_data.st_data.st_se_mvb.u16_bat_volt = st_bat_data.fl_bat_volt * 10;
    un_seeth_PC_data.st_data.st_se_mvb.u16_bat_soc = st_bat_data.fl_bat_soc;
    un_seeth_PC_data.st_data.st_se_mvb.u16_bat_soh = st_bat_data.fl_bat_soh;
    un_seeth_PC_data.st_data.st_se_mvb.u16_bat_chI = st_bat_data.fl_bat_chI * 10;
    un_seeth_PC_data.st_data.st_se_mvb.u16_bat_dischI = st_bat_data.fl_bat_dischI * 10;
    un_seeth_PC_data.st_data.st_se_mvb.u16_bat_max_volt = st_bat_data. u16_bat_max_volt;
    un_seeth_PC_data.st_data.st_se_mvb.u16_bat_min_volt = st_bat_data.u16_bat_min_volt;
    un_seeth_PC_data.st_data.st_se_mvb.u16_bat_max_temp = (st_bat_data.fl_bat_max_temp + 55) *10;
    un_seeth_PC_data.st_data.st_se_mvb.u16_bat_min_temp = (st_bat_data.fl_bat_min_temp + 55) *10;
    un_seeth_PC_data.st_data.st_se_mvb.u16_bat_max_R = st_bat_data.u16_bat_max_R;
    un_seeth_PC_data.st_data.st_se_mvb.u16_bat_min_R = st_bat_data.u16_bat_min_R;
    un_seeth_PC_data.st_data.st_se_mvb.u16_bat_avg_volt = st_bat_data.u16_bat_avg_volt;
    un_seeth_PC_data.st_data.st_se_mvb.u16_bat_avg_R = st_bat_data.u16_bat_avg_R;
	un_seeth_PC_data.st_data.st_se_mvb.u8_bat_err[0] = un_bat_err.u16_all & 0X00FF ;
	un_seeth_PC_data.st_data.st_se_mvb.u8_bat_err[1] =((un_bat_err.u16_all & 0XFF00) >> 8);
	un_seeth_PC_data.st_data.st_se_mvb.u8_softversion[0] = 0x01;	 //V1.0
	un_seeth_PC_data.st_data.st_se_mvb.u8_softversion[1] = 0x00; 
   
    for ( i = 0; i < TEST_BAT_NUM; i++)
    {
        un_seeth_PC_data.st_data.st_batcore_data.u16_batcore_volt[i] = st_batcore_data.u16_batcore_volt[i];
        un_seeth_PC_data.st_data.st_batcore_data.u16_batcore_temp[i] = st_batcore_data.u16_batcore_temp[i];
        un_seeth_PC_data.st_data.st_batcore_data.u16_batcore_R[i] = st_batcore_data.u16_batcore_R[i];
    }
    un_seeth_PC_data.st_data.un_batcore_err = un_batcore_err;
    
    get_rtc_time( &now_tm);
    un_seeth_PC_data.st_data.u8_uptime[0] = now_tm.tm_year - 100;
    un_seeth_PC_data.st_data.u8_uptime[1] = now_tm.tm_mon + 1;
    un_seeth_PC_data.st_data.u8_uptime[2] = now_tm.tm_mday;
    un_seeth_PC_data.st_data.u8_uptime[3] = now_tm.tm_hour;
    un_seeth_PC_data.st_data.u8_uptime[4] = now_tm.tm_min; 
    un_seeth_PC_data.st_data.u8_uptime[5] = now_tm.tm_sec;

    un_seeth_PC_data.st_data.u8_bat_num = st_product_preset.u8_bat_num;
    un_seeth_PC_data.st_data.u8_car_num = st_product_preset.u8_car_num;
    un_seeth_PC_data.st_data.st_KM_bit = st_KM_bit;
    un_seeth_PC_data.st_data.u8_ch_overI = st_product_preset.u8_ch_overI;
    un_seeth_PC_data.st_data.u8_disch_overI = st_product_preset.u8_disch_overI;
    un_seeth_PC_data.st_data.u8_resv1 = 0x00;
}

