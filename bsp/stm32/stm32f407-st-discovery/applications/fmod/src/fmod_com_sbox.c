/******************************************************************************************
** 头文件
*******************************************************************************************/
#include "fmod_com_sbox.h"

/********************************************************************************************
** 定义变量
********************************************************************************************/

union   Can_re_sbox_data      un_re_sbox_data[SUM_BAT_NUM_MAX];
 		
/********************************************************************************************
** 函数申明
********************************************************************************************/
static void fmod_get_sbox_data(void);
//static void fmod_sbox_chooseVolt(int bt_index);
//static void fmod_sbox_chooseTemp(int bt_index);
//static void fmod_sbox_choosePassiveEquilibrium(int bt_index);
/********************************************************************************************
** 内容
********************************************************************************************/


/******************************************************************************************
** 函数名称：sbox的通讯的发送和接收
** 函数描述：
** 输入参数：
** 返回值  ：无
*******************************************************************************************/
void fmod_sbox_com(void)
{
	
//	fmod_get_sbox_data( );
	
}

/******************************************************************************************
函数描述：CAN接收采样盒数据校验
输入参数：
返回值：	  
*******************************************************************************************/
//static void fmod_get_sbox_data(void)
//{   

//}
//static void fmod_sbox_chooseVolt(int bt_index)
//{
//	switch (bt_index)
//		{
//			case 1:
//				{
//					MCU_38E1_PIN_OFF;
//					MCU_38A1_PIN_OFF;
//					MCU_38B1_PIN_ON ;
//					MCU_38C1_PIN_ON;
//					
//					MCU_38E2_PIN_OFF;
//					MCU_38A2_PIN_OFF;
//					MCU_38B2_PIN_ON ;
//					MCU_38C2_PIN_ON;
//				}break;
//				case 2:
//				{
//					MCU_38E1_PIN_OFF;
//					MCU_38A1_PIN_ON;
//					MCU_38B1_PIN_OFF ;
//					MCU_38C1_PIN_ON;
//					
//					MCU_38E2_PIN_OFF;
//					MCU_38A2_PIN_OFF;
//					MCU_38B2_PIN_ON ;
//					MCU_38C2_PIN_ON;
//				}break;
//				case 3:
//				{
//					MCU_38E1_PIN_OFF;
//					MCU_38A1_PIN_ON;
//					MCU_38B1_PIN_OFF;
//					MCU_38C1_PIN_ON;
//					
//					MCU_38E2_PIN_OFF;
//					MCU_38A2_PIN_ON;
//					
//					MCU_38B2_PIN_OFF ;
//					MCU_38C2_PIN_ON;
//				}break;
//				case 4:
//				{
//					MCU_38E1_PIN_OFF;
//					MCU_38A1_PIN_OFF;
//					MCU_38B1_PIN_OFF ;
//					MCU_38C1_PIN_ON;
//					
//					MCU_38E2_PIN_OFF;
//					MCU_38A2_PIN_ON;
//					MCU_38B2_PIN_OFF ;
//					MCU_38C2_PIN_ON;
//				}break;
//				case 5:
//				{
//					MCU_38E1_PIN_OFF;
//					MCU_38A1_PIN_OFF;
//					MCU_38B1_PIN_OFF ;
//					MCU_38C1_PIN_ON;
//					
//					MCU_38E2_PIN_OFF;
//					MCU_38A2_PIN_OFF;
//					MCU_38B2_PIN_OFF ;
//					MCU_38C2_PIN_ON;
//				}break;
//				case 6:
//				{
//					MCU_38E1_PIN_OFF;
//					MCU_38A1_PIN_ON;
//					MCU_38B1_PIN_ON ;
//					MCU_38C1_PIN_OFF;
//					
//					MCU_38E2_PIN_OFF;
//					MCU_38A2_PIN_OFF;
//					MCU_38B2_PIN_OFF ;
//					MCU_38C2_PIN_ON;
//				}break;
//				case 7:
//				{
//					MCU_38E1_PIN_OFF;
//					MCU_38A1_PIN_ON;
//					MCU_38B1_PIN_ON ;
//					MCU_38C1_PIN_OFF;
//					
//					MCU_38E2_PIN_OFF;
//					MCU_38A2_PIN_ON;
//					MCU_38B2_PIN_ON ;
//					MCU_38C2_PIN_OFF;
//				}break;
//				case 8:
//				{
//					MCU_38E1_PIN_OFF;
//					MCU_38A1_PIN_OFF;
//					MCU_38B1_PIN_ON ;
//					MCU_38C1_PIN_OFF;
//					
//					MCU_38E2_PIN_OFF;
//					MCU_38A2_PIN_ON;
//					MCU_38B2_PIN_ON ;
//					MCU_38C2_PIN_OFF;
//				}break;
//				case 9:
//				{
//					MCU_38E1_PIN_OFF;
//					MCU_38A1_PIN_OFF;
//					MCU_38B1_PIN_ON ;
//					MCU_38C1_PIN_OFF;
//					
//					MCU_38E2_PIN_OFF;
//					MCU_38A2_PIN_OFF;
//					MCU_38B2_PIN_ON ;
//					MCU_38C2_PIN_OFF;
//				}break;
//				case 10:
//				{
//					MCU_38E1_PIN_OFF;
//					MCU_38A1_PIN_ON;
//					MCU_38B1_PIN_OFF ;
//					MCU_38C1_PIN_OFF;
//					
//					MCU_38E2_PIN_OFF;
//					MCU_38A2_PIN_OFF;
//					MCU_38B2_PIN_ON ;
//					MCU_38C2_PIN_OFF;
//				}break;
//				case 11:
//				{
//					MCU_38E1_PIN_OFF;
//					MCU_38A1_PIN_ON;
//					MCU_38B1_PIN_OFF ;
//					MCU_38C1_PIN_OFF;
//					
//					MCU_38E2_PIN_OFF;
//					MCU_38A2_PIN_ON;
//					MCU_38B2_PIN_OFF ;
//					MCU_38C2_PIN_OFF;
//				}break;
//				case 12:
//				{
//					MCU_38E1_PIN_OFF;
//					MCU_38A1_PIN_OFF;
//					MCU_38B1_PIN_OFF ;
//					MCU_38C1_PIN_OFF;
//					
//					MCU_38E2_PIN_OFF;
//					MCU_38A2_PIN_ON;
//					MCU_38B2_PIN_OFF ;
//					MCU_38C2_PIN_OFF;
//				}break;
//				default:break;
//				
//		}
//}

