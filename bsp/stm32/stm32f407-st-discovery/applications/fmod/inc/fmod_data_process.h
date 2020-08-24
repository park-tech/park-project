#ifndef _FMOD_DATA_PROCESS_H_
#define _FMOD_DATA_PROCESS_H_

//..........................头文件.......................................
#include <rtthread.h>
#include <rtdevice.h>
//#include <dfs_posix.h>   //虚拟文件系统
//#include <arpa/inet.h> 
#include <board.h>
#include <gui.h>


#include "thread_adc.h"
#include "fmod_fault_detect.h"
#include "fmod_store_fault.h"
#include "fmod_store_histroy.h"
#include "fmod_store_product_info.h"
#include "fmod_work_status.h"
#include "fmod_dtube_display.h"
#include "fmod_com_mvb.h"
#include "fmod_com_rs485.h"
#include "fmod_com_sbox.h"


#include "bsp_gpio.h"
#include "basic_timer.h"
#include "basic_hwtimer.h"
#include "basic_flash.h"
#include "basic_pwm.h"
#include "basic_rtc.h"
/*********************************************************************************************************
  宏定义
*********************************************************************************************************/
//....................电池相关信息定义..............................
#define SUM_BAT_NUM_MAX    84           //可测单体电池总数量
#define TEST_BAT_NUM       st_product_info.u8_bat_num        //实际测试电池数量

#define VRLA_Battery  1          //铅酸电池   
#define NC_Battery    0          //镍镉电池 
#define LIP_Battery   0          //磷酸铁锂电池 

//...................铅酸电池特性................................ 
#define VRLA_BC_Chfull_V   2.28               //充满电压值        标称2V
#define VRLA_BC_OVER_V     2.538              //单体电池过压值
#define VRLA_BC_UNDER_V    1.75               //单体电池欠压值
#define VRLA_BC_LOW_PREWARN_V    1.6          //单体电池低压预警值
#define VRLA_BC_OVER_R       20               //单体电池内阻过高值2MR  单位mR的10倍               1=0.1mR
#define VRLA_OVER_CHI      (0.2 * Bat_Qb )    //电池充电过流值


//...................镍镉电池特性................................ 
#define NC_BC_Chfull_V     1.4               //充满电压值         标称1.2V
#define NC_BC_OVER_V       1.45              //单体电池过压值
#define NC_BC_UNDER_V      1.05              //单体电池欠压值
#define NC_BC_LOW_PREWARN_V    1.15          //单体电池低压预警值
#define NC_BC_OVER_R        50               //单体电池内阻过高值 单位mR的10倍               1=0.1mR
#define NC_OVER_CHI        (0.2 * Bat_Qb )   //电池充电过流值

//..................磷酸铁锂电池特性................................ 
#define LIP_BC_Chfull_V    3.6               //充满电压值        标称3.2V
#define LIP_BC_OVER_V      3.6               //单体电池过压值
#define LIP_BC_UNDER_V     2.3               //单体电池欠压值
#define LIP_BC_LOW_PREWARN_V    2.4          //单体电池低压预警值
#define LIP_BC_OVER_R      30                 //单体电池内阻过高值 单位mR的10倍               1=0.1mR
#define LIP_OVER_CHI       (0.5 * Bat_Qb )   //电池充电过流值


#if(1 == VRLA_Battery)
#define BC_Chfull_V        VRLA_BC_Chfull_V      //充满电压值
#define BC_OVER_V          VRLA_BC_OVER_V        //单体电池过压值
#define BC_UNDER_V         VRLA_BC_UNDER_V       //单体电池欠压值
#define BC_LOW_PREWARN_V   VRLA_BC_LOW_PREWARN_V //单体电池预警
#define BC_OVER_R          VRLA_BC_OVER_R        //单体电池内阻过高值 单位mR
#define OVER_CHI           VRLA_OVER_CHI         //电池充电过流值
#endif

