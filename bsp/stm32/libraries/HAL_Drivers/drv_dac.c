
/******************************************************************************************
** 头文件
*******************************************************************************************/
#include "drv_dac.h"
#pragma diag_suppress 550
/********************************************************************************************
** 定义变量
********************************************************************************************/
DAC_HandleTypeDef hdac;
	
/********************************************************************************************
** 函数申明
********************************************************************************************/
/********************************************************************************************
** 内容
********************************************************************************************/

/********************************************************************************************
** 函数名称：	
** 函数作用：DAC初始化
** 参数：   
** 返回值:
********************************************************************************************/
void drv_dac_init(void)
{

	/* USER CODE BEGIN DAC_Init 0 */
	/* USER CODE END DAC_Init 0 */

	DAC_ChannelConfTypeDef sConfig = {0};

	/* USER CODE BEGIN DAC_Init 1 */
	/* USER CODE END DAC_Init 1 */
	/** DAC Initialization  */
	hdac.Instance = DAC;
	if(HAL_DAC_Init(&hdac) != HAL_OK)
	{
		rt_kprintf("dac init fail!\n");
        return ;
	}
	/** DAC channel OUT1 config 
	 */
	sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
#ifdef BSP_USING_DAC1 
	if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
	{
		rt_kprintf("dac1 init fail!\n");
	}
#endif
	/** DAC channel OUT2 config 
	 */
#ifdef BSP_USING_DAC2	
	if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2) != HAL_OK)
	{
		rt_kprintf("dac2 init fail!\n");
	}
#endif
	/* USER CODE BEGIN DAC_Init 2 */
	/* USER CODE END DAC_Init 2 */
}

/********************************************************************************************
** 函数名称：	
** 函数作用：DAC输出电压设置
** 参数：   channel 通道号#define DAC_CHANNEL_1                      0x00000000U
                         #define DAC_CHANNEL_2                      0x00000010U
			Alignment对齐方式和精度选择：	#define DAC_ALIGN_12B_R                    0x00000000U
                                          #define DAC_ALIGN_12B_L                    0x00000004U
                                          #define DAC_ALIGN_8B_R                     0x00000008U		 
            Data输出的数字量的值：V = VREF*Data/4096
** 返回值:
********************************************************************************************/
void drv_dac_set_value(uint32_t channel, uint32_t alignment, uint32_t data)
{
    HAL_DAC_SetValue(&hdac, channel,  alignment, data);
}

/********************************************************************************************
** 函数名称：	
** 函数作用：启动DAC
** 参数：    channel  通道号
** 返回值:
********************************************************************************************/
 void drv_dac_start(uint32_t channel)
 {
 	HAL_DAC_Start(&hdac, channel);
 }

 