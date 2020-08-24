#ifndef _FMOD_STORE_PRODUCT_INFO_H_
#define _FMOD_STORE_PRODUCT_INFO_H_
//..........................头文件.......................................
#include <rtthread.h>
#include "fmod_data_process.h"
#include "basic_flash.h"

//.............产品初始信息存储 应从该该寄存器读出初始状态
struct St_product_info_store  
{
    uint8_t    u8_headdata[2] ;             //  
    uint16_t   u16_product_num; 			//

	uint8_t    u8_bat_num;                  //   
	uint8_t    u8_car_num;					// 
	uint16_t   u16_bat_soc;                 //  *10

	uint16_t   u16_bat_soh;                 //  *10
	uint16_t   u16_bat_Qc_max;		        //  电池当前可充的最大电池容量	*10

	uint32_t   u32_bat_Qnow;		        //  电池当前容量 *10000
	    
	uint32_t   u32_time;                    //   

	uint8_t    u8_resv[2];                  //    
	uint8_t    u8_enddata[2];               //  
};
union  Product_info_store
{
	struct  St_product_info_store   st_data;
	uint8_t u8_data[24];
};

extern void  fmod_prodinfo_firsttime_read(void);
extern void fmod_prodinfo_store(void);
//..........................函数申明.......................................


#endif

