#ifndef _FMOD_STORE_FAULT_H_
#define _FMOD_STORE_FAULT_H_
//..........................头文件.......................................
#include <rtthread.h>
#include "fmod_data_process.h"
#include "basic_flash.h"
#include "cJSON.h"

//..............................系统时间.............................
struct Fault_store_time    //62字节
{   
    uint8_t     u8_year;
	uint8_t	    u8_mon;                   
	uint8_t     u8_day;           
};


//................单独一个文本文档 保存历史数据文件名称的时间.............................
struct Fault_file_tm    //62字节
{   
    uint8_t     u8_headdata[2];
	struct      Fault_store_time	 st_tm[6];    //3*6	
	uint8_t     u8_enddata[2];
};
union Fault_file_time    //52字节
{   
    struct      Fault_file_tm	 st_data;
	uint8_t     u8_data[22];
};



extern void fmod_fault_store(void);
//..........................函数申明.......................................


#endif