#if(1 == NC_Battery)
#define BC_Chfull_V        NC_BC_Chfull_V        //充满电压值
#define BC_OVER_V          NC_BC_OVER_V          //单体电池过压值
#define BC_UNDER_V         NC_BC_UNDER_V         //单体电池欠压值
#define BC_LOW_PREWARN_V   NC_BC_LOW_PREWARN_V   //单体电池预警
#define BC_OVER_R          NC_BC_OVER_R          //单体电池内阻过高值 单位mR
#define OVER_CHI           NC_OVER_CHI           //电池充电过流值
#endif


#if(1 == LIP_Battery)
#define BC_Chfull_V        LIP_BC_Chfull_V       //充满电压值
#define BC_OVER_V          LIP_BC_OVER_V         //单体电池过压值
#define BC_UNDER_V         LIP_BC_UNDER_V        //单体电池欠压值
#define BC_LOW_PREWARN_V   LIP_BC_LOW_PREWARN_V  //单体电池预警
#define BC_OVER_R          LIP_BC_OVER_R         //单体电池内阻过高值 单位mR
#define OVER_CHI           LIP_OVER_CHI          //电池充电过流值
#endif

//................................电池通用数据特征..................................
#define Bat_Qb             220              // 标称容量220Ah
#define OVER_TEMP          55               // 电池过温值
#define UNDER_TEMP        -20               // 电池低温值
#define Charge_full_V      (BC_Chfull_V * TEST_BAT_NUM ) //电池充满电压
#define Charge_full_I      1                //电池充满电流
#define OVER_DISCHI        (0.7 * Bat_Qb )  //电池放电过流值

//............产品的所有信息，包括需要存储的和非存储的数据........................
struct Product_info         
{
	char  i8_soft_version[8];   //软件版本号

	char i8_project_name[32];   //项目名称 
	char i8_product_name[32];   //产品名称

	uint16_t u16_product_num;   //产品编号
	uint8_t  u8_car_num;	    //产品车号
	uint8_t  u8_bat_num;	    //电池检测数量
	
};
//.............................产品可设定信息................................ 
struct productset_bits
{   
	uint16_t set_time_valid:1;          //时间设置有效
	uint16_t set_product_num_valid:1;   //产品编号设置有效
	uint16_t set_car_num_valid:1;       //车号设置有效
	uint16_t set_bat_num_valid:1;       //单体检测电池数量设置有效

	uint16_t set_soc_valid :1;		      //电池SOC设置有效	
	uint16_t set_soh_valid :1;          //电池SOH设置有效	
	uint16_t set_ch_overI_valid:1;      //电池过充电电流过大值设置有效
	uint16_t set_disch_overI_valid:1;   //电池放电电流过大值设置有效
};

union productset_reg
{
	struct	 productset_bits  st_bit;
	uint8_t  u16_all;   
};

struct product_preset     //产品预设值 20B
{   
	union     productset_reg  un_preset;	//设置有效位

    uint8_t   u8_time[6];	          //6B  
	uint16_t  u16_product_num;        //产品编号

	uint8_t   u8_car_num;             //车号
	uint8_t   u8_bat_num;             //电池测试数量

    uint8_t   u8_preset_soc;
	uint8_t   u8_preset_soh;

	uint8_t   u8_ch_overI;            //充电过流值，只是调试时适用，重启后恢复原来数值
	uint8_t   u8_disch_overI;         //放电过流值，只是调试时适用，重启后恢复原来数值
	
	uint8_t   u8_resv[4];
};

//.............................电池信息................................ 
struct Bat_data  //62
{
	float	     fl_bat_volt;			 //电池电压 	
	
	float        fl_bat_soc;             //电池剩余容量 100倍 
	float        fl_bat_soh;             //电池健康状态 100倍   	
	
	float	     fl_bat_chI;	         //当前充电电流 
	float	     fl_bat_dischI;          //当前放电电流
	
