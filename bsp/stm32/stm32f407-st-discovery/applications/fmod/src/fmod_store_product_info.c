/**************************************************************************************
原始信息存储：
		在程序启动前，需要读出该原始的信息，作为程序的初始值
	    设计1000条数据反复存储；
		容量大小：长度*1000  （byte）
****************************************************************************************/
/********************************************************************************************
头文件
********************************************************************************************/
#include "fmod_store_product_info.h"


#define  PRODINFO_NUM          1000       // 总共数据条数
#define  PRODINFO_DATA_LEN     24         // 每1组数据的字节长度
#define  PRODINFO_STORE_PATH   "prodinfo_store.hex"        // 故障记录存储路径
/********************************************************************************************
变量的定义e
********************************************************************************************/
union  Product_info_store  un_prodinfo_rdata;
union  Product_info_store  un_prodinfo_wdata;
uint16_t  u32_prodinfo_times = 0;        //存储偏移次数

//........................................函数申明.........................................
static uint32_t fmod_prodinfo_find_last_addr(uint16_t len);
static void fmod_prodinfo_read_lastdata(uint16_t len, uint32_t times);
static void fmod_prodinfo_store_updata(void);
static void fmod_prodinfo_store_data(void);

/******************************************************************************************
函数功能：每次刚上电时，判断是否是产品首次上电，读取最后一次存储的数据
输入参数：无
返回值：  无
*******************************************************************************************/
void  fmod_prodinfo_firsttime_read(void)
{ 
	if(nandflash_mount_flag) 
    {
		//...........................寻找最后一次存储位置和是否首次上电..........................
		u32_prodinfo_times = fmod_prodinfo_find_last_addr(PRODINFO_DATA_LEN) ;
		
		//...........................非第一次上电时，读取最后一次数据...............................	
		if(un_bat_status.st_bit.bat_first_work == 0) 
		{
			fmod_prodinfo_read_lastdata(PRODINFO_DATA_LEN, u32_prodinfo_times);
		}
		//...........................计算下一条要存的位置...........................
		u32_prodinfo_times = u32_prodinfo_times + 1;  
		if(u32_prodinfo_times >= PRODINFO_NUM  ) 
		{	
			u32_prodinfo_times = 0;	
		}
	}
}

/******************************************************************************************
函数功能：原始数据存储的过程
输入参数：数据长度
返回值 ：
*******************************************************************************************/
void fmod_prodinfo_store(void)
{	
	//.....................存储数据更新.............................
	fmod_prodinfo_store_updata( );
	
	if(nandflash_mount_flag) //挂载成功
	{
		//.....................数据存储..........................
		fmod_prodinfo_store_data( );
		
		//....................存储位置跟新.......................
		u32_prodinfo_times ++;
		if(u32_prodinfo_times >= PRODINFO_NUM   ) 
		{	
			u32_prodinfo_times = 0;	
		}
	}
	
}
/******************************************************************************************
函数功能：每次上寻找flash最后一次存储位置
输入参数：数据长度
返回值 ：last_offset 最后的偏移位置
*******************************************************************************************/
static uint32_t fmod_prodinfo_find_last_addr(uint16_t len)
{
    uint16_t i = 0, last_times = 0;          
	uint32_t real_time = 0;         //实时的时间
    uint32_t get_maxtime = 0;		//保存的最大时间
	uint16_t j = 0;
	//..................读取当前时间...........................
	get_rtc_second(&real_time);

	//...................................寻找偏移位置,只搜索每组数据第一个.........................................
	for(i = 0; i < PRODINFO_NUM; i++)   //200次故障
	{
		basic_read_filedata(PRODINFO_STORE_PATH,  un_prodinfo_rdata.u8_data, len, i * PRODINFO_DATA_LEN);	//	读取数据
			
		if(  un_prodinfo_rdata.st_data.u8_headdata[0] == 0XAA && un_prodinfo_rdata.st_data.u8_headdata[1] == PRODINFO_DATA_LEN &&
		     un_prodinfo_rdata.st_data.u8_enddata[0] == 0XAC && un_prodinfo_rdata.st_data.u8_enddata[1] == 0XCC )      
		{
            //......................根据时间找到最后一次存储的位置........................
			if(un_prodinfo_rdata.st_data.u32_time <= real_time)       //
			{	
				if(un_prodinfo_rdata.st_data.u32_time >= get_maxtime)	//等于时取最后一个记录的时间
				{
					get_maxtime = un_prodinfo_rdata.st_data.u32_time;
					last_times = i;
				}
			}
            //......................说明产品不是第一次上电........................
			un_bat_status.st_bit.bat_first_work = 0;  
		}
	}

	return last_times ;
}

