/******************************************************************************************
** 头文件
*******************************************************************************************/
#include "basic_uart.h"
/********************************************************************************************
宏定义
********************************************************************************************/
/********************************************************************************************
变量定义
********************************************************************************************/
#if defined(BSP_USING_UART1)
rt_device_t serial_uart1;     //设备号  句柄
struct rt_semaphore rx_sem1;   //信号量
#endif

#if defined(BSP_USING_UART3)
rt_device_t serial_uart3;
struct rt_semaphore rx_sem3;
#endif

#if defined(BSP_USING_UART4)
rt_device_t serial_uart4;
struct rt_semaphore rx_sem4;
#endif

#if defined(BSP_USING_UART5)
rt_device_t serial_uart5;
struct rt_semaphore rx_sem5;
#endif

#if defined(BSP_USING_UART6)
rt_device_t serial_uart6;
struct rt_semaphore rx_sem6;
#endif

/********************************************************************************************
函数申明
********************************************************************************************/
#if defined(BSP_USING_UART1)
/*** @ : ***************************************************************************************
 * @name: 
 * @describe: uart1接收数据回调函数
 * @param :   dev：设备号   size：大小
 * @return: 
 * @  : ****************************************************************************************/
static rt_err_t basic_uart1_input(rt_device_t dev, rt_size_t size)
{
     //................ 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量....................
    rt_sem_release(&rx_sem1);
    return RT_EOK;
}

/********************************************************************************************
** 函数名称：     
** 函数描述：uart1的初始化
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
rt_err_t basic_uart1_init(uint32_t baudrate)
{
   
    rt_err_t ret = RT_EOK;
    
    //..............................查找串口...............................
    serial_uart1 = rt_device_find("uart1");
    if(!serial_uart1)
    {
        rt_kprintf("find uart1 failed!\n");
        return RT_ERROR;
    }
    
    /* 初始化信号量 */
    rt_sem_init(&rx_sem1, "rx_sem1", 0, RT_IPC_FLAG_FIFO);
  
    //................... 串口配置结构体，使用serial.h的宏定义................
    struct serial_configure gps_use_config = 
    {
        baudrate,               // 波特率 bits/s  #define BAUD_RATE_9600  9600 #define BAUD_RATE_19200    19200
        DATA_BITS_8,            /* 8 databits */
        STOP_BITS_1,            /* 1 stopbit */
        PARITY_NONE,            /* No parity  */ 
        BIT_ORDER_LSB,          /* LSB first sent */
        NRZ_NORMAL,             /* Normal mode */
        RT_SERIAL_RB_BUFSZ,     /* 接收数据缓冲区大小 */
        0   
    };
    
    //..............................打开串口设备............................
    rt_device_open(serial_uart1, RT_DEVICE_FLAG_INT_RX);        /* DMA模式 */
   
    //..............................修改串口参数配置............................
    if (RT_EOK != rt_device_control(serial_uart1, RT_DEVICE_CTRL_CONFIG, (void *)&gps_use_config))
    {
        rt_kprintf("uart1 config baud rate failed.\n");
        return RT_ERROR;
    }
     //..............................设置设备接收回调函数............................
    rt_device_set_rx_indicate(serial_uart1,  basic_uart1_input); 
    return ret;
}
#endif

#if defined(BSP_USING_UART3)
/*** @ : ***************************************************************************************
 * @name: 
 * @describe: uart3接收数据回调函数
 * @param :   dev：设备号   size：大小
 * @return: 
 * @  : ****************************************************************************************/
static rt_err_t basic_uart3_input(rt_device_t dev, rt_size_t size)
{
     //................ 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量....................
    rt_sem_release(&rx_sem3);
    return RT_EOK;
}

