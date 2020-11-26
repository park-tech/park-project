#ifndef _THREAD_TCP_REV_H_
#define _THREAD_TCP_REV_H_

//..........................ͷ�ļ�.......................................



#include <rtthread.h>
//#include <sys/socket.h> /* ʹ��BSD socket����Ҫ����sockets.hͷ�ļ� */
#include <string.h>
#include <finsh.h>
#include "fmod_data_process.h"   

/******************************
		������ܵ�ѹ
		������
		�ŵ����
		�����¶�
		��ص�ǰ�ɳ���������
		��ص�ǰ����
		SOC���ʣ������
		SOH��ؽ���״̬
		��ص���(ģ��)��ߵ�ѹ
		��ص��壨ģ�飩��͵�ѹ
		��ص��壨ģ�飩����¶�
		��ص��壨ģ�飩����¶�
		��ߵ�ѹ���壨ģ�飩���
		��͵�ѹ���壨ģ�飩���
		����¶ȵ��壨ģ�飩���
		����¶ȵ��壨ģ�飩���
		���壨ģ�飩�¶ȴ��������ϱ��
		���壨ģ�飩������߱��
		��ѹ�����߶Ͽ����
		Ԥ��
		Ԥ��
		��������
		��������
		����������
		��طŵ����
		�����S0C��ѹԤ��
		�������
		Ԥ��
		Ԥ��
		���壨ģ�飩��ع���
		���壨ģ�飩��ع���
		���壨ģ�飩��ع��¹���
		���壨ģ�飩��ѹ�����߶Ͽ�����
		�¶ȴ���������
		���壨ģ�飩���ѹ�����
		�²�޹���
		���壨ģ�飩�������
		�Լ����
		�ڲ�ͨѶ����
		MVBͨѶ����
		ETHͨѶ����
		��������������
		Ԥ��
		Ԥ��
		Ԥ��
		Ԥ��
		Ԥ��
		Ԥ��
		Ԥ��
		Ԥ��
		Ԥ��
		Ԥ��
		�ܹ���ָʾ
		���壨ģ�飩��ص�ѹ(1-52)
		���壨ģ�飩����¶�(1-52)
		���壨ģ�飩�������(1-52)
		���Ե�ص�����
		����
		��Ʒ���
		������ֵ
		�ŵ����ֵ
		��
		��
		��
		ʱ
		��
		��
***********************/



#define SE_REVETH_LEN       38   //===���͵��ֽ���(��ʱ)
#define RE_PCETH_LEN        32
//................................��������.......................................

//...............................ETHͨѶ��Ϣ................................ 
//.............................����PC�˵�ETH����..........................

struct Re_eth_message
{   
	uint16_t  u16_life;
	uint16_t  u16_resv;
	uint8_t  u8_data[8];

};


union Re_eth_PC_data
{
	struct   Re_eth_message  st_data;
  uint8_t  u8_data[32];       
};


//.....................���͵�ETH����.....................
struct Send_eth_message
{   
	uint16_t   u16_product_num;       //��Ʒ���
  uint8_t    u8_softverdtion[2];    //===�����汾��

	
};


union  Se_eth_PC_data
{
	struct   Send_eth_message  st_data;
  uint8_t  u8_data[632];        //send_eth_packdata
};


extern  union   Re_eth_PC_data   un_reeth_PC_data;
extern  union   Se_eth_PC_data   un_seeth_PC_data;


#endif