#ifndef __BASIC_RTC_H
#define __BASIC_RTC_H
#include <rtthread.h>
#include <rtdevice.h>

void get_rtc_time(struct tm *rtc);
void get_rtc_second(uint32_t *sec);
void set_rtc_time(struct tm rtc);


#endif
