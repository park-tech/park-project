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
static uint16_t bit_to_index(uint16_t bit);

/******************************************************************************************
函数描述：被动均衡
输入参数：无
返回值：      无
*******************************************************************************************/
void fmod_PassiveEquilibrium(void)
{   
	uint16_t Passive_Equilibrium_bit= fmod_Passive_Equilibrium();
	uint16_t bat_index=bit_to_index(Passive_Equilibrium_bit);
	if(bat_index>=1)
	{
		fmod_sbox_choosePassiveEquilibrium(bat_index);
	}
}



//........................将1-9位转换成对应的数字................................
static uint16_t bit_to_index(uint16_t bit)
{
	static uint16_t index=0;
	uint16_t bit_array[9]={0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,0x0100};
	for(uint16_t i=0;i<sizeof(bit_array)/sizeof(bit_array[0]);i++)
	{
		if(bit&&bit_array[i]==bit_array[i])
		{
			index=i+1;
		
		}
	
	}
	return index;
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