	uint16_t	 u16_bat_max_volt;		 //电池最高单体电压 	1mv
	uint16_t	 u16_bat_min_volt;		 //电池最低单体电压 	1mv
	
	float		 fl_bat_max_temp;	     //电池最高温度实际值 	
	float		 fl_bat_min_temp;	     //电池最低温度实际值 
	
	uint16_t	 u16_bat_max_R;		     //电池单体最高内阻 0.01mΩ 
	uint16_t	 u16_bat_min_R;		     //电池单体最低内阻 0.01mΩ	
	
	float		 fl_bat_Qc_max;		     //电池当前可充的最大电池容量	
	float	     fl_bat_Qnow;			 //电池工作后实时容量 
	
  uint16_t	 u16_bat_avg_volt;		 //电池平均单体电压 	1mv
  uint16_t	 u16_bat_avg_R;		     //电池平均内阻 

	uint16_t	 u16_err_bat_num;		 //故障电池序号
  uint16_t	 u16_resv;		         //电池平均内阻 	          
};

struct Bat_status_bits
{  
	uint16_t bat_fault:1;        //自定义      电池故障 
	uint16_t bat_full:1;         //自定义      电池充满 
	uint16_t bat_empty:1;        //自定义      电池放光 
	uint16_t bat_first_work:1;   // 1表示非电池首次上电 电池连接 ，0表示是第一次和电池连接
};

union	 Bat_status_regs
{
	struct	 Bat_status_bits  st_bit;
	uint16_t u16_all;   
};

struct Bat_err_bits
{   
	uint16_t bat_overV:1;        //电池组过充  
    uint16_t bat_underV:1;       //电池组过放      86v
	uint16_t bat_underV_warn:1;	 //电池欠压预警
	uint16_t bat_over_chI:1;	 //电池充电过流
	
	uint16_t bat_overdischI:1; 	 //电池放电过流
	uint16_t bat_overT:1;        //电池过温    高于55度 1s
	uint16_t batcore_overV:1;	 //电池单体电池过充
	uint16_t batcore_underV:1;	 //电池单体电池过放

    uint16_t bat_overR:1;	     //电池内阻过高
	uint16_t self_check_err:1;   //自检故障
	uint16_t bat_underT:1;	     //电池低温 
	uint16_t bat_temp_fault:1;	 //温度传感器故障

	uint16_t rs485_com_err:1; 	  //通讯故障
	uint16_t recoverch_warn:1; 	  //恢复性充电预警
	uint16_t bat_short_board:1;   //电池短板提示	  
	uint16_t resv:1;              //
};

union	 Bat_err_regs
{
	struct	 Bat_err_bits  st_bit;
	uint16_t  u16_all;   
};
//.............................单体电池信息................................ 
struct Batcore_data
{
	uint16_t	 u16_batcore_volt[84];	 //单体电池电压 1 mv
	uint16_t     u16_batcore_temp[84];   //(T+55)*10 
	uint16_t     u16_batcore_R[84];      //单体电池内阻 0.01 m欧   	
};
//.........................单体电池故障标识................................
struct Batcore_err_bits          //电池过充，第一组：单体1-32
{  
    uint32_t err1:1;       
	uint32_t err2:1;	     
	uint32_t err3:1;	     
	uint32_t err4:1;	   
	uint32_t err5:1;	     
	uint32_t err6:1; 	    
	uint32_t err7:1; 	  
	uint32_t err8:1; 	   
	uint32_t err9:1; 	   
	uint32_t err10:1;	   
	uint32_t err11:1;	   
	uint32_t err12:1;			
	uint32_t err13:1;	     
	uint32_t err14:1; 		
	uint32_t err15:1; 		
	uint32_t err16:1; 		

