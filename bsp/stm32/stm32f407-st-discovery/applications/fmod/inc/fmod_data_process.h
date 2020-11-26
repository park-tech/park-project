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
#include "fmod_com_rs485.h"
#include "fmod_PassiveEquilibrium.h"

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
#define VRLA_BC_Chfull_V   13.2              //充满电压值     
#define VRLA_BC_OVER_V     14               //单体电池过压值 
#define VRLA_BC_UNDER_V    10.5             //单体电池欠压值 
#define VRLA_BC_LOW_PREWARN_V    10.8         //单体电池低压预警值
//#define VRLA_BC_OVER_R       20               //单体电池内阻过高值2MR  单位mR的10倍  //海泰蓄电池项目用不到             1=0.1mR
#define VRLA_OVER_CHI      5.4               //电池充电过流值  充电器的电流最大2A，此处填写蓄电池的实际充电过流值

////...................铅酸电池特性................................ 
//#define VRLA_BC_Chfull_V   2.28               //充满电压值        标称2V
//#define VRLA_BC_OVER_V     2.538              //单体电池过压值
//#define VRLA_BC_UNDER_V    1.75               //单体电池欠压值
//#define VRLA_BC_LOW_PREWARN_V    1.6          //单体电池低压预警值
//#define VRLA_BC_OVER_R       20               //单体电池内阻过高值2MR  单位mR的10倍               1=0.1mR
//#define VRLA_OVER_CHI      (0.2 * Bat_Qb )    //电池充电过流值

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
#define Bat_Qb             8.0             // 标称容量8Ah
#define OVER_TEMP          55               // 电池过温值
#define UNDER_TEMP        -20               // 电池低温值
#define Charge_full_V      (BC_Chfull_V * TEST_BAT_NUM ) //电池充满电压
#define Charge_full_I      1                //电池充满电流       //这个参数含义？
#define OVER_DISCHI        (0.7 * Bat_Qb )  //电池放电过流值 96A

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
	uint16_t set_time_valid:1;          						//时间设置有效
	uint16_t set_product_num_valid:1;   						//产品编号设置有效
	uint16_t set_car_num_valid:1;       						//车号设置有效
	uint16_t set_bat_num_valid:1;       						//单体检测电池数量设置有效

	uint16_t set_soc_valid :1;		      						//电池SOC设置有效	
	uint16_t set_soh_valid :1;          						//电池SOH设置有效	
	uint16_t set_ch_overI_valid:1;      						//电池过充电电流过大值设置有效
	uint16_t set_disch_overI_valid:1;   						//电池放电电流过大值设置有效
	
	uint16_t set_batcore_Volt1_adjust_valid:1;		        	//  单体电压校准值1有效
	uint16_t set_batcore_Volt2_adjust_valid:1;		        	//  单体电压校准值2有效
	uint16_t set_Charge_I_adjust_valid:1;		        		//  充电电流校准值有效
	uint16_t set_disCharge_I1_adjust_valid:1;		        	//  放电电流1校准值1有效
	
	uint16_t set_disCharge_I2_adjust_valid:1;	        		//  放电电流2校准值2有效
	uint16_t set_Outcharger_V1_adjust_valid:1;		        	//  外部充电机电压1校准值有效
	uint16_t set_Outcharger_V2_adjust_valid:1;		       	 	//   外部充电机电压2校准值有效

};

union productset_reg
{
	struct	 productset_bits  st_bit;
	uint16_t  u16_all;   
};

struct product_preset     //产品预设值 28B
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
	
	uint16_t   u16_batcore_Volt1_adjust_Value;		        //  单体电压校准值1
	uint16_t   u16_batcore_Volt2_adjust_Value;		        //  单体电压校准值2
	
	uint8_t   u8_Charge_I_adjust_Value;		        	//  充电电流校准值
	uint8_t   u8_disCharge_I1_adjust_Value;		        //  放电电流1校准值
	
	uint8_t   u8_disCharge_I2_adjust_Value;	        //  放电电流2校准值
	uint8_t   u8_Outcharger_V1_adjust_Value;		        //  外部充电机电压1校准值
	
	uint8_t   u8_Outcharger_V2_adjust_Value;		        //   外部充电机电压2校准值
	uint8_t   u8_charger_underV;       //充电机电压欠压值
	
	uint8_t	  u8_resv[2];
};

//.............................电池信息................................ 
struct Bat_data  //72个字节
{
	float	     fl_bat_volt;			 //电池电压 	
	float        fl_bat_soc;             //电池剩余容量 100倍
	
	float        fl_bat_soh;             //电池健康状态 100倍   
	float	     fl_bat_chI;	         //当前充电电流 
	
