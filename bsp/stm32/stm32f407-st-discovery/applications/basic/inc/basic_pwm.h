
#ifndef _BASIC_PWM_H_
#define _BASIC_PWM_H_

//..........................头文件.......................................
#include <rtthread.h>
#include <rtdevice.h>

#if defined(BSP_USING_PWM8_BKIN)
#include "stm32f407xx.h"
#include "stm32f4xx.h"
#endif

#define PWM_DEV_NAME        "pwm8"

#define PWM_DEV_CHANNEL1     1
#define PWM_DEV_CHANNEL2     2
#define PWM_DEV_CHANNEL3     3
#define PWM_DEV_CHANNEL4     4

extern rt_err_t basic_pwm_init(uint8_t pwm_channel);
extern void basic_pwm_set(uint8_t pwm_channel, uint32_t period, uint32_t  pulse);
#endif