/******************************************************************************************
函数功能：每次上电池读取original的上次的最后一条数据数据
输入参数： len：数据长度  times:存储的偏移次数
返回值 ：last_offset 最后一次存储位置
*******************************************************************************************/

static void fmod_prodinfo_read_lastdata(uint16_t len, uint32_t times)
{

     basic_read_filedata(PRODINFO_STORE_PATH,  un_prodinfo_rdata.u8_data, len,  len * times);	//	读取数据
	
	//.........................................读取数据内容...................................
	if(  un_prodinfo_rdata.st_data.u8_headdata[0] == 0XAA && un_prodinfo_rdata.st_data.u8_headdata[1] == PRODINFO_DATA_LEN &&
	     un_prodinfo_rdata.st_data.u8_enddata[0] == 0XAC && un_prodinfo_rdata.st_data.u8_enddata[1] == 0XCC )      
	{		
		st_product_info.u16_product_num = un_prodinfo_rdata.st_data.u16_product_num;
		st_product_info.u8_bat_num = un_prodinfo_rdata.st_data.u8_bat_num ;
        st_product_info.u8_car_num = un_prodinfo_rdata.st_data.u8_car_num;

		st_bat_data.fl_bat_soc = (float)un_prodinfo_rdata.st_data.u16_bat_soc * 0.1f;       //存储的数据是实际值10倍
		st_bat_data.fl_bat_soh = (float)un_prodinfo_rdata.st_data.u16_bat_soh * 0.1f;       //存储的数据是实际值10倍
		
		st_bat_data.fl_bat_Qc_max = (float)un_prodinfo_rdata.st_data.u16_bat_Qc_max * 0.1f ; //存储的数据是实际值10倍
		st_bat_data.fl_bat_Qnow = un_prodinfo_rdata.st_data.u32_bat_Qnow *0.0001f;            //存储的数据是实际值10000倍			
	
	}	
		
}
/*********************************************************************************************************
** 函数名称：故障存储数据更新
** 函数描述：
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
static void fmod_prodinfo_store_updata(void)
{
	uint32_t real_time = 0;
	
	un_prodinfo_wdata.st_data.u8_headdata[0] = 0XAA;
	un_prodinfo_wdata.st_data.u8_headdata[1] = PRODINFO_DATA_LEN;
	
	un_prodinfo_wdata.st_data.u16_product_num = st_product_info.u16_product_num;
	un_prodinfo_wdata.st_data.u8_bat_num = st_product_info.u8_bat_num;
    un_prodinfo_wdata.st_data.u8_car_num = st_product_info.u8_car_num;

    un_prodinfo_wdata.st_data.u16_bat_soc  = (uint16_t)(st_bat_data.fl_bat_soc * 10);          //SOC的10倍存储
	un_prodinfo_wdata.st_data.u16_bat_soh  = (uint16_t)(st_bat_data.fl_bat_soh * 10);
	un_prodinfo_wdata.st_data.u16_bat_Qc_max = (uint16_t)(st_bat_data.fl_bat_Qc_max * 10);
	un_prodinfo_wdata.st_data.u32_bat_Qnow = (uint32_t)(st_bat_data.fl_bat_Qnow * 10000);
	

	//..................读取当前时间...........................
	get_rtc_second(&real_time);
	un_prodinfo_wdata.st_data.u32_time = real_time;	 //1970年至今的秒数

	un_prodinfo_wdata.st_data.u8_resv[0] = 0x00;
	un_prodinfo_wdata.st_data.u8_resv[1] = 0x00;

	un_prodinfo_wdata.st_data.u8_enddata[0] = 0XAC;
	un_prodinfo_wdata.st_data.u8_enddata[1] = 0XCC;
}



/*********************************************************************************************************
** 函数名称：存储数据
** 函数描述：故障检测
** 输入参数：
** 返回值  ：无
*********************************************************************************************************///
static void fmod_prodinfo_store_data(void)
{	
	basic_write_filedata( PRODINFO_STORE_PATH, un_prodinfo_wdata.u8_data, PRODINFO_DATA_LEN, u32_prodinfo_times * PRODINFO_DATA_LEN);        //把当前的数据次数放与开始位置
		
}

/********************************************************************************************
    end file
********************************************************************************************/
