/**************************************************************************************
 *  Copyright :  zhizhuo
 *  Date :     2019.10.22
 *  function: 故障数据的存储
 *  describe：1. 如何挂载未nandflash编写。
             
****************************************************************************************/
#include "fmod_store_fault.h"

//........................................宏定义.........................................
#define  HISTROY_DATA_LEN   60                       // 每1组数据的字节长度
#define  HISTROY_TIME_PATH_NAME   "fault_data\\fault_store_time.hex"  //单独用来存放故障故障的时间的文件路径和名称

//........................................变量定义.........................................
struct  history_store_data   st_fault_wdata;    //故障数据存储
union   Fault_file_time      un_fault_rtime;    //读故障数据存储文件的时间记录 共记录两组数据，低二组数据为最新的数据
union   Fault_file_time      un_fault_wtime;    //写故障数据存储文件的时间记录

char    fault_path[60] = "" ;   //存放故障数据的路径

//........................................函数申明.........................................
static void  fmod_fault_time_read(void);
static void  fmod_fault_time_write(void);
static void  fmod_set_fault_pathname(void);
static char *fmod_make_fault_jsonbody(void);

static void fmod_fault_store_update(void);
static void fmod_fault_data_uptime(void);
static void fmod_fault_batcore_updata(void);

/******************************************************************************************
** 函数名称：
** 函数描述：sdcard数据存储
** 输入参数：*Jsondata：需要存储的数据
** 返回值	：无
*******************************************************************************************/
void fmod_fault_store(void)
{ 
	static uint8_t  fault_count = 0;
	char *jsonBody = NULL;
	//uint32_t len=0;

	if(nandflash_mount_flag) //挂载成功
	{
		if(fault_count == 0)
		{
			fault_count = 1;
			//........................启动时读取存取的时间.......................
			fmod_fault_time_read( );
			//..........................设置存故障存储的文件名称.................
			fmod_set_fault_pathname( );
		}

		//.........................数据更新..........................
		fmod_fault_store_update( );

		//........................封装Json...........................
		jsonBody = fmod_make_fault_jsonbody( );	
		
		//......................打印封装好的内存......................
//		printf("\n");							 
//		printf("%s\n",jsonBody);
		
		//......................存储数据在nandflash卡中.................. 
		basic_add_filedata(fault_path,  jsonBody, strlen(jsonBody), 1);		
      		
		//......................存储数据在nandflash卡中.................. 
		free(jsonBody);	
	}
}
/*********************************************************************************************************
** 函数名称：
** 函数描述：读取存储的存储数据文件的时间（单独建立一个文件放置每个存储文件的时间）
** 输入参数：
** 返回值  ：无
*********************************************************************************************************/
static void fmod_fault_time_read(void )
{
    union Fault_file_time   un_savetime;
	
    basic_read_filedata(HISTROY_TIME_PATH_NAME, un_savetime.u8_data, sizeof(un_savetime.u8_data),0 );
	
	if((un_savetime.st_data.u8_headdata[0] == 0XBB)  && (un_savetime.st_data.u8_enddata[1] == 0XCC) )
	{
		un_fault_rtime = un_savetime;                           //读取到存储的时间
	}	
}
/*********************************************************************************************************
** 函数名称：写故障存储时间
** 函数描述：
** 输入参数：
** 返回值  ：无
*********************************************************************************************************/
static void fmod_fault_time_write(void)
{
	un_fault_wtime.st_data.u8_headdata[0] = 0XBB;
	un_fault_wtime.st_data.u8_headdata[1] = 0X11;
	un_fault_wtime.st_data.u8_enddata[0] = 0X22;
	un_fault_wtime.st_data.u8_enddata[1] = 0XCC;	
	
	basic_write_filedata(HISTROY_TIME_PATH_NAME, un_fault_wtime.u8_data, sizeof(un_fault_wtime.u8_data), 0) ;	
    
}

