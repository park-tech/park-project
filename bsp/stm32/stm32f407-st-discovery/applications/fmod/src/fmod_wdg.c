#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"
#include "rtdbg.h"
#include "fmod_wdg.h"

static rt_device_t wdg_dev =RT_NULL;         /* ���Ź��豸��� */
// ι��
void feed_wdg(void)
{
    if(wdg_dev != RT_NULL) rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL); // ��ת֮ǰ����һ��ι��
}
// �趨���Ź���ʱʱ��
void set_wdt_timeout(uint8_t sec) 
{
    rt_uint32_t timeout = sec;        /* ���ʱ�䣬��λ���� */
    if(wdg_dev != RT_NULL)  rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, &timeout);
}
static void idle_hook(void)
{
    /* �ڿ����̵߳Ļص�������ι�� */
    rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
}

int wdt_config(void)
{
    rt_err_t ret = RT_EOK;
    rt_uint32_t timeout = 8;        /* ���ʱ�䣬��λ���� */
    if(wdg_dev == RT_NULL) wdg_dev = rt_device_find(WDT_DEVICE_NAME);
    if (!wdg_dev)
    {
        LOG_E("find %s failed!\n", WDT_DEVICE_NAME);
        return RT_ERROR;
    }
    /* ��ʼ���豸 */
    ret = rt_device_init(wdg_dev);
    if (ret != RT_EOK)
    {
        LOG_E("initialize %s failed!\n", WDT_DEVICE_NAME);
        return RT_ERROR;
    }
    /* ���ÿ��Ź����ʱ�� */
    ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, &timeout);
    if (ret != RT_EOK)
    {
        LOG_E("set %s timeout failed!\n", WDT_DEVICE_NAME);
        return RT_ERROR;
    }
    /* �������Ź� */
    ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_START, RT_NULL);
    if (ret != RT_EOK)
    {
        LOG_E("start %s failed!\n", WDT_DEVICE_NAME);
        return -RT_ERROR;
    }
    /* ���ÿ����̻߳ص����� */
    rt_thread_idle_sethook(idle_hook);

    return ret;
}
INIT_PREV_EXPORT(wdt_config);