/********************************************************************************************
** 函数名称：     
** 函数描述：uart3的初始化
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
rt_err_t basic_uart3_init(uint32_t baudrate)
{
   
    rt_err_t ret = RT_EOK;
    
    //..............................查找串口...............................
    serial_uart3 = rt_device_find("uart3");
    if(!serial_uart3)
    {
        rt_kprintf("find uart3 failed!\n");
        return RT_ERROR;
    }
    
    /* 初始化信号量 */
    rt_sem_init(&rx_sem3, "rx_sem3", 0, RT_IPC_FLAG_FIFO);
  
    //................... 串口配置结构体，使用serial.h的宏定义................
    struct serial_configure gps_use_config = 
    {
        baudrate,               // 波特率 bits/s  #define BAUD_RATE_9600  9600 #define BAUD_RATE_19200 
        DATA_BITS_8,            /* 8 databits */
        STOP_BITS_1,            /* 1 stopbit */
        PARITY_NONE,            /* No parity  */ 
        BIT_ORDER_LSB,          /* LSB first sent */
        NRZ_NORMAL,             /* Normal mode */
        RT_SERIAL_RB_BUFSZ,    /* 接收数据缓冲区大小 */
        0   
    };
    
    //..............................打开串口设备............................
    rt_device_open(serial_uart3, RT_DEVICE_FLAG_INT_RX);        /* DMA模式 */
   
    //..............................修改串口参数配置............................
    if (RT_EOK != rt_device_control(serial_uart3, RT_DEVICE_CTRL_CONFIG, (void *)&gps_use_config))
    {
        rt_kprintf("uart3 config baud rate failed.\n");
        return RT_ERROR;
    }
     //..............................设置设备接收回调函数............................
    rt_device_set_rx_indicate(serial_uart3,  basic_uart3_input); 
    return ret;
}
#endif

#if defined(BSP_USING_UART4)
/*** @ : ***************************************************************************************
 * @name: 
 * @describe: uart4接收数据回调函数
 * @param :   dev：设备号   size：大小
 * @return: 
 * @  : ****************************************************************************************/
rt_err_t basic_uart4_input(rt_device_t dev, rt_size_t size)
{
     //................ 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量....................
    rt_sem_release(&rx_sem4);
    return RT_EOK;
}

/********************************************************************************************
** 函数名称：     
** 函数描述：uart4的初始化
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
rt_err_t basic_uart4_init(uint32_t baudrate)
{
   
    rt_err_t ret = RT_EOK;
    
    //..............................查找串口...............................
    serial_uart4 = rt_device_find("uart4");
    if(!serial_uart4)
    {
        rt_kprintf("find uart4 failed!\n");
        return RT_ERROR;
    }
    
    /* 初始化信号量 */
    rt_sem_init(&rx_sem4, "rx_sem4", 0, RT_IPC_FLAG_FIFO);
  
    //................... 串口配置结构体，使用serial.h的宏定义................
    struct serial_configure gps_use_config = 
    {
        baudrate,               // 波特率 bits/s  #define BAUD_RATE_9600  9600 #define BAUD_RATE_19200 
        DATA_BITS_8,            /* 8 databits */
        STOP_BITS_1,            /* 1 stopbit */
        PARITY_NONE,            /* No parity  */ 
        BIT_ORDER_LSB,          /* LSB first sent */
        NRZ_NORMAL,             /* Normal mode */
        RT_SERIAL_RB_BUFSZ,    /* 接收数据缓冲区大小 */
        0   
    };
    
    //..............................打开串口设备............................
   if (RT_EOK != rt_device_open(serial_uart4, RT_DEVICE_FLAG_INT_RX))       /* DMA模式 */
   {   
        return RT_ERROR;

   }
   
    //..............................修改串口参数配置............................
    if (RT_EOK != rt_device_control(serial_uart4, RT_DEVICE_CTRL_CONFIG, (void *)&gps_use_config))
    {
        rt_kprintf("uart4 config baud rate failed.\n");
        return RT_ERROR;
    }
     //..............................设置设备接收回调函数............................
    rt_device_set_rx_indicate(serial_uart4,  basic_uart4_input); 
	
    return ret;
}
#endif

#if defined(BSP_USING_UART5)
/*** @ : ***************************************************************************************
 * @name: 
 * @describe: uart5接收数据回调函数
 * @param :   dev：设备号   size：大小
 * @return: 
 * @  : ****************************************************************************************/
static rt_err_t basic_uart5_input(rt_device_t dev, rt_size_t size)
{
     //................ 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量....................
    rt_sem_release(&rx_sem5);
    return RT_EOK;
}

