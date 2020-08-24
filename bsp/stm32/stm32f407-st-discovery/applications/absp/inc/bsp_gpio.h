#ifndef _BSP_GPIO_H_
#define _BSP_GPIO_H_

//..........................头文件.......................................
#include <rtthread.h>
#include <drv_gpio.h>
#include "basic_gpio.h"

/*********************************************************************************************************
  宏定义
*********************************************************************************************************/
//..............................................输入IO脚宏定义.................................

//#define CHARGE_STATUS_PIN               GET_PIN(E, 2)     //充电器状态信号
#define WAKEUP_STATUS_PIN              GET_PIN(G, 15)     //wakeup状态信号
#define SPEED0_STATUS_PIN              GET_PIN(G, 12)     //零速状态信号
#define EBCU_STATUS_PIN                GET_PIN(G, 10)     //EBCU状态信号
#define POWER_STATUS_PIN              GET_PIN(E, 0)     //充电机状态信号

#define K1_FEED_PIN                    GET_PIN(E, 1)     //K1反馈节点信号
#define K2_FEED_PIN                    GET_PIN(A, 4)     //K2反馈节点信号
#define K7_FEED_PIN                    GET_PIN(E, 2)     //K7反馈节点信号
#define K3_FEED_PIN                    GET_PIN(B, 7)     //K3反馈节点信号

#define CARID_A_FEED_PIN               GET_PIN(B, 5)      //车号A设置
#define CARID_B_FEED_PIN               GET_PIN(C, 3)      //车号B设置
#define CARID_C_FEED_PIN               GET_PIN(B, 0)      //车号C设置
#define CARID_D_FEED_PIN               GET_PIN(A, 3)      //车号D设置

//.............................................输出I/O脚宏定义..................................
#define MCU_4524A0_PIN                 GET_PIN(D, 13)     //被动均衡选择器0
#define MCU_4524A1_PIN                 GET_PIN(E, 12)     //被动均衡选择器1
#define MCU_4524A2_PIN                 GET_PIN(E, 14)     //被动均衡选择器2
#define MCU_4524A3_PIN                 GET_PIN(E, 13)     //被动均衡选择器3
#define MCU_4524EL_PIN                 GET_PIN(D, 10)     //被动均衡使能

#define MCU_38A1_PIN                   GET_PIN(G, 6)     //单体电池电压选择器A1
#define MCU_38B1_PIN                   GET_PIN(G, 3)     //单体电池电压选择器B1
#define MCU_38C1_PIN                   GET_PIN(G, 4)     //单体电池电压选择器C1
#define MCU_38E1_PIN                   GET_PIN(G, 2)     //单体电池电压选择器1使能
#define MCU_38A2_PIN                   GET_PIN(F, 0)     //单体电池电压选择器A2
#define MCU_38B2_PIN                   GET_PIN(F, 1)     //单体电池电压选择器B2
#define MCU_38C2_PIN                   GET_PIN(G, 7)     //单体电池电压选择器C2
#define MCU_38E2_PIN                   GET_PIN(G, 5)     //单体电池电压选择器2使能

#define MCU_67S0_PIN                   GET_PIN(F, 2)     //单体电池温度选择器S0
#define MCU_67S1_PIN                   GET_PIN(E, 6)     //单体电池温度选择器S1
#define MCU_67S2_PIN                   GET_PIN(E, 4)     //单体电池温度选择器S2
#define MCU_67S3_PIN                   GET_PIN(C, 2)     //单体电池温度选择器S3
#define MCU_67E_PIN                    GET_PIN(E, 5)     //单体电池温度选择器使能

#define K1_START_PIN                   GET_PIN(F, 13)     //K1驱动信号
#define K2_START_PIN                   GET_PIN(F, 12)     //K2驱动信号
#define K7_START_PIN                   GET_PIN(F, 15)     //K7驱动信号
#define K3_START_PIN                   GET_PIN(F, 14)     //K3驱动信号



//.............................................输入反馈值宏定义...............................
//#define CHARGE_STATUS_VALUE               rt_pin_read(CHARGE_STATUS_PIN)     //充电器状态信号
#define WAKEUP_STATUS_VALUE               rt_pin_read(WAKEUP_STATUS_PIN)     //wakeup状态信号
#define SPEED0_STATUS_VALUE               rt_pin_read(SPEED0_STATUS_PIN)     //零速状态信号
#define EBCU_STATUS_VALUE                 rt_pin_read(EBCU_STATUS_PIN)         //EBCU状态信号
#define POWER_STATUS_VALUE                rt_pin_read(POWER_STATUS_PIN)       //充电机状态信号反馈

#define K1_FEED_VALUE                    rt_pin_read(K1_FEED_PIN)          //K1反馈
#define K2_FEED_VALUE                    rt_pin_read(K2_FEED_PIN)          //K2反馈
#define K7_FEED_VALUE                    rt_pin_read(K7_FEED_PIN)          //K7反馈
#define K3_FEED_VALUE                    rt_pin_read(K3_FEED_PIN)          //K3反馈

#define CARID_A_FEED_VALUE               rt_pin_read(CARID_A_FEED_PIN)      //车号A设置
#define CARID_B_FEED_VALUE               rt_pin_read(CARID_B_FEED_PIN)      //车号B设置
#define CARID_C_FEED_VALUE               rt_pin_read(CARID_C_FEED_PIN)      //车号C设置
#define CARID_D_FEED_VALUE               rt_pin_read(CARID_D_FEED_PIN)      //车号D设置

//...........................................输出引脚置位.................................
#define MCU_4524A0_PIN_ON                 rt_pin_write(MCU_4524A0_PIN, PIN_HIGH);     //被动均衡选择器0位
#define MCU_4524A0_PIN_OFF                rt_pin_write(MCU_4524A0_PIN, PIN_LOW);      

