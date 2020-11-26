#include "bsp_bkpreg.h"
/* 1����ӱ�ģ�飻
   2��options for target--->linker---->edit
   3: �޸ĳ����ַ�ռ䣺LR_IROM1 0x08040000 0x000c0000  {    ; load region size_region
                        ER_IROM1 0x08040000 0x000c0000
   4���޸�system_stm32fxx.c:#define VECT_TAB_OFFSET  0x40000 ,�����ж�����
   5: �޸�drv_common.c:RT_WEAK void rt_hw_board_init()������__set_PRIMASK(0);�޸�Ϊ__set_PRIMASK(1);
*/
static RTC_HandleTypeDef RTC_Handler;

uint8_t whether_downloaded(void)
{
    RTC_Handler.Instance = RTC;
    if(HAL_RTCEx_BKUPRead(&RTC_Handler, RTC_BKP_DR0) == 0x5050a0a0)
    {
        return DOWNLOAD_APP;
    }
    else
    {
        return NO_DOWNLOAD_APP;
    }
}

void download_app_mark_set(void)
{
    RTC_Handler.Instance = RTC;
    HAL_RTCEx_BKUPWrite(&RTC_Handler, RTC_BKP_DR0, 0x5050a0a0);
}

void download_app_mark_clear(void)
{
    RTC_Handler.Instance = RTC;
    HAL_RTCEx_BKUPWrite(&RTC_Handler, RTC_BKP_DR0, 0x0000);
}

// boot������������,��Ӧ�ó�����Ӧ�ó���ִ��2sec������˼Ĵ���
void boot_start_times(void)
{
    uint8_t start_times;
    RTC_Handler.Instance = RTC;
    start_times = HAL_RTCEx_BKUPRead(&RTC_Handler, RTC_BKP_DR2);
    if(start_times > 5)
    {
        download_app_mark_set();
        start_times = 0;
    }
    start_times++;
    HAL_RTCEx_BKUPWrite(&RTC_Handler, RTC_BKP_DR2, start_times);
}
void start_times_clear(void)
{
    RTC_Handler.Instance = RTC;
    HAL_RTCEx_BKUPWrite(&RTC_Handler, RTC_BKP_DR2, 0x00);
}
// ============ end of file =======================

