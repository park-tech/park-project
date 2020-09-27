#ifndef _BASIC_FLASH_H_
#define _BASIC_FLASH_H_
//..........................头文件.......................................
#include <rtthread.h>
#include "drv_nand.h"
#include <rtdevice.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "board.h"
#include "File_Config.h"

extern uint8_t nandflash_mount_flag;


//#include <dfs_posix.h>
extern rt_err_t basic_nandflash_mount_status(void);
extern uint8_t *basic_read_filedata(char *path_name, uint8_t *u8_data, uint8_t len, uint32_t offest );
extern rt_err_t basic_write_filedata(char *pathname, uint8_t *u8_data,  uint8_t len, uint32_t offest);
extern rt_err_t basic_add_filedata(char *pathname, char *ptr, uint32_t len, uint8_t bsize);
extern rt_err_t basic_get_filesize(const char *pathname, FINFO *info);
#endif