	uint32_t err17:1;		    
	uint32_t err18:1;			
	uint32_t err19:1;			
	uint32_t err20:1;			
	uint32_t err21:1;			
	uint32_t err22:1;	   
	uint32_t err23:1;	   
	uint32_t err24:1;	   
	uint32_t err25:1;	   
	uint32_t err26:1;	    
	uint32_t err27:1;	    
	uint32_t err28:1;	    
	uint32_t err29:1;	     
	uint32_t err30:1; 		
	uint32_t err31:1; 		
	uint32_t err32:1; 		
};

union Batcore_err_reg 
{
	struct    Batcore_err_bits  st_bit;
	uint32_t  u32_all;
};

struct Batcore_fault_regs
{	
	union Batcore_err_reg     un_overV[3];//0-31位表示 第1-32节单体电池过充状态,位置1表示过压
	union Batcore_err_reg     un_underV[3];//0-31位表示 第1-32节单体电池过放状态
	union Batcore_err_reg     un_overT[3];//0-31位表示 第1-32节单体电池过温状态
    union Batcore_err_reg     un_overR[3];//0-31位表示 第1-32节单体电池内阻过高
	union Batcore_err_reg     un_Terr[3];  //0-31位表示 第1-32节单体电池温度传感器故障,    
	union Batcore_err_reg     un_short_board[3];  //0-31位表示 第1-32节单体电池短板故障,  	
};

union	Batcore_err_regs
{
	struct	     Batcore_fault_regs  st_err;
	uint32_t	 u32_batcore_err[18];  // 18*4=72 byte	
};


 //..........................接触器状态标志....................
struct Contactor_status_bits         
{
	uint8_t  KM1_work_sign:1;
	uint8_t  KM2_work_sign:1;
	uint8_t  KM1_fault_sign:1;
	uint8_t  KM2_fault_sign:1;

	uint8_t  bat_full:1;
	
};


//.............................MVB通讯信息................................ 
struct Send_mvb_message   //发送给MVB的参数内容
{   
	uint8_t	     u8_life;		         //生命信号
    uint8_t      u8_err_bat_num;         //短板电池的节数
	uint16_t     u16_bat_volt;           // *10
	uint16_t	 u16_bat_soc;			 //电池剩余容量百分比	
	uint16_t	 u16_bat_soh;			 //电池健康状态百分比
	
	uint16_t	 u16_bat_chI;			 //充电电流 *10
	uint16_t	 u16_bat_dischI;		 //放电电流 *10
	uint16_t	 u16_bat_max_volt;		 //电池最高单体电压mv	
	uint16_t	 u16_bat_min_volt;		 //电池最低单体电压mv
	
	uint16_t	 u16_bat_max_temp;		 //电池最高温度 = T*10 + 550
	uint16_t	 u16_bat_min_temp;		 //电池最低温度 = T*10 + 550
	uint16_t	 u16_bat_max_R;		     //电池最高内阻 1 = 0.01mΩ
	uint16_t	 u16_bat_min_R;		     //电池最低内阻 1 = 0.01mΩ
	
	uint16_t	 u16_bat_avg_volt;		 //电池平均单体电压 	1mv
    uint16_t	 u16_bat_avg_R;		     //电池平均内阻 1 = 0.01mΩ
	uint8_t      u8_bat_err[2];          //故障状态  Bat_err_bits
	uint8_t	     u8_softversion[2];
};

union  Se_mvb_data
{
	struct  Send_mvb_message  st_se_mvb;
    uint8_t u8_data[32];                            //send_mvb_packdata
};

struct Re_mvb_bits
{
	uint16_t time_set:1;
	uint16_t time_valid:1;
	uint16_t resv2:1;
	uint16_t resv3:1;
	
	uint16_t resv4:1;
	uint16_t resv5:1;
	uint16_t resv6:1;
	uint16_t resv7:1;
	
	uint16_t resv8:1;
	uint16_t resv9:1;
	uint16_t resv10:1;
	uint16_t resv11:1;
	
