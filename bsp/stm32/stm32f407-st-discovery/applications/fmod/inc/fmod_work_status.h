#ifndef  _FMOD_WORK_STATUS_H_
#define  _FMOD_WORK_STATUS_H_

//..........................头文件.......................................
#include <rtthread.h>

#include "fmod_data_process.h"
#include "bsp_gpio.h"


//..........................函数申明.......................................
extern void fmod_soc_soh_intiset(void);
extern void fmod_static_volt_adj_soc(void);
extern void fmod_updata_soc(void);
extern void fmod_relay_control(void);

void fmod_updata_soh(void);
void fmod_open_volt_adj_soc(void);

#endif