//static void fmod_sbox_chooseTemp(int bt_index)
//{
//	switch (bt_index)
//		{
//			case 1:
//				{
//					MCU_67E_PIN_OFF;
//					MCU_67S0_PIN_ON;
//					MCU_67S1_PIN_ON;
//					MCU_67S2_PIN_ON;
//					MCU_67S3_PIN_OFF;
//					
//				}break;
//				case 2:
//				{
//					MCU_67E_PIN_OFF;
//					MCU_67S0_PIN_OFF;
//					MCU_67S1_PIN_ON;
//					MCU_67S2_PIN_ON;
//					MCU_67S3_PIN_OFF;
//				}break;
//				case 3:
//				{
//					MCU_67E_PIN_OFF;
//					MCU_67S0_PIN_ON;
//					MCU_67S1_PIN_OFF;
//					MCU_67S2_PIN_ON;
//					MCU_67S3_PIN_OFF;
//				}break;
//				case 4:
//				{
//					MCU_67E_PIN_OFF;
//					MCU_67S0_PIN_OFF;
//					MCU_67S1_PIN_OFF;
//					MCU_67S2_PIN_ON;
//					MCU_67S3_PIN_OFF;
//				}break;
//				case 5:
//				{
//					MCU_67E_PIN_OFF;
//					MCU_67S0_PIN_ON;
//					MCU_67S1_PIN_ON;
//					MCU_67S2_PIN_OFF;
//					MCU_67S3_PIN_OFF;
//				}break;
//				case 6:
//				{
//					MCU_67E_PIN_OFF;
//					MCU_67S0_PIN_OFF;
//					MCU_67S1_PIN_ON;
//					MCU_67S2_PIN_OFF;
//					MCU_67S3_PIN_OFF;
//				}break;
//				case 7:
//				{
//					MCU_67E_PIN_OFF;
//					MCU_67S0_PIN_ON;
//					MCU_67S1_PIN_OFF;
//					MCU_67S2_PIN_OFF;
//					MCU_67S3_PIN_OFF;
//				}break;
//				case 8:
//				{
//					MCU_67E_PIN_OFF;
//					MCU_67S0_PIN_OFF;
//					MCU_67S1_PIN_OFF;
//					MCU_67S2_PIN_OFF;
//					MCU_67S3_PIN_OFF;
//				}break;
//				case 9:
//				{
//					MCU_67E_PIN_OFF;
//					MCU_67S0_PIN_OFF;
//					MCU_67S1_PIN_OFF;
//					MCU_67S2_PIN_OFF;
//					MCU_67S3_PIN_ON;
//				}break;
//				case 10:
//				{
//					MCU_67E_PIN_OFF;
//					MCU_67S0_PIN_ON;
//					MCU_67S1_PIN_OFF;
//					MCU_67S2_PIN_OFF;
//					MCU_67S3_PIN_ON;
//				}break;
//				case 11:
//				{
//					MCU_67E_PIN_OFF;
//					MCU_67S0_PIN_OFF;
//					MCU_67S1_PIN_ON;
//					MCU_67S2_PIN_OFF;
//					MCU_67S3_PIN_ON;
//				}break;
//				case 12:
//				{
//					MCU_67E_PIN_OFF;
//					MCU_67S0_PIN_ON;
//					MCU_67S1_PIN_ON;
//					MCU_67S2_PIN_OFF;
//					MCU_67S3_PIN_ON;
//				}break;
//				default:break;
//				
//		}
//}
//static void fmod_sbox_choosePassiveEquilibrium(int bt_index)
//{
//	switch (bt_index)
//		{
//			case 1:
//				{
//					MCU_4524EL_PIN_OFF; 
//					MCU_4524A0_PIN_OFF;
//					MCU_4524A1_PIN_OFF;
//					MCU_4524A2_PIN_OFF;
//					MCU_4524A3_PIN_OFF; 
//					
//				}break;
//				case 3:
//				{
//					MCU_4524EL_PIN_OFF; 
//					MCU_4524A0_PIN_ON;
//					MCU_4524A1_PIN_OFF;
//					MCU_4524A2_PIN_OFF;
//					MCU_4524A3_PIN_OFF; 
//				}break;
//				case 2:
//				{
//					MCU_4524EL_PIN_OFF; 
//					MCU_4524A0_PIN_OFF;
//					MCU_4524A1_PIN_ON;
//					MCU_4524A2_PIN_OFF;
//					MCU_4524A3_PIN_OFF; 
//				}break;
//				case 4:
//				{
//					MCU_4524EL_PIN_OFF; 
//					MCU_4524A0_PIN_ON;
//					MCU_4524A1_PIN_ON;
//					MCU_4524A2_PIN_OFF;
//					MCU_4524A3_PIN_OFF; 
//				}break;
//				case 5:
//				{
//					MCU_4524EL_PIN_OFF; 
//					MCU_4524A0_PIN_OFF;
//					MCU_4524A1_PIN_OFF;
//					MCU_4524A2_PIN_ON;
//					MCU_4524A3_PIN_OFF; 
//				}break;
//				case 6:
//				{
//					MCU_4524EL_PIN_OFF; 
//					MCU_4524A0_PIN_ON;
//					MCU_4524A1_PIN_OFF;
//					MCU_4524A2_PIN_ON;
//					MCU_4524A3_PIN_OFF; 
//				}break;
//				case 7:
//				{
//					MCU_4524EL_PIN_OFF; 
//					MCU_4524A0_PIN_OFF;
//					MCU_4524A1_PIN_ON;
//					MCU_4524A2_PIN_ON;
//					MCU_4524A3_PIN_OFF; 
//				}break;
//				case 8:
//				{
//					MCU_4524EL_PIN_OFF; 
//					MCU_4524A0_PIN_ON;
//					MCU_4524A1_PIN_ON;
//					MCU_4524A2_PIN_ON;
//					MCU_4524A3_PIN_OFF; 
//				}break;
//				case 10:
//				{
//					MCU_4524EL_PIN_OFF; 
//					MCU_4524A0_PIN_OFF;
//					MCU_4524A1_PIN_OFF;
//					MCU_4524A2_PIN_OFF;
//					MCU_4524A3_PIN_ON; 
//				}break;
//				case 9:
//				{
//					MCU_4524EL_PIN_OFF; 
//					MCU_4524A0_PIN_ON;
//					MCU_4524A1_PIN_OFF;
//					MCU_4524A2_PIN_OFF;
//					MCU_4524A3_PIN_ON; 
//				}break;
//				case 12:
//				{
//					MCU_4524EL_PIN_OFF; 
//					MCU_4524A0_PIN_OFF;
//					MCU_4524A1_PIN_ON;
//					MCU_4524A2_PIN_OFF;
//					MCU_4524A3_PIN_ON; 
//				}break;
//				case 11:
//				{
//					MCU_4524EL_PIN_OFF; 
//					MCU_4524A0_PIN_ON;
//					MCU_4524A1_PIN_ON;
//					MCU_4524A2_PIN_OFF;
//					MCU_4524A3_PIN_ON; 
//				}break;
//				default:break;
//				
//		}
//}

/******************************************************************************************
    end file
*******************************************************************************************/
