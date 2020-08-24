#include "basic_pwm.h"

#if defined(BSP_USING_PWM8)
#if defined(BSP_USING_PWM8_BKIN)
#define TIM_OSSR_DISABLE                    0x00000000U             
#define TIM_OSSR_ENABLE                     TIM_BDTR_OSSR           
#define TIM_OSSI_DISABLE                    0x00000000U             
#define TIM_OSSI_ENABLE                     TIM_BDTR_OSSI  
#define TIM_LOCKLEVEL_OFF                   0x00000000U         
#define TIM_LOCKLEVEL_1                     TIM_BDTR_LOCK_0      
#define TIM_LOCKLEVEL_2                     TIM_BDTR_LOCK_1     
#define TIM_LOCKLEVEL_3                     TIM_BDTR_LOCK   
#define TIM_BREAK_DISABLE                   0x00000000U             
#define TIM_BREAK_ENABLE                    TIM_BDTR_BKE           
#define TIM_BREAK_POLARITY_LOW              0x00000000U          
#define TIM_BREAK_POLARITY_HIGH             TIM_BDTR_BKP   
#define TIM_AUTOMATICOUTPUT_DISABLE         0x00000000U            
#define TIM_AUTOMATICOUTPUT_ENABLE          TIM_BDTR_AOE   
#define TIM_WriteReg(__INSTANCE__, __REG__, __VALUE__)    WRITE_REG((__INSTANCE__)->__REG__, (__VALUE__))
#endif

/********************************************************************************************
** 定义变量
********************************************************************************************/
#if defined(BSP_USING_PWM8_BKIN)
typedef struct
{
  uint32_t OSSRState;           
  uint32_t OSSIState;            
  uint32_t LockLevel;           
  uint8_t  DeadTime;             
  uint16_t BreakState;          
  uint32_t BreakPolarity;        
  uint32_t AutomaticOutput;    
} TIM_BDTR_InitTypeDef;
#endif
struct rt_device_pwm *pwm_dev;      /* PWM设备句柄 */	
/********************************************************************************************
** 函数申明
********************************************************************************************/
/********************************************************************************************
** 内容
********************************************************************************************/
/********************************************************************************************
** 函数名称：	
** 函数作用：刹车引脚初始化
** 参数：   
** 返回值:

********************************************************************************************/
#if defined(BSP_USING_PWM8_BKIN)
static void  basic_TIM_BDTR_Init(TIM_TypeDef *TIMx, TIM_BDTR_InitTypeDef *TIM_BDTRInitStruct)
{
  uint32_t tmpbdtr = 0;

  /* Set the BDTR bits */
  MODIFY_REG(tmpbdtr, TIM_BDTR_DTG, TIM_BDTRInitStruct->DeadTime);
  MODIFY_REG(tmpbdtr, TIM_BDTR_LOCK, TIM_BDTRInitStruct->LockLevel);
  MODIFY_REG(tmpbdtr, TIM_BDTR_OSSI, TIM_BDTRInitStruct->OSSIState);
  MODIFY_REG(tmpbdtr, TIM_BDTR_OSSR, TIM_BDTRInitStruct->OSSRState);
  MODIFY_REG(tmpbdtr, TIM_BDTR_BKE, TIM_BDTRInitStruct->BreakState);
  MODIFY_REG(tmpbdtr, TIM_BDTR_BKP, TIM_BDTRInitStruct->BreakPolarity);
  MODIFY_REG(tmpbdtr, TIM_BDTR_AOE, TIM_BDTRInitStruct->AutomaticOutput);
  MODIFY_REG(tmpbdtr, TIM_BDTR_MOE, TIM_BDTRInitStruct->AutomaticOutput);
  /* Set TIMx_BDTR */
  TIM_WriteReg(TIMx, BDTR, tmpbdtr);
}
static void basic_BDTR_init(void)
{
	TIM_BDTR_InitTypeDef  TIM_BDTRInitStruct;

	TIM_BDTRInitStruct.DeadTime        = (uint8_t)0x00; 
    TIM_BDTRInitStruct.LockLevel       = TIM_LOCKLEVEL_OFF;
    TIM_BDTRInitStruct.OSSIState       = TIM_OSSI_DISABLE;
    TIM_BDTRInitStruct.OSSRState       = TIM_OSSR_DISABLE;
    TIM_BDTRInitStruct.BreakState      = TIM_BREAK_ENABLE;
    TIM_BDTRInitStruct.BreakPolarity   = TIM_BREAK_POLARITY_LOW;
    TIM_BDTRInitStruct.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	
	//TIM_WriteReg(TIM8, BDTR, TIM_BDTRInitStruct);
   basic_TIM_BDTR_Init(TIM8, &TIM_BDTRInitStruct);  
}
#endif
/********************************************************************************************
** 函数名称：	
** 函数作用：PWM初始化
** 参数：   channel 通道号, period ：工作周期，单位为纳秒ns； pulse脉宽宽度，单位为纳秒ns
** 返回值:

********************************************************************************************/
rt_err_t basic_pwm_init(uint8_t pwm_channel)
{ 
	rt_uint32_t period, pulse;

    period = 500000;    /* 周期为0.5ms，单位为纳秒ns */
    pulse = 0;          /* PWM脉冲宽度值，单位为纳秒ns */
    
#if defined(BSP_USING_PWM8_BKIN)
    basic_BDTR_init( );
#endif   
    //.......................................查找设备......................................
	pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
    if (pwm_dev == RT_NULL)
    {
        rt_kprintf("pwm sample run failed! can't find %s device!\n", PWM_DEV_NAME);
        return RT_ERROR;
    }
  
	
    /* 设置PWM周期和脉冲宽度默认值 */
    rt_pwm_set(pwm_dev, pwm_channel, period, pulse);

    /* 使能设备 */
    rt_pwm_enable(pwm_dev, pwm_channel);
    
	
    return RT_EOK;
}
/********************************************************************************************
** 函数名称：	
** 函数作用：PWM输出
** 参数：   channel 通道号, period ：工作周期，单位为纳秒ns； pulse脉宽宽度，单位为纳秒ns
** 返回值:
********************************************************************************************/
void basic_pwm_set(uint8_t pwm_channel, uint32_t period, uint32_t  pulse)
{
    /* 设置PWM周期和脉冲宽度默认值 */
    rt_pwm_set(pwm_dev, pwm_channel, period, pulse);
}
#endif


