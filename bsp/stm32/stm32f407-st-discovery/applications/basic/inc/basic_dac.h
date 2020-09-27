
#ifndef _BASIC_DAC_H_
#define _BASIC_DAC_H_

//..........................头文件.......................................
#include <rtthread.h>
#include "drv_dac.h"



extern void basic_dac_init(void);
extern void basic_dac_set_value(uint32_t channel, uint32_t alignment, uint32_t data);
extern void basic_dac_start(uint32_t channel);

#endif
