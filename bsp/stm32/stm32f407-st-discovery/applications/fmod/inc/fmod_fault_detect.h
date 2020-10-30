#ifndef _FMOD_FAULT_DETECT_H_
#define _FMOD_FAULT_DETECT_H_

//..........................头文件.......................................
#include <rtthread.h>

#include "bsp_gpio.h"
#include "fmod_data_process.h"



extern  union   Re_mvb_data  un_remvb_data;
//..........................函数申明.......................................
void fmod_fault_detect(void);
void fmod_self_test(void);


#endif
