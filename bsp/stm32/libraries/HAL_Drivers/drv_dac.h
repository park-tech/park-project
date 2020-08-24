#ifndef __DRV_DAC_H__
#define __DRV_DAC_H__

#include <rtthread.h>
#include <board.h>

extern void  drv_dac_init(void);
extern void drv_dac_set_value(uint32_t channel, uint32_t alignment, uint32_t data);
extern void drv_dac_start(uint32_t channel);

#endif
