/******************************************************************************************
** 头文件
*******************************************************************************************/
#include "basic_flash.h"

/********************************************************************************************
** 定义变量
********************************************************************************************/
#define NAND_DEBUG    rt_kprintf
extern void NAND_EraseChip(void);
extern void Formatflash(void);
static rt_uint8_t defaultDisk = Nand_Flash;
//static void Mount_Disk(rt_uint8_t disk_name);

uint8_t nandflash_mount_flag = 0;
uint8_t ftp_open_flag = 0;


static const char * ReVal_Table[]= 
	
{
	"0:Success",			                        
	"1:IO error, I/O driver initialization failed, or no storage device, or device initialize failed",
	"2:Volume error, Mount failed, which means invalid MBR, boot record or non fat format for fat file system",
	"3:Fat log initialize failed, fat initialize succeeded, but log initialize failed",
};
/********************************************************************************************
** 函数申明
********************************************************************************************/
extern void DotFormat(uint64_t _ullVal, char *_sp) ;
/********************************************************************************************
** 内容
********************************************************************************************/
//extern struct rt_mutex ftp_open_mutex;

/***********************************************************************************
** 函数名称：
** 函数描述：判断nand0 是否挂载成功
** 输入参数：
** 返回值	：无
************************************************************************************/
int basic_nandflash_mount_status(void)
{	
	rt_uint8_t result;
		
	result = finit("N0:");
	if(result != NULL)
	{
		//NAND_EraseChip( );
		Formatflash( );			
		
		
		NAND_DEBUG("Failed to mount file system (%s)\r\n", ReVal_Table[result]);
		NAND_DEBUG("nandflash mount failed!");
		nandflash_mount_flag = 0;
		result = funinit("N0:");
		if(result != NULL) NAND_DEBUG("uMount failed\r\n");
		else  NAND_DEBUG("uMount success\r\n");		
		
		return 0;
	}
	else
	{
		NAND_DEBUG("Mount success (%s)\r\n", ReVal_Table[result]);
		nandflash_mount_flag = 1;
		return 1;
	}
	
}INIT_APP_EXPORT(basic_nandflash_mount_status);

/***********************************************************************************
** 函数名称：
** 函数描述：读取文件中指定位置的数据
** 输入参数：*filepath文件的路径  u8_data：读取数据地址，len：读取数据的长度，offse：偏移位置
** 返回值  ：读取到的数据
************************************************************************************/

uint8_t *basic_read_filedata(char *pathname, uint8_t *u8_data, uint8_t len, uint32_t offest )	//	
{
	int  res = 0;
	char filepath[60]= {0};
	FILE *fd;
	rt_uint32_t bw;
	
	
	if(ftp_open_flag ==1)
	{
		return 0;
	}
	
	sprintf(filepath, "N0:\\%s", pathname);
	//rt_kprintf("%s\n", filepath);
    fd = fopen(filepath, "r");
	if(fd == NULL)  
	{   
 		fd = fopen(filepath, "w+"); //文件不存在则新建文件
	}
    
	if(fd == NULL) 
	{
		 rt_kprintf("open %s read error!\n", filepath);
		 return 0;
	}

	(void) NULL;
	res = fseek(fd, (long)offest, SEEK_SET);               //移动文件位置的偏移数，以字节为单位
	if( res != NULL) //返回0表示成功
	{
		rt_kprintf("%s seek failed!\n", filepath);
		fclose(fd);
		return 0;
	}
	/* 读数据 */
	memset(u8_data, 0x0, len); 
	bw = fread(u8_data, sizeof(rt_uint8_t), len, fd);  //sizeof(rt_uint8_t)：每一个数据的大小
	if(bw == 0)
	{ 
		//rt_kprintf("%s read failed!\n", filepath);
		fclose(fd);
		return 0;
	}
	fclose(fd);
	return u8_data;	
}
 
/********************************************************************************************
** 函数名称：	
** 函数描述：flash写数据接口
** 输入参数：pathname文件的路径 u8_data：读取数据地址，len：读取数据的长度，offse：偏移位置
** 返回值  ：无
********************************************************************************************/
rt_err_t basic_write_filedata(char *pathname, uint8_t *u8_data,  uint8_t len, uint32_t offest) 
{
	int  res = 0;
	char filepath[60] ={0};
	memset(filepath,0x0,60);
	FILE *fd;
	rt_uint32_t bw;
	

    if(ftp_open_flag ==1)
	{
		return 0;
	}

	sprintf(filepath, "N0:\\%s", pathname);
    fd = fopen(filepath, "r+"); //打开可读可写的文件
	if(fd == NULL)  
	{   
 		fd = fopen(filepath, "w+");  //文件不存在则新建文件
	}

	if(fd== NULL)
	{
		 rt_kprintf("open %s write error!\n", pathname);
		 return RT_ERROR;
	}	

	res = fseek(fd, (long)offest, SEEK_SET);               //移动文件位置的偏移数，以字节为单位
	if( res != NULL) //返回0表示成功
	{
		rt_kprintf("%s seek failed!\n", filepath);
		fclose(fd);
		return RT_ERROR;
	}
	/* 写数据 */
	bw = fwrite(u8_data, sizeof(rt_uint8_t), len, fd);  //sizeof(rt_uint8_t)：每一个数据的大小
	if(bw != len)
	{ 
		rt_kprintf("%s write failed!\n", filepath);
		fclose(fd);
		return RT_ERROR;
	}

	fflush(fd);
	fclose(fd);
	return RT_EOK;	
		
}

