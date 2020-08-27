/******************************************************************************
** 头文件
*******************************************************************************/
#include "fmod_data_process.h"
/*******************************************************************************
** 变量定义
*******************************************************************************/
struct  Timer_flag     timer_flag;
union	Bat_err_regs   un_ago_bat_err;
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
    //..............接触器自检.......................................
		#if(TEST_MODE)
		fmod_self_test();
		#endif
    //.......................等待300ms ADC先第一次采样完成...............
		rt_thread_mdelay(300);

    while (RT_TRUE)
    {  
         
        //........................数据更新（产品可设置的信息更新和adc的数据）.................
        fmod_parameter_update( );

		    //........................故障查询.................................
        fmod_fault_detect( ); //需增加继电器、接触器是否触点粘连

        //.......................数码管显示.............................
				fmod_dtube_display( );  //待确定定义
			
				//.......................继电器控制.............................
				fmod_relay_control();
				//........................充放电接触器控制.................................
				#if(TEST_MODE)
				fmod_contactor_control();
				#endif	
		
				//.......................CAN和UART发送数据.........................	
				if(timer_flag.flag_500ms )
				{
					timer_flag.flag_500ms = 0;
					fmod_sbox_com( );
					//fmod_mvb_com( );
				}
		
				if(timer_flag.flag_1s )
				{
					timer_flag.flag_1s = 0;
					fmod_485_com( );
				}
        
				//.........................判断是否有新故障存储..............................
        if( (un_bat_err.u16_all > 0) &&                           //有故障
            (un_ago_bat_err.u16_all != un_bat_err.u16_all))       //和上次故障不一样
        {    
            fmod_fault_store( );
        }
        un_ago_bat_err.u16_all = un_bat_err.u16_all;
        
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

