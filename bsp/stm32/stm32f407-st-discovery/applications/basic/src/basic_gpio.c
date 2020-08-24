#include "basic_gpio.h"


void basic_gpio_exti_init(uint8_t pin_num)
{
    /* 引脚为输入模式 */
    rt_pin_mode(pin_num, PIN_MODE_INPUT_PULLUP);
    /* 绑定中断，上升沿模式，回调函数名为beep_on */
    rt_pin_attach_irq(pin_num, PIN_IRQ_MODE_FALLING, gpio_exti_out, RT_NULL);
    /* 使能中断 */
    rt_pin_irq_enable(pin_num, PIN_IRQ_ENABLE);
}
