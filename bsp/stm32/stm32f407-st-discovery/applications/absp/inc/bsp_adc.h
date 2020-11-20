#ifndef _BSP_ADC_H_
#define _BSP_ADC_H_

//..........................头文件.......................................
#include <rtthread.h>

#include "basic_adc.h"

 extern uint16_t bsp_adchannel_Iin(void);
 extern uint16_t bsp_adchannel_Iout1(void);
 extern uint16_t bsp_adchannel_Iout2(void);
 extern uint16_t bsp_adchannel_Vin2(void);
 extern uint16_t bsp_adchannel_Vin1(void);
 extern uint16_t bsp_adchannel_cell_temp(void);
 extern uint16_t bsp_adchannel_cell_vol_b0(void);
 extern uint16_t bsp_adchannel_cell_vol_b1(void);

#endif