/******************************************************************************************
** 函数名称：
** 函数描述：在文件的后面追加数据
** 输入参数：*pathname文件路径名称，增加数据内容指针，len增加数据的长度，bsize增加数据类型的大小为多少字节
** 返回值	：无
*******************************************************************************************/
rt_err_t basic_add_filedata(char *pathname, char *ptr, uint32_t len, uint8_t bsize)
{
	FILE *fp;    //文件指针，fopen标准库文件
	char filepath[60] = {0};
  

	//memset(filepath, 0x0, 60);

    if(ftp_open_flag ==1)
	{
		return 0;
	}

	sprintf(filepath, "N0:\\%s", pathname);
	
	//........................存储数据...........................	
	fp = fopen(filepath, "a+");//追加的方式打开文件，写的内容放在文件结尾，文件不存在 件则建立文
	if(fp == NULL)    
	{
		rt_kprintf("fopen %s failed!\n",  pathname);		
		return RT_ERROR;
	}

	fwrite(ptr, len, bsize, fp); //写入数据 1表示1字节，
	fclose(fp);
	return RT_EOK;
}
/******************************************************************************************
** 函数名称：
** 函数描述：在文件的后面追加数据
** 输入参数：*pathname文件路径名称，增加数据内容指针，len增加数据的长度，bsize增加数据类型的大小为多少字节
** 返回值	：无
*******************************************************************************************/
rt_err_t basic_add_u8data(char *pathname, uint8_t *ptr, uint32_t len, uint8_t bsize)
{
	FILE *fp;    //文件指针，fopen标准库文件
	char filepath[60] = {0};
  

	//memset(filepath, 0x0, 60);

    if(ftp_open_flag ==1)
	{
		return 0;
	}

	sprintf(filepath, "N0:\\%s", pathname);
	//........................存储数据...........................	
    fp = fopen(filepath, "a+");	
	if(fp  == NULL)     //追加的方式打开文件，写的内容放在文件结尾，文件不存在则建立文件
	{
		rt_kprintf("fopen %s failed!\n",  pathname);
		return RT_ERROR;
	}

	fwrite(ptr, len, bsize, fp); //写入数据 1表示1字节，
	fclose(fp);
	return RT_EOK;
}

/******************************************************************************************
** 函数名称：
** 函数描述：获取文件大小
** 输入参数：*pathname文件路径名称，增加数据内容指针，len增加数据的长度，bsize增加数据类型的大小为多少字节
** 返回值	：无
*******************************************************************************************/
rt_err_t basic_get_filesize(const char *pathname, FINFO *info)
{
	char filepath[60];
	rt_uint8_t result;
	rt_uint8_t buf[15];
	
    info->fileID = 0;                /* 每次使用ffind函数前，info.fileID必须初始化为0 */

    if(ftp_open_flag ==1)
	{
		return 0;
	}
	sprintf(filepath, "N0:\\%s", pathname);	
	//rt_kprintf("%s\n", filepath );
	result = ffind(filepath, info);	
	if(result == 0)   
	{		
		/* 调整文件显示大小格式 */
		DotFormat(info->size, (char *)buf);
		//...............打印文件信息................
	   // NAND_DEBUG ("%-20s %12s bytes       %04d  ", info->name, buf, info->fileID);
		printf ("\n--%30s: %10s bytes, fileID: %04d  ", info->name, buf, info->fileID);
      
		/* 判断是文件还是子目录 */
		if (info->attrib & ATTR_DIRECTORY) //10为文件夹，20为文件
		{
			NAND_DEBUG("  doc:"); //文件夹
		}
		else
		{
			NAND_DEBUG("  file:");//文件
		}
		
		/* 显示文件日期 */
		NAND_DEBUG ("  --%04d.%02d.%02d  %02d:%02d\r\n",
               info->time.year, info->time.mon, info->time.day,
               info->time.hr, info->time.min);
	}
	
	if(info->fileID == 0)  
	{
		rt_kprintf ("%s no found in Nandflash!\r\n", filepath);
		return RT_ERROR;
	}
	return RT_EOK;
}
/******************************************************************************************
** 函数名称：
** 函数描述：TF卡和flash的挂载
** 输入参数：*pathname文件路径名称，增加数据内容指针，len增加数据的长度，bsize增加数据类型的大小为多少字节
** 返回值	：无
*******************************************************************************************/

//static void Mount_Disk(rt_uint8_t disk_name)
//{
//    defaultDisk = disk_name;
//    if(disk_name == tf_Card){
//        funinit("N0:");
//        finit("M0:");
//    }else{
//        funinit("M0:");
//        finit("N0:");
//    }
//}

rt_uint8_t Get_MountDisk(void)
{
    return defaultDisk;
}