#define MCU_4524A1_PIN_ON                 rt_pin_write(MCU_4524A1_PIN, PIN_HIGH);     //被动均衡选择器1位
#define MCU_4524A1_PIN_OFF                rt_pin_write(MCU_4524A1_PIN, PIN_LOW);     

#define MCU_4524A2_PIN_ON                 rt_pin_write(MCU_4524A2_PIN, PIN_HIGH);     //被动均衡选择器2位
#define MCU_4524A2_PIN_OFF                rt_pin_write(MCU_4524A2_PIN, PIN_LOW);

#define MCU_4524A3_PIN_ON                 rt_pin_write(MCU_4524A3_PIN, PIN_HIGH);     //被动均衡选择器3位
#define MCU_4524A3_PIN_OFF                rt_pin_write(MCU_4524A3_PIN, PIN_LOW);

#define MCU_4524EL_PIN_ON                  rt_pin_write(MCU_4524EL_PIN, PIN_HIGH);      //被动均衡选择器使能
#define MCU_4524EL_PIN_OFF                 rt_pin_write(MCU_4524EL_PIN, PIN_LOW);

#define MCU_38A1_PIN_ON                   rt_pin_write(MCU_38A1_PIN, PIN_HIGH);   //单体电池电压选择器A1
#define MCU_38A1_PIN_OFF                  rt_pin_write(MCU_38A1_PIN, PIN_LOW);

#define MCU_38B1_PIN_ON                 rt_pin_write(MCU_38B1_PIN, PIN_HIGH);     //单体电池电压选择器B1
#define MCU_38B1_PIN_OFF                rt_pin_write(MCU_38B1_PIN, PIN_LOW);

#define MCU_38C1_PIN_ON               rt_pin_write(MCU_38C1_PIN, PIN_HIGH);   //单体电池电压选择器C1
#define MCU_38C1_PIN_OFF              rt_pin_write(MCU_38C1_PIN, PIN_LOW);

#define MCU_38E1_PIN_ON               rt_pin_write(MCU_38E1_PIN, PIN_HIGH);   //单体电池电压选择器1使能
#define MCU_38E1_PIN_OFF              rt_pin_write(MCU_38E1_PIN, PIN_LOW);

#define MCU_38A2_PIN_ON                 rt_pin_write(MCU_38A2_PIN, PIN_HIGH);     //单体电池电压选择器A2
#define MCU_38A2_PIN_OFF                rt_pin_write(MCU_38A2_PIN, PIN_LOW);

#define MCU_38B2_PIN_ON                  rt_pin_write(MCU_38B2_PIN, PIN_HIGH);      //单体电池电压选择器B2
#define MCU_38B2_PIN_OFF                 rt_pin_write(MCU_38B2_PIN, PIN_LOW);

#define MCU_38C2_PIN_ON                rt_pin_write(MCU_38C2_PIN, PIN_HIGH);    //单体电池电压选择器C2
#define MCU_38C2_PIN_OFF               rt_pin_write(MCU_38C2_PIN, PIN_LOW);

#define MCU_38E2_PIN_ON                 rt_pin_write(MCU_38E2_PIN, PIN_HIGH);  //单体电池电压选择器2使能
#define MCU_38E2_PIN_OFF                rt_pin_write(MCU_38E2_PIN, PIN_LOW);

#define MCU_67S0_PIN_ON                  rt_pin_write(MCU_67S0_PIN, PIN_HIGH);  //单体电池温度选择器0
#define MCU_67S0_PIN_OFF                  rt_pin_write(MCU_67S0_PIN, PIN_LOW);

#define MCU_67S1_PIN_ON                 rt_pin_write(MCU_67S1_PIN, PIN_HIGH);  //单体电池温度选择器1
#define MCU_67S1_PIN_OFF                   rt_pin_write(MCU_67S1_PIN, PIN_LOW);

#define MCU_67S2_PIN_ON                  rt_pin_write(MCU_67S2_PIN, PIN_HIGH);   //单体电池温度选择器2
#define MCU_67S2_PIN_OFF                 rt_pin_write(MCU_67S2_PIN, PIN_LOW);

#define MCU_67S3_PIN_ON                 rt_pin_write(MCU_67S3_PIN, PIN_HIGH);     //单体电池温度选择器3
#define MCU_67S3_PIN_OFF                rt_pin_write(MCU_67S3_PIN, PIN_LOW);

#define MCU_67E_PIN_ON                  rt_pin_write(MCU_67E_PIN, PIN_HIGH);        //单体电池温度选择器使能
#define MCU_67E_PIN_OFF                 rt_pin_write(MCU_67E_PIN, PIN_LOW);

#define K1_START_PIN_ON                 rt_pin_write(K1_START_PIN, PIN_HIGH);       //K1线圈驱动
#define K1_START_PIN_OFF                rt_pin_write(K1_START_PIN, PIN_LOW);

#define K2_START_PIN_ON                 rt_pin_write(K2_START_PIN, PIN_HIGH);     //K2线圈驱动
#define K2_START_PIN_OFF                rt_pin_write(K2_START_PIN, PIN_LOW);

#define K7_START_PIN_ON                  rt_pin_write(K7_START_PIN, PIN_HIGH);      //K7线圈驱动
#define K7_START_PIN_OFF                 rt_pin_write(K7_START_PIN, PIN_LOW);

#define K3_START_PIN_ON                  rt_pin_write(K3_START_PIN, PIN_HIGH);      //K3线圈驱动
#define K3_START_PIN_OFF                 rt_pin_write(K3_START_PIN, PIN_LOW);

extern void bsp_gpio_init(void) ;
#endif





