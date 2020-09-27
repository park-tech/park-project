#ifndef _BASIC_UART_H_
#define _BASIC_UART_H_
//..........................头文件.......................................

#include <rtthread.h>
#include <rtdevice.h>


#if defined(BSP_USING_UART1)
extern rt_device_t serial_uart1;     //设备号  句柄
extern struct rt_semaphore rx_sem1;   //信号量
#endif

#if defined(BSP_USING_UART3)
extern rt_device_t serial_uart3;
extern struct rt_semaphore rx_sem3;
#endif

#if defined(BSP_USING_UART4)
extern rt_device_t serial_uart4;
extern struct rt_semaphore rx_sem4;
#endif

#if defined(BSP_USING_UART5)
extern rt_device_t serial_uart5;
extern struct rt_semaphore rx_sem5;
#endif

#if defined(BSP_USING_UART6)
extern rt_device_t serial_uart6;
extern struct rt_semaphore rx_sem6;
#endif

#if defined(BSP_USING_UART1)
extern rt_err_t basic_uart1_init(uint32_t baudrate);
#endif
#if defined(BSP_USING_UART3) //用于2G了，一般不要使用
extern rt_err_t basic_uart3_init(uint32_t baudrate);
#endif
#if defined(BSP_USING_UART4)
extern rt_err_t basic_uart4_init(uint32_t baudrate);
#endif
#if defined(BSP_USING_UART5)
extern rt_err_t basic_uart5_init(uint32_t baudrate);
#endif
#if defined(BSP_USING_UART6)
extern rt_err_t basic_uart6_init(uint32_t baudrate);
#endif

extern rt_err_t basic_uart4_input(rt_device_t dev, rt_size_t size);

#endif
