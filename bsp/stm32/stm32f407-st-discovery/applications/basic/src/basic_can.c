#include "basic_can.h"

#ifdef RT_USING_CAN
#ifdef BSP_USING_CAN1
    struct rt_semaphore rx_can1_sem;     /* 用于接收消息的信号量 */
    /* 接收数据回调函数 */
    static rt_err_t basic_can1_rx_call(rt_device_t dev, rt_size_t size)
    {
        /* CAN 接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
        rt_sem_release(&rx_can1_sem);
        return RT_EOK;
    }
#endif  // #ifdef BSP_USING_CAN1
#ifdef BSP_USING_CAN2
    struct rt_semaphore rx_can2_sem;     /* 用于接收消息的信号量 */
    /* 接收数据回调函数 */
    static rt_err_t basic_can2_rx_call(rt_device_t dev, rt_size_t size)
    {
        /* CAN 接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
        rt_sem_release(&rx_can2_sem);

        return RT_EOK;
    }
#endif

#ifdef RT_CAN_USING_HDR                  // 使用硬件过滤器
    // can过滤器设置,过滤器的配置必须放在can配置之后进行
    // hdr为硬件过滤号表，一般是每一个不同的MASK放置在一个hdr里，hdr在STM32中有0-27，共28个；当不使用har时其值可以设定成-1
    void basic_CAN_Filter_Init(rt_device_t dev,uint32_t id,uint8_t ide, uint32_t mask, uint8_t hdr)
    {
        static rt_device_t can1_dev = RT_NULL;   // can设备句柄
        static rt_device_t can2_dev = RT_NULL;   // can设备句柄
        struct rt_can_filter_item item;
        if(RT_CAN_STDID == ide)
        {
            item.id = id<<18;
            item.mask = mask<<18;
        }
        else if(RT_CAN_EXTID == ide)
        {
            item.id = id;
            item.mask = mask;
        }
        else
        {
            LOG_D("CAN IDE ERROR.\n");
            return;
        }
        item.ide = ide;
        item.hdr = hdr;
        item.rtr = RT_CAN_DTR;
        item.mode = 1;
        item.args = NULL;
        item.ind  = NULL;
        struct rt_can_filter_config filter_cfg = {1, 1, &item}; /* 一共有 2 个过滤表 */
        if(dev == NULL) 
        {
            LOG_D("no can filter be init.\n");
            return;
        }
        can1_dev = rt_device_find("can1");
        can2_dev = rt_device_find("can2");
        if(dev == can1_dev)
        {
            #ifndef BSP_USING_CAN1
            LOG_D("enable BSP_USING_CAN1.\n");
            return;
            #endif  // ifdef BSP_USING_CAN1
            rt_device_control(dev,RT_CAN_CMD_SET_FILTER,&filter_cfg);  // 设置过滤器
            LOG_D("initialize can1\n");
        }
        else if(dev == can2_dev)
        {
            #ifndef BSP_USING_CAN2
            LOG_D("enable BSP_USING_CAN2.\n");
            return;
            #endif  // ifdef BSP_USING_CAN2
			#ifdef BSP_USING_CAN2
            rt_device_control(dev,RT_CAN_CMD_SET_FILTER,&filter_cfg);  // 设置过滤器
            LOG_D("initialize can1\n");
			#endif
        }
        else
        {
            LOG_D("no can filter be init.\n");
            return;
        }
    }
#endif  // #ifdef RT_CAN_USING_HDR
// can初始化
void basic_CAN_Init(rt_device_t dev ,enum CANBAUD baudrate)
{
    static rt_device_t can1_dev = RT_NULL;   // can设备句柄
    static rt_device_t can2_dev = RT_NULL;   // can设备句柄
    uint32_t argval;
    can1_dev = rt_device_find("can1");
    can2_dev = rt_device_find("can2");
    if(dev == NULL) 
    {
        LOG_D("no can dev be used to send msg.\n");
        return;
    }
    if(dev == can1_dev)
    {
        #ifndef BSP_USING_CAN1
        LOG_D("enable BSP_USING_CAN1.\n");
        return;
        #endif  // ifdef BSP_USING_CAN1
        rt_device_set_rx_indicate(dev,basic_can1_rx_call);    // 接收回调函数
        LOG_D("initialize can1\n");
    }
    else if(dev == can2_dev)
    {
        #ifndef BSP_USING_CAN2
        LOG_D("enable BSP_USING_CAN2.\n");
        return;
        #endif  // ifdef BSP_USING_CAN2
		#ifdef BSP_USING_CAN2
        rt_device_set_rx_indicate(dev,basic_can2_rx_call);    // 接收回调函数
        LOG_D("initialize can1\n");
		#endif
    }
    else
    {
        LOG_D("no can dev be initialized.\n");
        return;
    }
    
    
    rt_device_control(dev,RT_CAN_CMD_SET_MODE,(void *)RT_CAN_MODE_NORMAL); // 设定CAN工作模式:正常
    rt_device_open(dev, RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX);    /* 以中断接收及发送方式打开 CAN 设备 */
    rt_device_control(dev,RT_CAN_CMD_SET_BAUD,(void *)baudrate);        // 设置波特率
    rt_device_control(dev,RT_CAN_CMD_SET_PRIV,(void *)argval);// fa
    return;
}


void basic_send_can(uint8_t dev,uint32_t id,uint32_t ide,uint8_t len,uint8_t *buf)
{
    static rt_device_t can1_dev = RT_NULL;   // can设备句柄
    static rt_device_t can2_dev = RT_NULL;   // can设备句柄
    struct rt_can_msg txmsg = {0};
    uint8_t i;
    if(can1_dev == NULL )
    {
        can1_dev = rt_device_find("can1");
    }
    if(can2_dev == NULL)
    {
        can2_dev = rt_device_find("can2");
    }

    if(dev==1)    // CAN 1
    {
        txmsg.id  = id;             /* ID */
        txmsg.ide = ide;            /* 帧类型 */
        txmsg.rtr = RT_CAN_DTR;     /* 数据帧 */
        txmsg.len = len;            /* 数据长度 */
        for(i=0; i<len&&i<8; i++)
        {
            txmsg.data[i] = buf[i];
        }
        if(0 == rt_device_write(can1_dev, 0, &txmsg, sizeof(txmsg))) 
        {
            LOG_D("can dev write data failed!\n");
        }
        else LOG_D("can1 be used to send msg.\n");
    }
    else if(dev==2)    // CAN 2
    {
        
        txmsg.id  = id;             /* ID */
        txmsg.ide = ide;            /* 帧类型 */
        txmsg.rtr = RT_CAN_DTR;     /* 数据帧 */
        txmsg.len = len;            /* 数据长度 */
        for(i=0;i<len&&i<8;i++)
        {
            txmsg.data[i] = buf[i];
        }
        if(0==rt_device_write(can2_dev, 0, &txmsg, sizeof(txmsg)))
        {
            LOG_D("can dev write data failed!\n");
        }
        else LOG_D("can2 be used to send msg.\n");
    }
    else
    {
        LOG_D("no can dev be used to send msg.\n");
    }
}

#endif // #ifdef RT_USING_CAN


//       end of file  
