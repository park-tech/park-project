/******************************************************************************************
@title	             	����20BMS M4 thread_tcp_rev.c
@funcation            ��̫����ȡ�汾��Ϣ    (�汾��Ϣ������Ӧ�ó���汾��bootloader�汾��Ӳ���汾��)
@author	            	wentao
@since		            MDK 5.31
@version	            1.0
@remark		                    
@time                 2020/11/09
@site     
_______________________________________________________________________________________________________________________________________________________
|ʱ��                 |     ����                     |            ��������              |   ���Ⱑ������                                              |
|��������������������������������������������������������������������������������������������������������|��������������������������������������������������������������������|��������������������������������������������������������������������������������������������������������������������������|
|2020.11.09 ��        |             �½�             |һ��� ��������                   |һ��� ��������                                              |
|2020.11.10 ��        |     �������                 |һ��� ��������                   |                                                             |
|2020.11.11 ��        |         ���������ӻ���       |                                  |��ά���̲߳���ͬʱ�򿪣�����                                 |
|2020.11.12 ��        |            ���ݷ���OK��      |������̫���������ݡ�versionread���� |��ά���̲߳���ͬʱ�򿪣�����                                 |
|                     |                              |��ظ��汾��Ϣ                    |                                                             |
|2020.11.12.16��46��  |���ܼӰ�ojbk                  |��ϸд�ϰ汾�ţ�bug               |�������ǲ��ǿ��Ź������⣬������λ������                     |
|                     |                              |                                  |�پ����� re_pc_eth_thread_init����ͬʱ����������             |
|           18: 00:   |                              |init_TcpNet()���⵼������һ��     |���⣿����������������                                       |
|                     |                              |��̫���̲߳���ͬʱ���������Ź�����|                                                             |
|                     |                              | ��ʱι��                         |                                                             |
|2020.11.13.08:15:    |                              |                                  |��̫����M4������ݣ�һ��ʱ���������̫���������Ӳ��ϣ�     |
|           14:30��   |sendbuf��̬�ڴ����������   |                                  |                                                             |
|                     |sendbuf��ָ���ʱ��Ҫ�ж�ָ |                                  |                                                             |
|                     |���ǲ��ǿյ�                  |                                  |                                                             |
|                     |                              |                                  |                                                             |
|                     |                              |                                  |                                                             |
|                     |                              |                                  |                                                             |
������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������|


PC�˲�����Ϣ��   
	 IP��ַ��192.168.0.10;
	 �˿ںţ�6111��	
M4�˲�����Ϣ��   
	 IP��ַ��192.168.0.200;
	 �˿ںţ�6103��versionread
ͨѶ��ʽ��   
	 �ʴ�ʽ������λ������ѯ��M4�İ汾�ź�ָ�M4���յ���ѯ��ָ���ظ��汾��Ϣ
��λ������ָ�����ݣ�
				"version_read"
M4��ظ��汾��Ϣ��
	 �汾��Ϣ������Ӧ�ó���汾��bootloader�汾��Ӳ���汾�� 
	 �����ʽ��"ZZaxxxxxxxxx;ZZbxxxxxxx;ZZ3.226.xxxx;"    ���������������淶
	 ��ÿһ���汾֮���á�;��������������������������汾��Ϣ��Ҫ��ӣ����ڡ�;�����������ӣ�
*******************************************************************************************/
/******************************************************************************************
** ͷ�ļ�
*******************************************************************************************/

#include "thread_tcp_rev.h"

/******************************************************************************************
** �궨��
*******************************************************************************************/

#define  REV_PCETH_THREAD_STACK_SZ        4096         // �߳�ջ��С����λ���ֽڡ�
//#define  RE_PCETH_THREAD_PRIO            21           // ���ȼ� ��Χ�Ǵ� 0 �� 255����ֵԽС���ȼ�Խ�ߣ�0 ����������ȼ�
//#define  RE_PCETH_THREAD_TIMESLICE       30           // ʱ��Ƭ��С������һ�ε����ܹ����е����ʱ�䳤��
#define  rev_LocalPort_NUM                6103
#define  rev_RemotePort_NUM               6111
#define  IP1                              192
#define  IP2                              168
#define  IP3                              0
#define  IP4                              10