/********************************************************************************************
** 函数名称：     
** 函数描述：uart5的初始化
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
rt_err_t basic_uart5_init(uint32_t baudrate)
{
   
    rt_err_t ret = RT_EOK;
    
    //..............................查找串口...............................
    serial_uart5 = rt_device_find("uart5");
    if(!serial_uart5)
    {
        rt_kprintf("find uart5 failed!\n");
        return RT_ERROR;
    }
    
    /* 初始化信号量 */
    rt_sem_init(&rx_sem5, "rx_sem5", 0, RT_IPC_FLAG_FIFO);
  
    //................... 串口配置结构体，使用serial.h的宏定义................
    struct serial_configure gps_use_config = 
    {
        baudrate,               // 波特率 bits/s  #define BAUD_RATE_9600  9600 #define BAUD_RATE_19200 
        DATA_BITS_8,            /* 8 databits */
        STOP_BITS_1,            /* 1 stopbit */
        PARITY_NONE,            /* No parity  */ 
        BIT_ORDER_LSB,          /* LSB first sent */
        NRZ_NORMAL,             /* Normal mode */
        RT_SERIAL_RB_BUFSZ,    /* 接收数据缓冲区大小 */
        0   
    };
    
    //..............................打开串口设备............................
    rt_device_open(serial_uart5, RT_DEVICE_FLAG_INT_RX);        /* DMA模式 */
   
    //..............................修改串口参数配置............................
    if (RT_EOK != rt_device_control(serial_uart5, RT_DEVICE_CTRL_CONFIG, (void *)&gps_use_config))
    {
        rt_kprintf("uart5 config baud rate failed.\n");
        return RT_ERROR;
    }
     //..............................设置设备接收回调函数............................
    rt_device_set_rx_indicate(serial_uart5,  basic_uart5_input); 
    return ret;
}
#endif

#if defined(BSP_USING_UART6)
/*** @ : ***************************************************************************************
 * @name: 
 * @describe: uart6接收数据回调函数
 * @param :   dev：设备号   size：大小
 * @return: 
 * @  : ****************************************************************************************/
static rt_err_t basic_uart6_input(rt_device_t dev, rt_size_t size)
{
     //................ 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量....................
    rt_sem_release(&rx_sem6);
    return RT_EOK;
}

/********************************************************************************************
** 函数名称：     
** 函数描述：uart6的初始化
** 输入参数：无
** 返回值  ：    无
********************************************************************************************/
rt_err_t basic_uart6_init(uint32_t baudrate)
{
   
    rt_err_t ret = RT_EOK;
    
    //..............................查找串口...............................
    serial_uart6 = rt_device_find("uart6");
    if(!serial_uart6)
    {
        rt_kprintf("find uart6 failed!\n");
        return RT_ERROR;
    }
    
    /* 初始化信号量 */
    rt_sem_init(&rx_sem6, "rx_sem6", 0, RT_IPC_FLAG_FIFO);
  
    //................... 串口配置结构体，使用serial.h的宏定义................
    struct serial_configure gps_use_config = 
    {
        baudrate,               // 波特率 bits/s  #define BAUD_RATE_9600  9600 #define BAUD_RATE_19200 
        DATA_BITS_8,            /* 8 databits */
        STOP_BITS_1,            /* 1 stopbit */
        PARITY_NONE,            /* No parity  */ 
        BIT_ORDER_LSB,          /* LSB first sent */
        NRZ_NORMAL,             /* Normal mode */
        RT_SERIAL_RB_BUFSZ,    /* 接收数据缓冲区大小 */
        0   
    };
    
    //..............................打开串口设备............................
    rt_device_open(serial_uart6, RT_DEVICE_FLAG_INT_RX);        /* DMA模式 */
   
    //..............................修改串口参数配置............................
    if (RT_EOK != rt_device_control(serial_uart6, RT_DEVICE_CTRL_CONFIG, (void *)&gps_use_config))
    {
        rt_kprintf("uart6 config baud rate failed.\n");
        return RT_ERROR;
    }
     //............................设置设备接收回调函数............................
    rt_device_set_rx_indicate(serial_uart6,  basic_uart6_input); 
    return ret;
}
#endif