	float	     fl_bat_dischI;          //当前放电电流
	float		 fl_bat_max_temp;	     //电池最高温度实际值 
	
	float		 fl_bat_min_temp;	     //电池最低温度实际值 
	float		 fl_bat_Qc_max;		     //电池当前可充的最大电池容量
	
	float	     fl_bat_Qnow;			 //电池工作后实时容量 
	float		 fl_charger_volt1;			 //外部充电机电压1 
	
	float		 fl_charger_volt2;			 //外部充电机电压2 
	float	 	 fl_bat_dischI1;			//负载电流1
	
	float	 	 fl_bat_dischI2;			//负载电流2
	float	 	 f1_resv0;					//预留
	
	uint16_t	 u16_batcore_max_volt;		 //电池最高单体电压 	1mv
	uint16_t	 u16_batcore_min_volt;		 //电池最低单体电压 	1mv
	
    uint16_t	 u16_batcore_avg_volt;		 //电池平均单体电压 	1mv
	uint16_t	 u16_err_bat_num;		 //故障电池序号
	
    uint16_t	 u16_resv[4];		    //预留	          
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

//系统输入输出位
struct Sys_Inout_bits
{   
	uint8_t In_Sleep:1;        	 			//睡眠信号
    uint8_t In_speed0:1;       				//零速信号
	uint8_t In_DC_Charger_fault:1;	 		//内部DC电源故障
	uint8_t In_resv3:1;	    //外部充电机信号
	
	uint8_t Out_Sys_fault:1;               //预留
	uint8_t resv5:1;               //预留
	uint8_t resv6:1;               //预留
	uint8_t resv7:1;               //预留
};
union	 Sys_Inout_regs
{
	struct	 Sys_Inout_bits  st_bit;
	uint8_t  u8_all;   
};
 //..........................系统状态标志....................
struct Sys_status_bits         
{
//	uint8_t  KM1_work_sign:1;
//	uint8_t  KM2_work_sign:1;
//	uint8_t  KM7_work_sign:1;
//	uint8_t  resv:1;
	
	uint8_t  charge_KM_work_sign:1;  //充电接触器KM1状态
	uint8_t  disch_KM_sign:1;      //放电接触器KM2状态
	uint8_t  power_KM_work_sign:1; //板子电源继电器K7的状态
	uint8_t  resv3:1;
	
	uint8_t  resv4:1;
	uint8_t  resv5:1;
	uint8_t  resv6:1;
	uint8_t  PassiveEquilibrium_sign:1;//被动均衡状态
	
};
union	Sys_status_regs
{
	struct	     Sys_status_bits  st_bit ;
	uint8_t  u8_all;   
};


//系统故障锁死位
struct Bat_lock_bits
{   
	uint16_t bat_over_chI_lock:1;        	 			//充电过流锁死 
    uint16_t bat_overdischI_lock:1;       				//放电过流锁死
	uint16_t bat_overT_lock:1;	 					//电池过温锁死
	uint16_t resv3:1;	            //预留
	
	uint16_t resv4:1;               //预留
	uint16_t resv5:1;               //预留
	uint16_t resv6:1;               //预留
	uint16_t resv7:1;               //预留

	uint16_t resv8:1;               //预留
	uint16_t resv9:1;               //预留
	uint16_t resv10:1;               //预留
	uint16_t resv11:1;               //预留

	uint16_t resv12:1;               //预留
	uint16_t resv13:1;               //预留
	uint16_t resv14:1;               //预留
	uint16_t resv15:1;               //预留
};
union	 Bat_lock_regs
{
	struct	 Bat_lock_bits  st_bit;
	uint16_t  u16_all;   
};

//系统故障位
struct Bat_err_bits1
{   
	uint16_t bat_overV_err:1;        //电池组过充  
    uint16_t bat_underV_err:1;       //电池组过放      
	uint16_t bat_over_chI_err:1;	 //电池充电过流
	uint16_t bat_overdischI_err:1; 	 //电池放电过流

	uint16_t bat_underSOC_warn:1;	 //电池S0C低压预警
	uint16_t bat_overT_err:1;        //电池过温    高于55度 1s
	uint16_t bat_underT_err:1;	     //电池低温
	uint16_t bat_sensor_I_err:1;	     //电流传感器故障

	uint16_t batcore_overV_err:1;	 //电池单体电池过充
	uint16_t batcore_underV_err:1;	 //电池单体电池过放
	uint16_t batcore_overT_err:1;	 //单体（模组）电池过温故障
	uint16_t batcore_V_err:1;	 //单体（模组）电压采样线断开故障

