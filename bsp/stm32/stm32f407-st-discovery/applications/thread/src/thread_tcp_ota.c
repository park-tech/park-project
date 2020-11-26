#include <string.h>
#include <stdio.h>
#include "rtdbg.h"
#include "bsp_bkpreg.h"
#include "thread_tcp_ota.h"
#include "RTL.h"
#include "board.h"
#pragma diag_suppress  111
#pragma diag_suppress  177
/* 1�����ģ��:thread_tcp_ota.c�Լ�bsp_bkpreg.c��
   2��options for target--->linker---->editd�Գ����ַ����ƫ������
   3: �޸ĳ����ַ�ռ䣺LR_IROM1 0x08040000 0x000c0000
                        ER_IROM1 0x08040000 0x000c0000
   4���޸�system_stm32fxx.c:#define VECT_TAB_OFFSET  0x40000 ,�����ж�����ƫ��
   5: �޸�drv_common.c:__set_PRIMASK(0);  �޸�Ϊ__set_PRIMASK(1)
   
   app��:ˢдָ��:
   1:��̫���·�:download app;
   2:�·�restart,����ϵͳ,����bootloader����,
   boot��:
   1:�·��ļ�����:filename:zza_xxx.bin;
   2:�·�restart,����ϵͳ,����bootloader����,
   
*/
#define THREAD_STACK_SIZE   2048         // ջ�ռ�
ALIGN(RT_ALIGN_SIZE)
CCMRAM static rt_uint8_t thread_ota_tx_stack[THREAD_STACK_SIZE];    // �߳�ջ
ALIGN(RT_ALIGN_SIZE)
CCMRAM static rt_uint8_t thread_ota_rx_stack[THREAD_STACK_SIZE];    // �߳�ջ

CCMRAM static struct rt_thread st_ota_rx_thread;        // �߳̽ṹ��
CCMRAM static struct rt_thread st_ota_tx_thread;        // �߳̽ṹ��
struct rt_event tcpACKevent;
struct rt_semaphore link_down_sem;   //�ź���
struct rt_semaphore ota_rx_sem;   //�ź���
#define client
#define TCP_ETH
#define otaPort_NUM         6102    //  bootloadʹ�ö˿�
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
// �ص�����
static rt_uint16_t ota_callback (rt_uint8_t socket, rt_uint8_t evt, rt_uint8_t *ptr, rt_uint16_t par)
{
	char buf[50];
	
	if (socket != socket_eth)  /* ȷ����socket_eth�Ļص� */
	{
		return (0);
	}
	switch (evt) 
	{
		case TCP_EVT_CONREQ:
			sprintf(buf, "[remote client][IP: %d.%d.%d.%d] connectting ... ", ptr[0], ptr[1], ptr[2], ptr[3]);
			LOG_D("IP:%s  port:%d\r\n", buf, par);
			return (1);
		/* ������ֹ */
		case TCP_EVT_ABORT:
            tcp_connect_flag = 0;
			break;
		/* SocketԶ�������Ѿ����� */
		case TCP_EVT_CONNECT:
			LOG_D("Socket is connected to remote peer\r\n");
            tcp_connect_flag = 1;
			break;
		/* ���ӶϿ� */
		case TCP_EVT_CLOSE:
		   	LOG_D("Connection has been closed\r\n");
            tcp_connect_flag = 0;
			break;
		/* ���͵������յ�Զ���豸Ӧ�� */
		case TCP_EVT_ACK:
        break;
		
		/* ���յ�TCP����֡��ptrָ�����ݵ�ַ��par��¼���ݳ��ȣ���λ�ֽ� */
		case TCP_EVT_DATA:
            // �����ݷ���buff,�ͷ��ź���,֪ͨ�����̴߳�����յ�������
			tcp_rec_len = par;
            memcpy(tcp_recv_buf,ptr,tcp_rec_len);
			rt_sem_release(&ota_rx_sem);
			break;
	}
	return (0);
}
// �ͻ������ӵ�������
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
// ���߶Ͽ���⣬���߶Ͽ����ͷ�һ���ź���link_down_sem��
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
                      5, &e) == RT_EOK) // Ӧ��������ȷ
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
    rt_thread_mdelay(3000); // �ȴ������̶߳���̫��ģ���ʼ��

    while(RT_TRUE)
    {
        if(0 == tcp_connect_flag)  tcp_connect2serv();   // �����������
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
    rt_sem_take(&ota_rx_sem, 1000);// �ȴ��ź�������ʱ1000ms�˳�
    if(tcp_rec_len>0)
    {
        if(0==strncmp((const char*)tcp_recv_buf,"download app",tcp_rec_len))
        {// ���س��������Ҫд���ݼĴ���
            rt_event_send(&tcpACKevent,EVENT_FLAG0);
            download_app_mark_set();
        }
        else if(0==strncmp((const char*)tcp_recv_buf,"restart",tcp_rec_len))
        {   // ����ϵͳ,����ˢд��ɲ���������
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
    // ��ʼ��6����Ϣ����,�ֱ����ڴ��6��ͨ��������
    rt_sem_init(&link_down_sem, "link_down_sem", 0, RT_IPC_FLAG_FIFO);  // ��ʼ���ź���
    rt_sem_init(&ota_rx_sem, "ota_rx_sem", 0, RT_IPC_FLAG_FIFO);  // ��ʼ���ź���
    rt_event_init(&tcpACKevent,"tcpACKevent",RT_IPC_FLAG_FIFO);            
    rt_thread_init( &st_ota_rx_thread,
                    "eth_recv",
                    thread_entry_eth_rx, RT_NULL,
                    thread_ota_rx_stack, THREAD_STACK_SIZE,
                    15,  5);     // �߳����ȼ���ʱ��Ƭ��С
    rt_thread_startup(&st_ota_rx_thread);  // �����߳�
    
    rt_thread_init( &st_ota_tx_thread,
                    "eth_send",
                    thread_entry_eth_tx, RT_NULL,
                    thread_ota_tx_stack, THREAD_STACK_SIZE,
                    15,  5);    // �߳����ȼ���ʱ��Ƭ��С
    rt_thread_startup(&st_ota_tx_thread);  // �����߳�
	return  0;
}
INIT_COMPONENT_EXPORT(thread_init_ota);

