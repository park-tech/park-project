#include <string.h>
#include "basic_rtc.h"
#include "rtdbg.h"
#if defined (RT_USING_RTC)
/**
  **********************************************************************************
  * @funcation  :void get_rtc_time(struct tm *rtc)
  * @author     :
  * @date       :20200219
  * @brief      :�ⲿ�ӿں���,��ȡRTC,
  * @attention  :
  **********************************************************************************
  */
void get_rtc_time(struct tm *rtc)
{
    uint32_t now;
    struct tm *p_tm;
    static rt_device_t rtc_dev = RT_NULL;   // rtc�豸���
    if(RT_NULL == rtc_dev)
    {
        rtc_dev = rt_device_find("rtc");
    }
    if(rtc_dev)
    {
        rt_device_control(rtc_dev,RT_DEVICE_CTRL_RTC_GET_TIME,&now); //��ȡ��ǰ����
        if(rtc != RT_NULL)  
        {
            p_tm = localtime(&now); //�ѵ�ǰʱ���ת���ɶ�Ӧ������
            memcpy(rtc, p_tm, sizeof(struct tm));
        }
    }
}
/**
  **********************************************************************************
  * @funcation  :void get_rtc_second(uint32_t *sec)
  * @author     :
  * @date       :20200221
  * @brief      :��ȡRTC������
  * @attention  :
  **********************************************************************************
  */
void get_rtc_second(uint32_t *sec)
{
    uint32_t now;
    static rt_device_t rtc_dev = RT_NULL;   // rtc�豸���
    if(RT_NULL == rtc_dev)
    {
        rtc_dev = rt_device_find("rtc");
    }
    if(rtc_dev)
    {
        rt_device_control(rtc_dev,RT_DEVICE_CTRL_RTC_GET_TIME,&now); //��ȡ��ǰ����
        if(sec != RT_NULL) *sec = now;
    }
}
/**
  **********************************************************************************
  * @funcation  :void set_rtc_time(struct tm rtc)
  * @author     :ZhouLH
  * @date       :20200219
  * @brief      :�ⲿ�ӿں���,�趨RTC
  * @attention  :rtc.tm_year  = year-1900;
                 rtc.tm_mon   = mon-1;
  **********************************************************************************
  */
void set_rtc_time(struct tm rtc)
{   
    static rt_device_t rtc_dev = RT_NULL;   // rtc�豸���
    uint32_t  now = 0;
    if(RT_NULL == rtc_dev)
    {
        rtc_dev = rt_device_find("rtc");
    }
    if(rtc_dev)
    {
        now = mktime(&rtc);
        rt_device_control(rtc_dev,RT_DEVICE_CTRL_RTC_SET_TIME,&now);
    }
}


#endif
/************** end of file ************************/