	uint16_t bat_sensorT_err:1;	 	//温度传感器故障
	uint16_t batcore_over_difV_err:1;	 	//单体（模组）电池压差故障
    uint16_t batcore_TEqui_err:1;	     //温差超限故障
	uint16_t resv:1;              	//预留
};
union	 Bat_err_regs1
{
	struct	 Bat_err_bits1  st_bit;
	uint16_t  u16_all;   
};
struct Bat_err_bits2
{ 
	uint16_t self_check_err:1;   //自检故障
	uint16_t SYSin_com_err:1;     //内部通讯故障
	uint16_t CANorMVB_com_err:1; 	  //CAN/MVB通讯故障
	uint16_t ETH_com_err:1; 	  //ETH通讯故障
	
	uint16_t  Charge_KM_err:1;   //KM1故障
	uint16_t  Disch_KM_err:1;   //KM2故障
	uint16_t Power_KM_err:1;   //KM7故障
	uint16_t  resv7:1;   			//预留
	
	uint16_t charger_underV_err:1;     //外部充电机欠压
	uint16_t DCpower_err:1;      //内部充电器故障
	uint16_t resv10:1;              //预留
	uint16_t resv11:1;              //预留

	uint16_t resv12:1;              //预留
	uint16_t resv13:1;              //预留
	uint16_t resv14:1;              //预留
	uint16_t SYS_err:1;          //系统故障
};
union	 Bat_err_regs2
{
	struct	 Bat_err_bits2  st_bit;
	uint16_t  u16_all;   
};

//.............................单体电池信息................................ 
struct Batcore_data
{
	uint16_t	 u16_batcore_volt[12];	 //单体电池电压                  ※※1mv
	uint16_t     u16_batcore_temp[12];   //(T+55)*10   	
};
//.........................单体电池故障标识................................
struct Batcore_err_bits          //电池过充，第一组：单体1-16
{  
    uint16_t err1:1;       
	uint16_t err2:1;	     
	uint16_t err3:1;	     
	uint16_t err4:1;	   
	uint16_t err5:1;	     
	uint16_t err6:1; 	    
	uint16_t err7:1; 	  
	uint16_t err8:1; 	   
	uint16_t err9:1; 	   
	uint16_t err10:1;	   
	uint16_t err11:1;	   
	uint16_t err12:1;			
	uint16_t err13:1;	     
	uint16_t err14:1; 		
	uint16_t err15:1; 		
	uint16_t err16:1; 		
 		
};

union Batcore_err_reg 
{
	struct    Batcore_err_bits  st_bit;
	uint16_t  u16_all;
};

struct Batcore_fault_regs
{	
	union Batcore_err_reg     un_overV[1];//0-16位表示 第1-16节单体电池过充状态,位置1表示过压
	union Batcore_err_reg     un_underV[1];//0-16位表示 第1-16节单体电池过放状态
	union Batcore_err_reg     un_overT[1];//0-16位表示 第1-16节单体电池过温状态
  //union Batcore_err_reg     un_overR[1];//0-16位表示 第1-16节单体电池内阻过高
	union Batcore_err_reg     un_Terr[1];  //0-16位表示 第1-16节单体电池温度传感器故障,    
	union Batcore_err_reg     un_short_board[1];  //0-16位表示 第1-16节单体电池短板故障,  	
};

union	Batcore_err_regs
{
	struct	     Batcore_fault_regs  st_err;
	uint16_t	 u16_batcore_err[5];  // 5*2=10 byte	
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
    char         batcore_volt_string[52];           // struct  Batcore_data      st_batcore_data电芯的数据
	char         batcore_temp_string[52];           // struct  Batcore_data      st_batcore_data电芯的数据
	
	union	Bat_err_regs1      	un_bat_err1;      			// 2字节      电池组的故障1
	union	Bat_err_regs2      	un_bat_err2;      			// 2字节      电池组的故障2
	union	Bat_lock_regs    	un_bat_lock;				//锁死故障
	union	Sys_Inout_regs   	un_sys_Inout;			//系统输入输出
	union	Sys_status_regs   un_sys_status;				//继电器接触器状态
	
	char		 i8_version[8];		                // M4应用程序版本
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


extern  union	Bat_err_regs1      un_bat_err1;
extern  union	Bat_err_regs2      un_bat_err2;

extern  union	Bat_lock_regs    un_bat_lock;

extern  union	Bat_status_regs   un_bat_status ;

extern  struct  Batcore_data      st_batcore_data;
extern  union	Batcore_err_regs  un_batcore_err;

extern  union	Sys_status_regs   un_sys_status;

extern  union	 Sys_Inout_regs   un_sys_Inout;
	

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
extern void fmod_adjust_value_judge(void);

void endian_swaps(void* data);
void endian_swapl(void* data);

#endif 


