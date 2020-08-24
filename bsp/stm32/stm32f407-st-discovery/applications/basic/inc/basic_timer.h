#ifndef _TIMER_INTERRUPT_H_
#define _TIMER_INTERRUPT_H_

//..........................头文件.......................................
#include <rtthread.h>
#include "thread_int_timer.h"

//..........................函数申明.......................................

extern int basic_timer1_init( uint32_t msec);
extern int basic_timer2_init( uint32_t msec);

#endif
