#ifndef _BASIC_HWTIMER_H_
#define _BASIC_HWTIMER_H_


#define HWTIMER_DEV_NAME   "timer3"  /* 定时器名称 */

//..........................头文件.......................................
#include <rtthread.h>
#include <rtdevice.h>

#include "thread_int_hwtimer.h"
        
//..........................函数申明.......................................
extern  int basic_hwtimer_init(int hw_sec, int hw_usec);

#endif

