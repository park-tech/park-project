#ifndef __BSP_BKP_H__
#define __BSP_BKP_H__
#include <board.h>

#define DOWNLOAD_APP 		1
#define NO_DOWNLOAD_APP     0

uint8_t whether_downloaded(void);
void download_app_mark_set(void);
void download_app_mark_clear(void);
void start_times_clear(void);
int BKP_Init(void);


#endif


