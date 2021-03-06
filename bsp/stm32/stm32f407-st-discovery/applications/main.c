/******************************************************************************
** 头文件
*******************************************************************************/
#include "fmod_data_process.h"
/*******************************************************************************
** 变量定义
*******************************************************************************/
struct  Timer_flag     timer_flag;
union	Bat_err_regs1   un_ago_bat_err1;
union	Bat_err_regs2   un_ago_bat_err2;
extern  uint8_t        product_preset_flag;

/*******************************************************************************
** 函数名称：
** 函数描述：main函数
** 输入参数：
** 返回值  ：无
*******************************************************************************/
int main(void)
{  	
    
	//..............初始化............................................   
    fmod_device_init( );
    fmod_variable_init( );

    //..............读取产品信息，且判断是否首次上电...................
    fmod_prodinfo_firsttime_read();

    //..............第一次连接时设置SOC和SOH .Qc_max, soc .............
    if(un_bat_status.st_bit.bat_first_work == 1)  //是否首次上电	
    {	   
        fmod_soc_soh_intiset( ); 	
    }
	//..............校正值异常复位 .............
	fmod_adjust_value_judge();
	
    //..............接触器自检.......................................
		
	fmod_self_test();//接触器反馈，原始状态的信息  放在故障检测里面
		
    //.......................等待300ms ADC先第一次采样完成...............
	rt_thread_mdelay(300);

    while (RT_TRUE)
    {  
         
        //........................数据更新（产品可设置的信息更新和adc的数据）.................
        fmod_parameter_update( );

		    //........................故障查询.................................
        fmod_fault_detect( ); //
	
		//.......................继电器接触器控制.............................
		fmod_relay_control();
		
		//.......................均衡控制.............................
		fmod_PassiveEquilibrium();
		
		//.......................数码管显示.............................
		fmod_dtube_display( );  //	

		//.......................CAN和UART发送数据.........................	
		if(timer_flag.flag_500ms )
		{
			timer_flag.flag_500ms = 0;
			
			//fmod_mvb_com( );
		}

		if(timer_flag.flag_1s )
		{
			timer_flag.flag_1s = 0;
			fmod_485_com( );
		}
        
				//.........................判断是否有新故障存储..............................
        if(( (un_bat_err1.u16_all > 0) &&                           //有故障
            (un_ago_bat_err1.u16_all != un_bat_err1.u16_all)) ||      //和上次故障不一样
		((un_bat_err2.u16_all > 0) && 
		(un_ago_bat_err2.u16_all != un_bat_err2.u16_all)))
        {    
            fmod_fault_store( );
        }
        un_ago_bat_err1.u16_all = un_bat_err1.u16_all;
		un_ago_bat_err2.u16_all = un_bat_err2.u16_all;
        
         //........................1分钟存储一次SOC,SOH..............................
		if( product_preset_flag || timer_flag.flag_1min)
		{   
			product_preset_flag = 0;
			timer_flag.flag_1min = 0;
	
			fmod_prodinfo_store( );	
		}
		//.........................2分钟存储一次历史数据..............................
		if(timer_flag.flag_2min )
		{
			timer_flag.flag_2min = 0;
			fmod_histroy_store( );
		}	
		rt_thread_mdelay(100);
		}

		return RT_EOK;
}