	uint16_t mvb_control:1;
	uint16_t resv13:1;
	uint16_t resv14:1;
	uint16_t resv15:1;

};
union Re_mvb_reg
{
	struct    Re_mvb_bits  st_bit;		
	uint16_t  u16_all;
};

struct Re_mvb_message
{   
	uint16_t  u16_life;		                 //2B
	union     Re_mvb_reg  un_16b_remvb;      //2B
	uint8_t   u8_time[6];	                 //6B
	uint8_t   u8_data[22];                   //14B	
};

union Re_mvb_data
{
	struct  Re_mvb_message st_re_mvb;
    uint8_t u8_data[32];
};


//..............................系统时间.............................
struct Sys_time_data    //62字节
{   
    uint8_t     u8_year;
	uint8_t	    u8_mon;                   
	
	uint8_t     u8_day;           
	uint8_t		u8_hour;

	uint8_t		u8_min;        	
	uint8_t		u8_sec;
};

//..................历史数据存储和故障数据存储使用相同的数据结构.................................
//..................历史数据存储 1s一次  一天（24h）的大概5-6M,一个月总存储量150M容量.............
struct history_store_data
{   
	char         i8_project_name[20];                //20byte

	char		 i8_product_name[18];                //18byte
	uint16_t	 u16_product_num;                    //2byte   产品编号
	
	uint32_t     u32_sd_num;                         // 序号         SD存的信息序号	
	struct       Bat_data     st_bat_data;           // 电池组的数据
    char         batcore_volt_string[352];           // struct  Batcore_data      st_batcore_data电芯的数据
	char		 batcore_temp_string[352];
	char		 batcore_R_string[352];
	union	     Bat_err_regs      un_bat_err;      // 2字节      电池组的故障
	union        Batcore_err_regs  un_batcore_err;  // 72字节        电芯的故障
	char		 i8_version[8];		                // 软件版本
	char		 i8_timestring[20];	                // 上传时间
};


struct Timer_flag
{
	uint32_t	flag_10ms:1;                 
	uint32_t	flag_20ms:1;               
	uint32_t	flag_50ms:1;               
	uint32_t	flag_100ms:1;     
    uint32_t	flag_200ms:1;                 
	uint32_t	flag_500ms:1;               
    uint32_t	flag_1s:1;                 
	uint32_t	flag_2s:1; 

	uint32_t	flag_5s:1;               
	uint32_t	flag_10s:1;     
    uint32_t	flag_20s:1;                 
	uint32_t	flag_30s:1;               
	uint32_t	flag_1min:1;               
	uint32_t	flag_2min:1; 
    uint32_t	flag_5min:1; 
    uint32_t	flag_10min:1; 
};

extern  struct  Product_info      st_product_info;
extern  struct  product_preset    st_product_preset;

extern  struct  Bat_data          st_bat_data;  
extern  union	Bat_err_regs      un_bat_err;
extern  union	Bat_status_regs   un_bat_status ;

extern  struct  Batcore_data      st_batcore_data;
extern  union	Batcore_err_regs  un_batcore_err;
extern  struct  Contactor_status_bits   st_KM_bit;

extern  union   Can_se_sbox_data      un_se_sbox_data;
extern  union   Can_re_sbox_data      un_re_sbox_data[SUM_BAT_NUM_MAX];

extern  struct  Timer_flag          timer_flag;   
extern  struct  Sys_time_data       st_savefile_time[6];

extern  union   Product_info_store  un_prodinfo_rdata;
extern  union   Status_store_data    un_history_wdata;        //实时工作状态存储
extern  union   Status_store_data    un_fault_wdata[30];     //故障信息存储

extern  time_t  real_sec;                          //从70年1月1号开始的秒数	

extern void fmod_device_init(void);
extern void fmod_variable_init(void);
extern void fmod_parameter_update(void);
extern void fmod_relay_control(void);


void endian_swaps(void* data);
void endian_swapl(void* data);

#endif 


