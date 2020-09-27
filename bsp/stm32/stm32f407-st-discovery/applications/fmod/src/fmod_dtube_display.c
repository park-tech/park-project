
/********************************************************************************************
头文件
********************************************************************************************/
#include "fmod_dtube_display.h"
/********************************************************************************************
宏定义
********************************************************************************************/
/********************************************************************************************
变量定义
********************************************************************************************/
extern  union	Bat_err_regs      un_bat_err;
/********************************************************************************************
函数申明
********************************************************************************************/


/**********************************************************************************
** 函数名称：dtube_display
** 函数描述：数码管显示
** 输入参数：无
** 返回值  ：系统返回值
************************************************************************************/
void fmod_dtube_display(void)
{	
	uint16_t showdata = 0;

	showdata = 0x0001;
	
	if(un_bat_err.st_bit.self_check_err == 1)   showdata = 0x0002;
	if(un_bat_err.st_bit.rs485_com_err == 1 )         showdata = 0x0004;
	if(un_bat_err.st_bit.batcore_underV == 1)   showdata = 0x0008; 	
	if(un_bat_err.st_bit.batcore_overV == 1)   showdata = 0x0010;  	
	
	if(un_bat_err.st_bit.bat_underV_warn == 1)  showdata = 0x0020;
	if(un_bat_err.st_bit.bat_underV == 1)       showdata = 0x0040;
	
	if(un_bat_err.st_bit.bat_over_chI == 1)     showdata = 0x0080;    //逆变器的输入过欠压,输入保险排除在外
	if(un_bat_err.st_bit.bat_overdischI == 1)   showdata = 0x0100;   
	
	if(un_bat_err.st_bit.bat_overV == 1)        showdata = 0x0200; 
    if(un_bat_err.st_bit.bat_overT == 1)        showdata = 0x0400; 
	
	
	
   //.........................设定具体的显示内容....................................
	switch(showdata)     //根据state_point显示内容
	{  
		case 0x0001 :   
			g_Gui_show_pic("8");    break;    //正常
		case 0x0002 :   
			g_Gui_show_pic("F");    break;    //自检故障
		case 0x0004 :   
			g_Gui_show_pic("c");    break;    //通讯故障
		case 0x0008 :   
			g_Gui_show_pic("5");    break;    //单体电压低
		case 0x0010 :   
			g_Gui_show_pic("7");    break;    //单体电压高
		case 0x0020 :   
			g_Gui_show_pic("3");    break;    //电池低压报警
		case 0x0040 :   
			g_Gui_show_pic("2");    break;    //电池欠压
		case 0x0080 :   
			g_Gui_show_pic("A");    break;    //充电过流
		case 0x0100 :   
			g_Gui_show_pic("b");    break;    //放电过流
		case 0x0200 :   
			g_Gui_show_pic("1");    break;    //输入过压
		case 0x0400 :   
			g_Gui_show_pic("9");    break;    //过温
		
		default: break;
    }		
}