/*********************************************************************************************************
** 函数名称：数据更新
** 函数描述：
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
static void fmod_fault_store_update(void)
{  
	if(st_fault_wdata.u32_sd_num < 0xFFFFFFF)  st_fault_wdata.u32_sd_num++;
	else  st_fault_wdata.u32_sd_num = 0;
	
	strcpy(st_fault_wdata.i8_project_name, st_product_info.i8_project_name);
	strcpy(st_fault_wdata.i8_product_name, st_product_info.i8_product_name);
	st_fault_wdata.u16_product_num = st_product_info.u16_product_num;
	
	st_fault_wdata.st_bat_data = st_bat_data;

	fmod_fault_batcore_updata( );
	
	st_fault_wdata.un_bat_err1.u16_all = un_bat_err1.u16_all;
	st_fault_wdata.un_bat_err2.u16_all = un_bat_err2.u16_all;
	st_fault_wdata.un_bat_lock.u16_all = un_bat_lock.u16_all;
	st_fault_wdata.un_sys_Inout.u8_all = un_sys_Inout.u8_all;
	st_fault_wdata.un_sys_status.u8_all = un_sys_status.u8_all;



	strcpy(st_fault_wdata.i8_version, st_product_info.i8_soft_version);
	fmod_fault_data_uptime( ); 	  //上传时间	
    
}


/******************************************************************************************
** 函数名称：
** 函数描述：制作Json封装
** 输入参数：
** 返回值	：无
*******************************************************************************************/