#define  BUFSZ                            1024


/********************************************************************************************
** �������
********************************************************************************************/

static const char send_data[] = "ZZa_PD19011_A001.V1.0_20201116;ZZb*******;ZZ3.226.931DLB";   //===���򣡣�����Ҫ��������Ϣ����

static const char rec_data[] = {0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x5f,0x72,0x65,0x61,0x64};   //===��version_read��
/* �����õ������� */   /*�汾��Ϣ������Ӧ�ó���汾��bootloader�汾��Ӳ���汾��*/
static struct rt_thread rev_re_pc_eth_thread;
static rt_uint8_t rev_re_pc_eth_thread_stack[REV_PCETH_THREAD_STACK_SZ];

static rt_uint8_t socket_rev;          //===
static rt_uint8_t remote_rev_ip[4] = {IP1,IP2,IP3,IP4};
static int tcp_connect_flag = 0;       //===�������ӱ�־λ
static int flag = 0;
/********************************************************************************************
** ��������
********************************************************************************************/

static rt_uint16_t tcp_callback (rt_uint8_t soc, rt_uint8_t evt, rt_uint8_t *ptr, rt_uint16_t par);  //===??
static void rev_se_eth_PC_data(void);  //===��������


/*******************************************************************************
** �������ƣ�
** ��������������udpserv���ݷ����߳����
** ���������
** ����ֵ  ����
**
*******************************************************************************/
static void thread_entry_rev_re_pc_eth(void *param)
{

    int  res = 0;
//.........................TCPЭ���ʼ��.............................
//    init_TcpNet (); 
	rt_thread_mdelay(4000);
 //........................����socket..................................
	socket_rev = tcp_get_socket (TCP_TYPE_CLIENT | TCP_TYPE_KEEP_ALIVE, 0, 10, tcp_callback);
	if(socket_rev != 0)   //����ͬTCP socket�ɹ�
	{
			rt_kprintf("TCP Socket rev creat ok!\n");
			res = tcp_connect (socket_rev, remote_rev_ip, rev_RemotePort_NUM, rev_LocalPort_NUM);
			if(res)  rt_kprintf("TCP socket rev creat done! res = %d\r\n", res);
			else rt_kprintf("TCP socket rev creat err! res = %d\r\n", res);
	}

	while (1)
	{
        
		if(tcp_connect_flag) //socket���ӳɹ������յ����ݣ����������ûص�������
	
		{		
		//.......................��������.........................	
			if(flag)	
			{	
				rev_se_eth_PC_data( );      //===�յ��Ǹ�ɶ�ӵ�ʱ�򣬷�������  	
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
** �������ƣ�     
** �����������̵߳ĳ�ʼ��
** �����������
** ����ֵ  ��    ��
**
********************************************************************************************/

int rev_re_pc_eth_thread_init(void)
{

	// .....................................��̬�̳߳�ʼ��.......................................
	rt_thread_init( &rev_re_pc_eth_thread,                                   // �߳̽ṹ��
									"rev_pc_eth",                                         // �߳�����
									thread_entry_rev_re_pc_eth, RT_NULL,                     // �߳���ں������ƺͲ���
									rev_re_pc_eth_thread_stack, REV_PCETH_THREAD_STACK_SZ,      // ջ����ʼλ�úʹ�С 
									23,  30);    // �߳����ȼ���ʱ��Ƭ��С
	 //..................................... �����߳� �������߳���ں���...........................
	rt_thread_startup(&rev_re_pc_eth_thread);
    return 0 ;    
}
INIT_COMPONENT_EXPORT(rev_re_pc_eth_thread_init);

/*******************************************************************************
** �������ƣ�
** ����������tcp�ص�����
** ���������
** ����ֵ  ����
**
*******************************************************************************/
static rt_uint16_t tcp_callback (rt_uint8_t soc, rt_uint8_t evt, rt_uint8_t *ptr, rt_uint16_t par)
{

    char buf[50];
    rt_uint16_t i;

/* ȷ����socket_rev�Ļص� */

    if (soc != socket_rev) 
    {
        return (0);
    }
	/*
		Զ�̿ͻ���������Ϣ
			1������ptr�洢Զ���豸��IP��ַ��par�д洢�˿ںš�
			2��������ֵ1�������ӣ�������ֵ0��ֹ���ӡ�
*/
switch (evt) 
{
	case TCP_EVT_CONREQ:
		sprintf(buf, "[remote client][IP: %d.%d.%d.%d] connectting ... ", ptr[0], ptr[1], ptr[2], ptr[3]);
		rt_kprintf("IP:%s  port:%d\r\n", buf, par);
		return (1);
	
	/* ������ֹ */
	case TCP_EVT_ABORT:
		tcp_connect_flag =0;
		break;
	
	/* SocketԶ�������Ѿ����� */
	case TCP_EVT_CONNECT:
		rt_kprintf("tcp Socket rev connected remote success!\r\n"); 
			tcp_connect_flag =1;
		break;
	
	/* ���ӶϿ� */
	case TCP_EVT_CLOSE:
			rt_kprintf("tcp rev connection has been closed!\r\n");
			tcp_connect_flag =0;
		break;
	
	/* ���͵������յ�Զ���豸Ӧ�� */
	case TCP_EVT_ACK:
		break;
	/* ���յ�TCP����֡��ptrָ�����ݵ�ַ��par��¼���ݳ��ȣ���λ�ֽ� */
	case TCP_EVT_DATA:
        
//===rt_kprintf("Data length = %d\r\n", par);
//===rt_kprintf("[hex]:");
    
     if(par==12)
	 {
		for(i = 0; i < par; i++)
		{       
				if(ptr[i]!=rec_data[i])       //===�ж����յ������ǲ���version_read
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
** �������ƣ�se_eth_PC_rev(void)
** ��������������PC�˵İ汾��Ϣ
** ���������
** ����ֵ  ����
*******************************************************************************/
//static void se_eth_PC_data(void)
static void rev_se_eth_PC_data(void)
{
 
	rt_uint8_t *sendbuf;
 
	//.................��������................................................
	sendbuf = tcp_get_buf(SE_REVETH_LEN );     //===���͵��ֽ�����ͬʱ���붯̬�ڴ棩
	if(RT_NULL != sendbuf)                     //===��ָ���ʱ��Ҫ�ж�ָ���ǲ��ǿյ�
	{
		memcpy (sendbuf, send_data, SE_REVETH_LEN );
		tcp_send (socket_rev, sendbuf, SE_REVETH_LEN );     //===�������� ��ͬʱ�ͷŶ�̬�ڴ棩         
	}
	else
	{
		sendbuf = rt_malloc(26);
		__asm("nop");
		rt_free(sendbuf);
	}
   flag=0;
/*
memcpy �� �ڴ濽������
����ԭ��Ϊvoid *memcpy(void *destin, void *source, unsigned n)��
�����Ĺ����Ǵ�Դ�ڴ��ַ����ʼλ�ÿ�ʼ�������ɸ��ֽڵ�Ŀ���ڴ��ַ�У�
����Դsource�п���n���ֽڵ�Ŀ��destin��
������
destin-- ָ�����ڴ洢�������ݵ�Ŀ�����飬����ǿ��ת��Ϊ void* ָ�롣
source-- ָ��Ҫ���Ƶ�����Դ������ǿ��ת��Ϊ void* ָ�롣
n-- Ҫ�����Ƶ��ֽ�����
����ͷ�ļ���
C���ԣ�#include<string.h>
*/
//	tcp_send (socket_rev, sendbuf, SE_REVETH_LEN );     //===��������

/*
socket_tcp:   ��TCP�׽��ַ������ݰ�
sendbuf:      ָ�����Ҫ���͵����ݵĻ�������ָ��
SE_PCETH_LEN: Ҫ���͵������ֽ���
*/

}
