/********************************************************************************************
头文件
********************************************************************************************/

#include"basic_hwtimer.h"
/********************************************************************************************
宏定义
********************************************************************************************/

/********************************************************************************************
变量定义
********************************************************************************************/

/********************************************************************************************
函数申明
********************************************************************************************/
extern rt_err_t hwtimeout_cb(rt_device_t dev, rt_size_t size);

#if defined(BSP_USING_TIM)
/**************************************************************************************************
** 函数名称：
** 函数描述：定时器初始化
** 输入参数：hw_sec为定时的秒数，hw_usec为定时的为微秒数， 最少时间为100us
** 返回值  ：无
***************************************************************************************************/
int basic_hwtimer_init(int hw_sec, int hw_usec)
{
    rt_err_t  ret = RT_EOK;
    rt_hwtimerval_t  timeout_s;      /* 定时器超时值 */
    rt_device_t  hw_dev = RT_NULL;   /* 定时器设备句柄 */
    rt_hwtimer_mode_t  mode;         /* 定时器模式 */
    rt_uint32_t  freq = 10000;       /* 计数频率  周期100us*/
    
    /* 查找定时器设备 */
    hw_dev = rt_device_find(HWTIMER_DEV_NAME);
    if (hw_dev == RT_NULL)
    {
        rt_kprintf("hwtimer init  failed! can't find %s device!\n", HWTIMER_DEV_NAME);
        return RT_ERROR;
    }
    /* 以读写方式打开设备 */
    ret = rt_device_open(hw_dev, RT_DEVICE_OFLAG_RDWR);
    if (ret != RT_EOK)
    {
        rt_kprintf("open %s device failed!\n", HWTIMER_DEV_NAME);
        return ret;
    }
    /* 设置超时回调函数 */
    rt_device_set_rx_indicate(hw_dev, hwtimeout_cb);
 
    /* 设置计数频率*/
    ret = rt_device_control(hw_dev, HWTIMER_CTRL_FREQ_SET, &freq);
    if (ret != RT_EOK)
    {
        rt_kprintf("set frequency failed! ret is :%d\n", ret);
        return ret;
    }
      
    /* 设置模式为周期性定时器 */
    mode = HWTIMER_MODE_PERIOD;
    ret = rt_device_control(hw_dev, HWTIMER_CTRL_MODE_SET, &mode);
    if (ret != RT_EOK)
    {
        rt_kprintf("set mode failed! ret is :%d\n", ret);
        return ret;
    }
    /* 设置定时器超时值为5s并启动定时器 */
    timeout_s.sec = hw_sec;        /* 秒 */
    timeout_s.usec = hw_usec;     /* 微秒 */
    if (rt_device_write(hw_dev, 0, &timeout_s, sizeof(timeout_s)) != sizeof(timeout_s))
    {
        rt_kprintf("set timeout value failed\n");
        return RT_ERROR;
    }
    
    /* 延时500ms */
    rt_thread_mdelay(500);
    
    /* 读取定时器当前值 */
    rt_device_read(hw_dev, 0, &timeout_s, sizeof(timeout_s));
    rt_kprintf("Read: Sec = %d, Usec = %d\n", timeout_s.sec, timeout_s.usec);
    return ret;
}

#endif

