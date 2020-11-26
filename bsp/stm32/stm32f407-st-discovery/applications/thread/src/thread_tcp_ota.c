#include <string.h>
#include <stdio.h>
#include "rtdbg.h"
#include "bsp_bkpreg.h"
#include "thread_tcp_ota.h"
#include "RTL.h"
#include "board.h"
#pragma diag_suppress  111
#pragma diag_suppress  177
/* 1：添加模块:thread_tcp_ota.c以及bsp_bkpreg.c；
   2：options for target--->linker---->editd对程序地址进行偏移设置
   3: 修改程序地址空间：LR_IROM1 0x08040000 0x000c0000
                        ER_IROM1 0x08040000 0x000c0000
   4：修改system_stm32fxx.c:#define VECT_TAB_OFFSET  0x40000 ,调节中断向量偏移
   5: 修改drv_common.c:__set_PRIMASK(0);  修改为__set_PRIMASK(1)
   
   app中:刷写指令:
   1:以太网下发:download app;
   2:下发restart,重启系统,进入bootloader程序,
   boot中:
   1:下发文件名称:filename:zza_xxx.bin;
   2:下发restart,重启系统,进入bootloader程序,
   
*/
#define THREAD_STACK_SIZE   2048         // 栈空间
ALIGN(RT_ALIGN_SIZE)
CCMRAM static rt_uint8_t thread_ota_tx_stack[THREAD_STACK_SIZE];    // 线程栈
ALIGN(RT_ALIGN_SIZE)
CCMRAM static rt_uint8_t thread_ota_rx_stack[THREAD_STACK_SIZE];    // 线程栈

CCMRAM static struct rt_thread st_ota_rx_thread;        // 线程结构体
CCMRAM static struct rt_thread st_ota_tx_thread;        // 线程结构体
struct rt_event tcpACKevent;
struct rt_semaphore link_down_sem;   //信号量
struct rt_semaphore ota_rx_sem;   //信号量
#define client
#define TCP_ETH
#define otaPort_NUM         6102    //  bootload使用端口
#define RemotePort_NUM      6110    // 
#define IP1                 192
#define IP2                 168
#define IP3                 0
#define IP4                 10
static rt_uint8_t socket_eth;
static rt_uint8_t tcp_connect_flag = 0;
CCMRAM rt_uint8_t tcp_recv_buf[1200];
static rt_uint16_t tcp_rec_len;
#define EVENT_FLAG0 (1 << 0)
#define EVENT_FLAG1 (1 << 1)
#define EVENT_FLAG2 (1 << 2)
#define EVENT_FLAG3 (1 << 3)
#define EVENT_FLAG4 (1 << 4)
#define EVENT_FLAG5 (1 << 5)
#define EVENT_FLAG6 (1 << 6)
#define EVENT_FLAG7 (1 << 7)
const char * tcp_cmd_Table[]= 
{
	" cmd1:download app     ",
	" cmd2:restart          ",
	" filename:zza_xxxx.bin ",
	" delete zza_xxxx.bin   "
};
const char * tcp_ack_Table[] =
{
    "cmd excuted!",
    "reboot system...",
    "help...",
    "wiping app zoom...",
    "wiped ok...",
    "programing....",
    "programed ok..",
};
// 回调函数
static rt_uint16_t ota_callback (rt_uint8_t socket, rt_uint8_t evt, rt_uint8_t *ptr, rt_uint16_t par)
{
	char buf[50];
	
	if (socket != socket_eth)  /* 确保是socket_eth的回调 */
	{
		return (0);
	}
	switch (evt) 
	{
		case TCP_EVT_CONREQ:
			sprintf(buf, "[remote client][IP: %d.%d.%d.%d] connectting ... ", ptr[0], ptr[1], ptr[2], ptr[3]);
			LOG_D("IP:%s  port:%d\r\n", buf, par);
			return (1);
		/* 连接终止 */
		case TCP_EVT_ABORT:
            tcp_connect_flag = 0;
			break;
		/* Socket远程连接已经建立 */
		case TCP_EVT_CONNECT:
			LOG_D("Socket is connected to remote peer\r\n");
            tcp_connect_flag = 1;
			break;
		/* 连接断开 */
		case TCP_EVT_CLOSE:
		   	LOG_D("Connection has been closed\r\n");
            tcp_connect_flag = 0;
			break;
		/* 发送的数据收到远程设备应答 */
		case TCP_EVT_ACK:
        break;
		
		/* 接收到TCP数据帧，ptr指向数据地址，par记录数据长度，单位字节 */
		case TCP_EVT_DATA:
            // 将数据放入buff,释放信号量,通知接收线程处理接收到的数据
			tcp_rec_len = par;
            memcpy(tcp_recv_buf,ptr,tcp_rec_len);
			rt_sem_release(&ota_rx_sem);
			break;
	}
	return (0);
}
// 客户端连接到服务器
static void tcp_connect2serv(void)
{
    int res;
    static rt_uint8_t Rem_IP[4] = {IP1,IP2,IP3,IP4};
    tcp_abort (socket_eth);
    tcp_close (socket_eth);
    tcp_release_socket (socket_eth);
    rt_thread_mdelay(500);
    socket_eth = tcp_get_socket (TCP_TYPE_CLIENT | TCP_TYPE_KEEP_ALIVE, 0, 50, ota_callback);
    if(socket_eth != 0)
    {
        res = tcp_connect (socket_eth, Rem_IP, RemotePort_NUM, otaPort_NUM);
       // LOG_D("TCP Socket creat done res = %d\r\n", res);
    }
    rt_thread_mdelay(500);
}
// 网线断开检测，网线断开后，释放一个信号量link_down_sem，
static void link_down_check(void)
{
    if(RT_EOK ==rt_sem_take(&link_down_sem, 5))
    {
        tcp_connect_flag = 0;
    }
}
static void tcp_ota_ack(void)
{
    rt_uint32_t e;
    rt_uint8_t *sendbuf;
    static uint16_t len;
    if(rt_event_recv(&tcpACKevent, EVENT_FLAG0 ,
                      RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                      5, &e) == RT_EOK) // 应答命令正确
    {
        len = strlen(tcp_ack_Table[0]);
        sendbuf = tcp_get_buf (len);
        memcpy(sendbuf,tcp_ack_Table[0],len);
        tcp_send (socket_eth, sendbuf, len);
        __asm("nop");
    }
    if(rt_event_recv(&tcpACKevent, EVENT_FLAG1 ,
                  RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                  5, &e) == RT_EOK) // reboot system....
    {
        len = strlen(tcp_ack_Table[1]);
        sendbuf = tcp_get_buf(len);
        memcpy(sendbuf,tcp_ack_Table[1],len);
        tcp_send (socket_eth, sendbuf, len);
        __asm("nop");
    }
}
/**
  **********************************************************************************
  * @funcation  :
  * @author     :ZhouLH
  * @date       :20200227
  * @attention  :
  **********************************************************************************
  */
