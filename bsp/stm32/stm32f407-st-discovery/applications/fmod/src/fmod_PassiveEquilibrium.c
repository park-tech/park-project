/********************************************************************************************
头文件
********************************************************************************************/
#include "fmod_PassiveEquilibrium.h"

/********************************************************************************************
宏定义
********************************************************************************************/
/********************************************************************************************
变量定义
********************************************************************************************/


/********************************************************************************************
函数申明
********************************************************************************************/

static void fmod_sbox_choosePassiveEquilibrium (int bt_index);

/******************************************************************************************
函数描述：被动均衡
输入参数：无
返回值：      无
*******************************************************************************************/
void fmod_PassiveEquilibrium(void)
{   
	
	un_bat_err.st_bat_err_bit.PassiveEquilibrium=0;
	
	for(int i=0;i<TEST_BAT_NUM;i++)
	{
		
		if(st_batcore_data.u16_batcore_volt[i]>st_bat_data.u16_bat_avg_volt+10)//当单体电压高于平均电压1V时启动被动均衡机制
		{
			fmod_sbox_choosePassiveEquilibrium(i+1);
			un_bat_err.st_bat_err_bit.PassiveEquilibrium=1;
		}
	
	}
}
//........................选择单体电池进行放电................................
static void fmod_sbox_choosePassiveEquilibrium(int bt_index)
{
	switch (bt_index)
	{
	case 1:
	{
		MCU_4524EL_PIN_OFF;
		MCU_4524A0_PIN_OFF;
		MCU_4524A1_PIN_OFF;
		MCU_4524A2_PIN_OFF;
		MCU_4524A3_PIN_OFF;
	}
	break;
	case 3:
	{
		MCU_4524EL_PIN_OFF;
		MCU_4524A0_PIN_ON;
		MCU_4524A1_PIN_OFF;
		MCU_4524A2_PIN_OFF;
		MCU_4524A3_PIN_OFF;
	}
	break;
	case 2:
	{
		MCU_4524EL_PIN_OFF;
		MCU_4524A0_PIN_OFF;
		MCU_4524A1_PIN_ON;
		MCU_4524A2_PIN_OFF;
		MCU_4524A3_PIN_OFF;
	}
	break;
	case 4:
	{
		MCU_4524EL_PIN_OFF;
		MCU_4524A0_PIN_ON;
		MCU_4524A1_PIN_ON;
		MCU_4524A2_PIN_OFF;
		MCU_4524A3_PIN_OFF;
	}
	break;
	case 5:
	{
		MCU_4524EL_PIN_OFF;
		MCU_4524A0_PIN_OFF;
		MCU_4524A1_PIN_OFF;
		MCU_4524A2_PIN_ON;
		MCU_4524A3_PIN_OFF;
	}
	break;
	case 6:
	{
		MCU_4524EL_PIN_OFF;
		MCU_4524A0_PIN_ON;
		MCU_4524A1_PIN_OFF;
		MCU_4524A2_PIN_ON;
		MCU_4524A3_PIN_OFF;
	}
	break;
	case 7:
	{
		MCU_4524EL_PIN_OFF;
		MCU_4524A0_PIN_OFF;
		MCU_4524A1_PIN_ON;
		MCU_4524A2_PIN_ON;
		MCU_4524A3_PIN_OFF;
	}
	break;
	case 8:
	{
		MCU_4524EL_PIN_OFF;
		MCU_4524A0_PIN_ON;
		MCU_4524A1_PIN_ON;
		MCU_4524A2_PIN_ON;
		MCU_4524A3_PIN_OFF;
	}
	break;
	case 10:
	{
		MCU_4524EL_PIN_OFF;
		MCU_4524A0_PIN_OFF;
		MCU_4524A1_PIN_OFF;
		MCU_4524A2_PIN_OFF;
		MCU_4524A3_PIN_ON;
	}
	break;
	case 9:
	{
		MCU_4524EL_PIN_OFF;
		MCU_4524A0_PIN_ON;
		MCU_4524A1_PIN_OFF;
		MCU_4524A2_PIN_OFF;
		MCU_4524A3_PIN_ON;
	}
	break;
	case 12:
	{
		MCU_4524EL_PIN_OFF;
		MCU_4524A0_PIN_OFF;
		MCU_4524A1_PIN_ON;
		MCU_4524A2_PIN_OFF;
		MCU_4524A3_PIN_ON;
	}
	break;
	case 11:
	{
		MCU_4524EL_PIN_OFF;
		MCU_4524A0_PIN_ON;
		MCU_4524A1_PIN_ON;
		MCU_4524A2_PIN_OFF;
		MCU_4524A3_PIN_ON;
	}
	break;
	default:
		break;
	}
}

