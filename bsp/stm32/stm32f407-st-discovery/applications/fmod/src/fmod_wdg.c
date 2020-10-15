#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"
#include "rtdbg.h"
#include "fmod_wdg.h"

static rt_device_t wdg_dev =RT_NULL;         /* 看门狗设备句柄 */
// 喂狗
void feed_wdg(void)
{
    if(wdg_dev != RT_NULL) rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL); // 跳转之前进行一次喂狗
}
// 设定看门狗超时时间
void set_wdt_timeout(uint8_t sec) 
{
    rt_uint32_t timeout = sec;        /* 溢出时间，单位：秒 */
    if(wdg_dev != RT_NULL)  rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, &timeout);
}
static void idle_hook(void)
{
    /* 在空闲线程的回调函数里喂狗 */
    rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
}

int wdt_config(void)
{
    rt_err_t ret = RT_EOK;
    rt_uint32_t timeout = 8;        /* 溢出时间，单位：秒 */
    if(wdg_dev == RT_NULL) wdg_dev = rt_device_find(WDT_DEVICE_NAME);
    if (!wdg_dev)
    {
        LOG_E("find %s failed!\n", WDT_DEVICE_NAME);
        return RT_ERROR;
    }
    /* 初始化设备 */
    ret = rt_device_init(wdg_dev);
    if (ret != RT_EOK)
    {
        LOG_E("initialize %s failed!\n", WDT_DEVICE_NAME);
        return RT_ERROR;
    }
    /* 设置看门狗溢出时间 */
    ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, &timeout);
    if (ret != RT_EOK)
    {
        LOG_E("set %s timeout failed!\n", WDT_DEVICE_NAME);
        return RT_ERROR;
    }
    /* 启动看门狗 */
    ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_START, RT_NULL);
    if (ret != RT_EOK)
    {
        LOG_E("start %s failed!\n", WDT_DEVICE_NAME);
        return -RT_ERROR;
    }
    /* 设置空闲线程回调函数 */
    rt_thread_idle_sethook(idle_hook);

    return ret;
}
INIT_PREV_EXPORT(wdt_config);

