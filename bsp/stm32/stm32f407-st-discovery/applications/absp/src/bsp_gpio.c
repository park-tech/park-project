#include "bsp_gpio.h"


static void bsp_gpio_out_initstate_set(void) ;
/**
 * @ : ***************************************************************************************
 * @name: 
 * @describe: GPIO输入输出配置
 * @param : 
 * @return: 
 * @  : ***************************************************************************************
 */
void bsp_gpio_init(void) 
{ 
    //.............输入配置............................
    rt_pin_mode(SLEEP_STATUS_PIN, PIN_MODE_INPUT);
    rt_pin_mode(SPEED0_STATUS_PIN, PIN_MODE_INPUT);
    rt_pin_mode(DC_Charger_STATUS_PIN, PIN_MODE_INPUT);
    //rt_pin_mode(CHARGE_STATUS_PIN, PIN_MODE_INPUT);
	
    rt_pin_mode(K1_FEED_PIN , PIN_MODE_INPUT);
    rt_pin_mode(K2_FEED_PIN, PIN_MODE_INPUT);
    rt_pin_mode(K7_FEED_PIN, PIN_MODE_INPUT);
    //rt_pin_mode(K3_FEED_PIN, PIN_MODE_INPUT);
    
    rt_pin_mode(CARID_A_FEED_PIN, PIN_MODE_INPUT);
    rt_pin_mode(CARID_B_FEED_PIN, PIN_MODE_INPUT);
    rt_pin_mode(CARID_C_FEED_PIN, PIN_MODE_INPUT);
    rt_pin_mode(CARID_D_FEED_PIN, PIN_MODE_INPUT);
	
   //.............输出配置............................
    rt_pin_mode(MCU_4524A0_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(MCU_4524A1_PIN, PIN_MODE_OUTPUT);
		rt_pin_mode(MCU_4524A2_PIN, PIN_MODE_OUTPUT);
		rt_pin_mode(MCU_4524A3_PIN, PIN_MODE_OUTPUT);
		rt_pin_mode(MCU_4524EL_PIN, PIN_MODE_OUTPUT);
	
		rt_pin_mode(MCU_38A1_PIN, PIN_MODE_OUTPUT);
		rt_pin_mode(MCU_38B1_PIN, PIN_MODE_OUTPUT);
		rt_pin_mode(MCU_38C1_PIN, PIN_MODE_OUTPUT);
		rt_pin_mode(MCU_38E1_PIN, PIN_MODE_OUTPUT);
		rt_pin_mode(MCU_38A2_PIN, PIN_MODE_OUTPUT);
		rt_pin_mode(MCU_38B2_PIN, PIN_MODE_OUTPUT);
		rt_pin_mode(MCU_38C2_PIN, PIN_MODE_OUTPUT);
		rt_pin_mode(MCU_38E2_PIN, PIN_MODE_OUTPUT);
		
		rt_pin_mode(MCU_67S0_PIN, PIN_MODE_OUTPUT);
		rt_pin_mode(MCU_67S1_PIN, PIN_MODE_OUTPUT);
		rt_pin_mode(MCU_67S2_PIN, PIN_MODE_OUTPUT);
		rt_pin_mode(MCU_67S3_PIN, PIN_MODE_OUTPUT);
		rt_pin_mode(MCU_67E_PIN, PIN_MODE_OUTPUT);
		
		rt_pin_mode(K1_START_PIN, PIN_MODE_OUTPUT);
		rt_pin_mode(K2_START_PIN, PIN_MODE_OUTPUT);
		rt_pin_mode(K7_START_PIN, PIN_MODE_OUTPUT);
		rt_pin_mode(Sys_fault_PIN, PIN_MODE_OUTPUT);
    
    
    bsp_gpio_out_initstate_set( );
}
//INIT_COMPONENT_EXPORT(bsp_gpio_init);	
/**
 * @ : ***************************************************************************************
 * @name: 
 * @describe: GPIO输出脚初始状态设定
 * @param : 
 * @return: 
 * @  : ***************************************************************************************
 */
static void bsp_gpio_out_initstate_set(void) 
{	
	MCU_4524A0_PIN_OFF;
	MCU_4524A1_PIN_OFF;
	MCU_4524A2_PIN_OFF;
	MCU_4524A3_PIN_OFF ;      
	MCU_4524EL_PIN_ON ;  
	
	
	//初始化单体电压选择D0和D1
	MCU_38E1_PIN_OFF;
	MCU_38A1_PIN_OFF;
	MCU_38B1_PIN_ON;
	MCU_38C1_PIN_ON;

	MCU_38E2_PIN_OFF;
	MCU_38A2_PIN_OFF;
	MCU_38B2_PIN_ON;
	MCU_38C2_PIN_ON;
	
	//初始化单体电池温度选择单体1的温度
	
	MCU_67E_PIN_OFF;
	MCU_67S0_PIN_ON;
	MCU_67S1_PIN_ON;
	MCU_67S2_PIN_ON;
	MCU_67S3_PIN_OFF;
	
	
	
	K1_START_PIN_OFF;
	K2_START_PIN_OFF;
	K7_START_PIN_ON;
	Sys_fault_PIN_OFF;
}


/*******************************************************************************
** 函数名称：
** 函数描述：以太网引脚的初始化
** 输入参数：
** 返回值  ：无
*******************************************************************************/
void phy_reset(void)
{
    rt_pin_mode(phy_rst_pin, PIN_MODE_OUTPUT);
    rt_pin_write(phy_rst_pin, PIN_HIGH);
    rt_thread_mdelay(500);
    rt_pin_write(phy_rst_pin, PIN_LOW);
    rt_thread_mdelay(500);
    rt_kprintf("%s done\r\n", __func__);
    rt_pin_write(phy_rst_pin, PIN_HIGH);
}