static void thread_entry_eth_tx(void *param)
{
    rt_thread_mdelay(3000); // 等待其它线程对以太网模块初始化

    while(RT_TRUE)
    {
        if(0 == tcp_connect_flag)  tcp_connect2serv();   // 连接正常侦测
        else    
        {
            //rt_thread_mdelay(200);
            tcp_ota_ack();
            link_down_check();
        }
    }
    return ;
}
static void tcp_ota_rountine(void)
{
    int res;
    rt_uint8_t *sendbuf;
    rt_uint8_t Rem_IP[4] = {IP1,IP2,IP3,IP4};
    rt_sem_take(&ota_rx_sem, 1000);// 等待信号量，超时1000ms退出
    if(tcp_rec_len>0)
    {
        if(0==strncmp((const char*)tcp_recv_buf,"download app",tcp_rec_len))
        {// 下载程序命令，需要写备份寄存器
            rt_event_send(&tcpACKevent,EVENT_FLAG0);
            download_app_mark_set();
        }
        else if(0==strncmp((const char*)tcp_recv_buf,"restart",tcp_rec_len))
        {   // 重启系统,程序刷写完成才允许重启
            rt_event_send(&tcpACKevent,EVENT_FLAG1);
            rt_thread_mdelay(400);
            rt_hw_cpu_reset();
            __asm("nop");
        }
    }
    tcp_rec_len = 0;
}
/**
  **********************************************************************************
  * @funcation  :
  * @author     :ZhouLH
  * @date       :20200227
  * @attention  :
  **********************************************************************************
  */
static void thread_entry_eth_rx(void *param)
{
    static uint8_t tick_time =0;
    rt_thread_mdelay(800);
    while(RT_TRUE)
    {
        tcp_ota_rountine();
        tick_time++;
        if(tick_time > 5)
        {
            tick_time = 0;
            start_times_clear();
        }
    }
}
/**
  **********************************************************************************
  * @funcation  :int thread_init_ota(void)
  * @author     :ZhouLH
  * @date       :20200218
  * @brief      :
  * @attention  :
  **********************************************************************************
  */
int thread_init_ota(void)
{ 
    // 初始化6个信息队列,分别用于存放6个通道的数据
    rt_sem_init(&link_down_sem, "link_down_sem", 0, RT_IPC_FLAG_FIFO);  // 初始化信号量
    rt_sem_init(&ota_rx_sem, "ota_rx_sem", 0, RT_IPC_FLAG_FIFO);  // 初始化信号量
    rt_event_init(&tcpACKevent,"tcpACKevent",RT_IPC_FLAG_FIFO);            
    rt_thread_init( &st_ota_rx_thread,
                    "eth_recv",
                    thread_entry_eth_rx, RT_NULL,
                    thread_ota_rx_stack, THREAD_STACK_SIZE,
                    15,  5);     // 线程优先级，时间片大小
    rt_thread_startup(&st_ota_rx_thread);  // 启动线程
    
    rt_thread_init( &st_ota_tx_thread,
                    "eth_send",
                    thread_entry_eth_tx, RT_NULL,
                    thread_ota_tx_stack, THREAD_STACK_SIZE,
                    15,  5);    // 线程优先级，时间片大小
    rt_thread_startup(&st_ota_tx_thread);  // 启动线程
	return  0;
}
INIT_COMPONENT_EXPORT(thread_init_ota);

