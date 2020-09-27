
#ifndef __BASIC_CAN_H
#define __BASIC_CAN_H

#include <rtdevice.h>
#include <board.h>
#include "rtdbg.h"

extern void basic_send_can(uint8_t dev, uint32_t id, uint32_t ide, uint8_t len, uint8_t *buf);

#endif