static char *fmod_make_fault_jsonbody(void)
{
	cJSON * pJsonRoot = NULL;
	cJSON * pSubJson = NULL;
	char * p = NULL;


	pJsonRoot = cJSON_CreateObject( );
	if(NULL == pJsonRoot) //是否创建成功
	{
	    cJSON_Delete(pJsonRoot);
	    return NULL;
	}
	cJSON_AddItemToObject(pJsonRoot, "product_num", cJSON_CreateNumber(st_fault_wdata.u16_product_num));
    cJSON_AddItemToObject(pJsonRoot, "SN",  cJSON_CreateNumber(st_fault_wdata.u32_sd_num));
  
   
    pSubJson = cJSON_CreateObject( );
    if(NULL == pSubJson)
    { 
		cJSON_Delete(pJsonRoot);
		return NULL;
    }

	cJSON_AddItemToObject(pSubJson, "bat_volt", cJSON_CreateNumber(st_fault_wdata.st_bat_data.fl_bat_volt));
	cJSON_AddItemToObject(pSubJson, "bat_chI", cJSON_CreateNumber(st_fault_wdata.st_bat_data.fl_bat_chI));
	cJSON_AddItemToObject(pSubJson, "bat_dischI", cJSON_CreateNumber(st_fault_wdata.st_bat_data.fl_bat_dischI));
	cJSON_AddItemToObject(pSubJson, "bat_Qc_max", cJSON_CreateNumber(st_fault_wdata.st_bat_data.fl_bat_Qc_max));
	cJSON_AddItemToObject(pSubJson, "bat_Qnow", cJSON_CreateNumber(st_fault_wdata.st_bat_data.fl_bat_Qnow));	
	cJSON_AddItemToObject(pSubJson, "bat_soc", cJSON_CreateNumber(st_fault_wdata.st_bat_data.fl_bat_soc));
	cJSON_AddItemToObject(pSubJson, "bat_soh", cJSON_CreateNumber(st_fault_wdata.st_bat_data.fl_bat_soh));
	cJSON_AddItemToObject(pSubJson, "batcore_max_volt", cJSON_CreateNumber(st_fault_wdata.st_bat_data.u16_batcore_max_volt));
	cJSON_AddItemToObject(pSubJson, "batcore_min_volt", cJSON_CreateNumber(st_fault_wdata.st_bat_data.u16_batcore_min_volt));
	cJSON_AddItemToObject(pSubJson, "bat_max_temp", cJSON_CreateNumber(st_fault_wdata.st_bat_data.fl_bat_max_temp));
	cJSON_AddItemToObject(pSubJson, "bat_min_temp", cJSON_CreateNumber(st_fault_wdata.st_bat_data.fl_bat_min_temp));
	cJSON_AddItemToObject(pSubJson, "charger_volt", cJSON_CreateNumber(st_fault_wdata.st_bat_data.fl_charger_volt1));
	cJSON_AddItemToObject(pSubJson, "charger_volt", cJSON_CreateNumber(st_fault_wdata.st_bat_data.fl_charger_volt2));
	cJSON_AddItemToObject(pSubJson, "bat_dischI1", cJSON_CreateNumber(st_fault_wdata.st_bat_data.fl_bat_dischI1));
	cJSON_AddItemToObject(pSubJson, "bat_dischI2", cJSON_CreateNumber(st_fault_wdata.st_bat_data.fl_bat_dischI2));
	
	cJSON_AddItemToObject(pSubJson, "bc_volt", cJSON_CreateString(st_fault_wdata.batcore_volt_string));  

	cJSON_AddItemToObject(pSubJson, "bat_err1", cJSON_CreateNumber(st_fault_wdata.un_bat_err1.u16_all));
	cJSON_AddItemToObject(pSubJson, "bat_err2", cJSON_CreateNumber(st_fault_wdata.un_bat_err2.u16_all));
	cJSON_AddItemToObject(pSubJson, "bat_lock", cJSON_CreateNumber(st_fault_wdata.un_bat_lock.u16_all));
	cJSON_AddItemToObject(pSubJson, "sys_Inout", cJSON_CreateNumber(st_fault_wdata.un_sys_Inout.u8_all));
	cJSON_AddItemToObject(pSubJson, "sys_status", cJSON_CreateNumber(st_fault_wdata.un_sys_status.u8_all));
	

	cJSON_AddItemToObject(pJsonRoot,  "APS_bms_data", pSubJson);

    cJSON_AddItemToObject(pJsonRoot, "version", cJSON_CreateString(st_fault_wdata.i8_version));
    cJSON_AddItemToObject(pJsonRoot, "uptime",  cJSON_CreateString(st_fault_wdata.i8_timestring));

    p = cJSON_Print(pJsonRoot);
    if(NULL == p)
    {
		cJSON_Delete(pJsonRoot);
		return NULL;
    }

    cJSON_Delete(pJsonRoot);

    return p;
}
/******************************************************************************************
** 函数名称：
** 函数描述：确定SD卡存储文件的文件名
** 输入参数：
** 返回值	：无
*******************************************************************************************/
static void fmod_set_fault_pathname(void) 
{
	struct tm real_tm;
	uint8_t i = 0;
	int  res = 0;
	char fault_filename[60] = "";
	FINFO file_info;

    get_rtc_time( &real_tm);

	//.................如果电池首次上电，当前时间作为第一次文件建立时间........................
	if((un_fault_rtime.st_data.st_tm[2].u8_year == 0) && (un_fault_rtime.st_data.st_tm[2].u8_mon == 0) )	 
	{	
		un_fault_wtime.st_data.st_tm[2].u8_year = real_tm.tm_year;
		un_fault_wtime.st_data.st_tm[2].u8_mon =  real_tm.tm_mon;
		un_fault_wtime.st_data.st_tm[2].u8_day =  real_tm.tm_mday;
		//...........................对时间进行存储....................
        fmod_fault_time_write( );	
		//....................................确定文件名称........................
	    snprintf(fault_filename, sizeof(fault_filename), "fault_data\\fault_%04d-%02d-%02d.txt", 
		     un_fault_wtime.st_data.st_tm[2].u8_year+1900, un_fault_wtime.st_data.st_tm[2].u8_mon+1, un_fault_wtime.st_data.st_tm[2].u8_day);
        strcpy(fault_path, fault_filename);
        return	;	
	} 
	
	//....................................非首次上电 查看原来的文件是否已经超过指定大小........................
	snprintf(fault_filename, sizeof(fault_filename), "fault_data\\fault_%04d-%02d-%02d.txt", 
				un_fault_rtime.st_data.st_tm[2].u8_year+1900, un_fault_rtime.st_data.st_tm[2].u8_mon+1, un_fault_rtime.st_data.st_tm[2].u8_day);	
   basic_get_filesize(fault_filename, &file_info) ;//读取文件的大小	
	//rt_kprintf("status file size = %d\n", file_info.size);     //字节数
	if( file_info.size < (12 * 1024* 1024) )                //文件大于2M 则删除最早的文件
	{
		//....................................确定文件名称........................
		snprintf(fault_filename, sizeof(fault_filename), "fault_data\\fault_%04d-%02d-%02d.txt", 
				un_fault_rtime.st_data.st_tm[2].u8_year+1900, un_fault_rtime.st_data.st_tm[2].u8_mon+1, un_fault_rtime.st_data.st_tm[2].u8_day);		
	}
    else
	{	
		if(un_fault_rtime.st_data.st_tm[0].u8_year > 0)   //到达最高个数应该删除最早的那一个文件了
		{
			snprintf(fault_filename, sizeof(fault_filename), "fault_data\\fault_%04d-%02d-%02d.txt", 
						un_fault_rtime.st_data.st_tm[0].u8_year+1900, un_fault_rtime.st_data.st_tm[0].u8_mon+1,un_fault_rtime.st_data.st_tm[0].u8_day);

            res = unlink(fault_filename);
			if(res == 0) //删除文件成功
			{
				rt_kprintf("%s unlink file successful!\n", fault_filename); 
			}	
		}
		
		//..................更新时间记录...............................
		for(i = 0; i < 2; i++) //前5次的记录时间一次前移
		{
			un_fault_wtime.st_data.st_tm[i].u8_year = un_fault_wtime.st_data.st_tm[i+1].u8_year;
			un_fault_wtime.st_data.st_tm[i].u8_mon = un_fault_wtime.st_data.st_tm[i+1].u8_mon;
			un_fault_wtime.st_data.st_tm[i].u8_day = un_fault_wtime.st_data.st_tm[i+1].u8_day;
		}	 
		//........................此时的时间为新文件的名称..................
		un_fault_wtime.st_data.st_tm[2].u8_year = real_tm.tm_year;  
		un_fault_wtime.st_data.st_tm[2].u8_mon =  real_tm.tm_mon;
		un_fault_wtime.st_data.st_tm[2].u8_day =  real_tm.tm_mday;
		
		//...........................对时间进行存储....................
		fmod_fault_time_write( );	
		//....................................确定文件名称........................
		snprintf(fault_filename, sizeof(fault_filename), "fault_data\\fault_%04d-%02d-%02d.txt", 
		     un_fault_wtime.st_data.st_tm[2].u8_year+1900, un_fault_wtime.st_data.st_tm[2].u8_mon+1, un_fault_wtime.st_data.st_tm[2].u8_day);	
	}	
	strcpy(fault_path, fault_filename);
}
/******************************************************************************************
** 函数名称：
** 函数描述：JSON数据的上传时间设定 将字节转华为字符
** 输入参数：
** 返回值	：无
*******************************************************************************************/
static void fmod_fault_data_uptime(void)
{
	char time_buf[6] = {0};
    struct tm  rtc_tm;
	
	get_rtc_time(&rtc_tm);
	memset(st_fault_wdata.i8_timestring, 0x0, 20); 
    //.............	timestring[4][7]为- [10]空格,[13] [16]为“:” 如2018-01-01 00:00:00......................
    sprintf(time_buf, "%4d", rtc_tm.tm_year+1900);	
	st_fault_wdata.i8_timestring[0] = time_buf[0];
	st_fault_wdata.i8_timestring[1] = time_buf[1];
	st_fault_wdata.i8_timestring[2] = time_buf[2];
	st_fault_wdata.i8_timestring[3] = time_buf[3];
	st_fault_wdata.i8_timestring[4] = 0X2D; // "-"

	sprintf(time_buf, "%02d", rtc_tm.tm_mon+1);
	st_fault_wdata.i8_timestring[5] = time_buf[0];
	st_fault_wdata.i8_timestring[6] = time_buf[1];
	st_fault_wdata.i8_timestring[7] = 0X2D;// "-"
	
	sprintf(time_buf, "%02d", rtc_tm.tm_mday);
	st_fault_wdata.i8_timestring[8] = time_buf[0];
	st_fault_wdata.i8_timestring[9] = time_buf[1];
	st_fault_wdata.i8_timestring[10] =  0X20;	//空格
	
	sprintf(time_buf, "%02d", rtc_tm.tm_hour);
	st_fault_wdata.i8_timestring[11] = time_buf[0];
	st_fault_wdata.i8_timestring[12] = time_buf[1];	
	st_fault_wdata.i8_timestring[13] = 0X3A;  //“:”

	sprintf(time_buf,  "%02d:", rtc_tm.tm_min);
	st_fault_wdata.i8_timestring[14] = time_buf[0];
	st_fault_wdata.i8_timestring[15] = time_buf[1];
	st_fault_wdata.i8_timestring[16] = 0X3A;  //“:”

	sprintf(time_buf,  "%02d", rtc_tm.tm_sec);
	st_fault_wdata.i8_timestring[17] = time_buf[0];
	st_fault_wdata.i8_timestring[18] = time_buf[1];
	
}
/******************************************************************************************
** 函数名称：
** 函数描述：单体电芯的数据字节转华为字符串
** 输入参数：
** 返回值	：无
*******************************************************************************************/
static void fmod_fault_batcore_updata(void)
{
    uint8_t i = 0;
	char  batcore_data[6] = {0};

    for(i = 0; i < 12; i++ ) 
    {  
	    sprintf(batcore_data, "%04x", st_batcore_data.u16_batcore_volt[i]);
		st_fault_wdata.batcore_volt_string[4*i]   = batcore_data[0];
		st_fault_wdata.batcore_volt_string[4*i+1] = batcore_data[1];
		st_fault_wdata.batcore_volt_string[4*i+2] = batcore_data[2];
		st_fault_wdata.batcore_volt_string[4*i+3] = batcore_data[3];
    }

	
}
/********************************************************************************************
    end file
********************************************************************************************/
