
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
extern  union	Bat_err_regs1      un_bat_err1;
extern  union	Bat_err_regs2      un_bat_err2;
extern  union	Sys_status_regs   un_sys_status;
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

	showdata = 0x0008;                                                       //优先级低
	
	if(un_sys_status.st_bit.charge_KM_work_sign== 1)        			showdata = 0x00014; 	//优先级中
	if(un_sys_status.st_bit.disch_KM_sign== 1)        			showdata = 0x00015; 	//优先级中
	if(un_sys_status.st_bit.PassiveEquilibrium_sign== 1)	showdata = 0x00012;		//优先级中
	
	if(un_bat_err2.st_bit.Charge_KM_err== 1)   			showdata = 0x0000;      //优先级高
	if(un_bat_err2.st_bit.Disch_KM_err== 1)   			showdata = 0x0001;
	if(un_bat_err2.st_bit.Power_KM_err == 1)   			showdata = 0x0002;
	if(un_bat_err1.st_bit.batcore_overV_err == 1)    	showdata = 0x0003;  	
	if(un_bat_err1.st_bit.batcore_underV_err == 1)   	showdata = 0x0004; 	
	if(un_bat_err1.st_bit.bat_over_chI_err == 1)     	showdata = 0x0005;  
	if(un_bat_err2.st_bit.self_check_err== 1)    	showdata = 0x0006;
	
	
	
	
   //.........................设定具体的显示内容....................................
	switch(showdata)     //根据state_point显示内容
	{  
		case 0x0000 :   
			g_Gui_show_pic("0");    break;    //KM1故障
		case 0x0001 :   
			g_Gui_show_pic("1");    break;    //KM2故障
		case 0x0002 :   
			g_Gui_show_pic("2");    break;    //KM3故障
		case 0x0003 :   
			g_Gui_show_pic("3");    break;   //KM7故障
		case 0x0004 :   
			g_Gui_show_pic("4");    break;    //单体过压故障
		case 0x0005 :   
			g_Gui_show_pic("5");    break;    //单体欠压故障
		case 0x0006 :   
			g_Gui_show_pic("6");    break;    //充电过流故障
		case 0x0007 :   
			g_Gui_show_pic("7");    break;    //放电过流故障
		case 0x0008 :   
			g_Gui_show_pic("8");    break;    //正常
		case 0x0009 :   
			g_Gui_show_pic("9");    break;    //过温故障
		case 0x0010 :   
			g_Gui_show_pic("A");    break;    //低温故障
		case 0x0011 :   
			g_Gui_show_pic("b");    break;     //短板故障
		case 0x0012 :   
			g_Gui_show_pic("C");    break;    //被动均衡
		case 0x0013 :   
			g_Gui_show_pic("D");    break;    //自检故障
		case 0x0014 :   
			g_Gui_show_pic("E");    break;    //充电状态
		case 0x0015 :   
			g_Gui_show_pic("F");    break;    //放电状态
		default: break;
    }		
}
