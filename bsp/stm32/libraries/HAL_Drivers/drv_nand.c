/*
 * Change Logs:
 * Date           Author           Notes
 * 2019-09-15     guolz      first implementation
 */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <rtdevice.h>
#include "board.h"
#include "drv_nand.h"
#include "File_Config.h"
#include <string.h>

//#pragma diag_suppress 69
#pragma diag_suppress 177

#define NAND_DEBUG    rt_kprintf

#define DMA_CHANNEL     DMA_Channel_0
#define DMA_STREAM      DMA2_Stream0
#define DMA_TCIF        DMA_FLAG_TCIF0
#define DMA_IRQN        DMA2_Stream0_IRQn
#define DMA_IRQ_HANDLER DMA2_Stream0_IRQHandler
#define DMA_CLK         RCC_AHB1Periph_DMA2

#define NAND_BANK     ((rt_uint32_t)0x70000000)
#define ECC_SIZE     4

NAND_HandleTypeDef NAND_Handler;    //NAND FLASH���

#define Bank2_NAND_ADDR    ((uint32_t)0x70000000)
#define Bank_NAND_ADDR     Bank2_NAND_ADDR

#define NAND_CMD_AREA		*(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA)
#define NAND_ADDR_AREA		*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA)
#define NAND_DATA_AREA		*(__IO uint8_t *)(Bank_NAND_ADDR | DATA_AREA)

static const char * ReVal_Table[]= 
{
	"0:Success",			                        
	"1:IO error, I/O driver initialization failed, or no storage device, or device initialize failed",
	"2:Volume error, Mount failed, which means invalid MBR, boot record or non fat format for fat file system",
	"3:Fat log initialize failed, fat initialize succeeded, but log initialize failed",
};


static rt_err_t nandflash_readid(struct rt_mtd_nand_device *mtd)
{
    NAND_IDTypeDef nand_id;

    HAL_NAND_Read_ID(&NAND_Handler,&nand_id);

    NAND_DEBUG("ID[%X,%X,%X,%X]\n",nand_id.Maker_Id,nand_id.Device_Id,nand_id.Third_Id,nand_id.Fourth_Id);
    if (nand_id.Maker_Id == 0xEF && nand_id.Device_Id == 0xDA)
    {
        return (RT_EOK);
    }

    return (RT_ERROR);
}

static rt_err_t nandflash_readpage(struct rt_mtd_nand_device* device, rt_off_t page,
                                   rt_uint8_t *data, rt_uint32_t data_len,
                                   rt_uint8_t *spare, rt_uint32_t spare_len)
{
     return (RT_MTD_EOK);
}

static rt_err_t nandflash_writepage(struct rt_mtd_nand_device* device, rt_off_t page,
                                    const rt_uint8_t *data, rt_uint32_t data_len,
                                    const rt_uint8_t *spare, rt_uint32_t spare_len)
{
     return (RT_MTD_EOK);
}

rt_err_t nandflash_eraseblock(struct rt_mtd_nand_device* device, rt_uint32_t block)
{
     return (RT_MTD_EOK);
}

static rt_err_t nandflash_pagecopy(struct rt_mtd_nand_device *device, rt_off_t src_page, rt_off_t dst_page)
{
     return (RT_MTD_EOK);
}

static rt_err_t nandflash_checkblock(struct rt_mtd_nand_device* device, rt_uint32_t block)
{
    return (RT_MTD_EOK);
}

static rt_err_t nandflash_markbad(struct rt_mtd_nand_device* device, rt_uint32_t block)
{
    return (RT_MTD_EOK);
}

static struct rt_mtd_nand_driver_ops ops =
{
    nandflash_readid,
    nandflash_readpage,
    nandflash_writepage,
    nandflash_pagecopy,
    nandflash_eraseblock,
#if defined(RT_USING_DFS_UFFS) && !defined(RT_UFFS_USE_CHECK_MARK_FUNCITON)
    RT_NULL,
    RT_NULL,
#else
    nandflash_checkblock,
    nandflash_markbad
#endif
};



static rt_uint8_t FSMC_NAND_ReadStatus(void)
{
	rt_uint8_t ucData;
	rt_uint8_t ucStatus = NAND_BUSY;

	NAND_CMD_AREA = NAND_CMD_STATUS;
	ucData = *(__IO rt_uint8_t *)(Bank_NAND_ADDR);

	if((ucData & NAND_ERROR) == NAND_ERROR)
	{
		ucStatus = NAND_ERROR;
	}
	else if((ucData & NAND_READY) == NAND_READY)
	{
		ucStatus = NAND_READY;
	}
	else
	{
		ucStatus = NAND_BUSY;
	}

	return (ucStatus);
}

static rt_uint8_t FSMC_NAND_GetStatus(void)
{
	rt_uint32_t ulTimeout = 0x10000;
	rt_uint8_t ucStatus = NAND_READY;

	ucStatus = FSMC_NAND_ReadStatus();

	while ((ucStatus != NAND_READY) &&( ulTimeout != 0x00))
	{
		ucStatus = FSMC_NAND_ReadStatus();
		if(ucStatus == NAND_ERROR)
		{
			return (ucStatus);
		}
		ulTimeout--;
	}

	if(ulTimeout == 0x00)
	{
		ucStatus =  NAND_TIMEOUT_ERROR;
	}

	return (ucStatus);
}



//��ȡNAND FLASH��ID
//��ͬ��NAND���в�ͬ��������Լ���ʹ�õ�NAND FALSH�����ֲ�����д����
//����ֵ:NAND FLASH��IDֵ
static rt_uint32_t NAND_ReadID(void)
{
    NAND_IDTypeDef nand_id;

    HAL_NAND_Read_ID(&NAND_Handler,&nand_id);

    NAND_DEBUG("ID[%X,%X,%X,%X]\n",nand_id.Maker_Id,nand_id.Device_Id,nand_id.Third_Id,nand_id.Fourth_Id);

    return 0;
}  

//��λNAND
//����ֵ:0,�ɹ�;
//    ����,ʧ��
static rt_uint8_t NAND_Reset(void)
{ 
    NAND_CMD_AREA = NAND_RESET;	//��λNAND
    if(FSMC_NAND_GetStatus()==NAND_READY)
        return 0;           //��λ�ɹ�
    else 
        return 1;			//��λʧ��
} 


void rt_hw_mtd_nand_deinit(void)
{
    HAL_NAND_DeInit(&NAND_Handler);
}


//��ʼ��NAND FLASH
rt_uint8_t rt_hw_mtd_nand_init(void)
{
    if(&NAND_Handler != NULL){
        rt_hw_mtd_nand_deinit();
    }
    FMC_NAND_PCC_TimingTypeDef ComSpaceTiming,AttSpaceTiming;
                                              
    NAND_Handler.Instance               = FMC_NAND_DEVICE;
    NAND_Handler.Init.NandBank          = FSMC_NAND_BANK2;                             //NAND����BANK2��
    NAND_Handler.Init.Waitfeature       = FSMC_NAND_PCC_WAIT_FEATURE_DISABLE;           //�رյȴ�����
    NAND_Handler.Init.MemoryDataWidth   = FSMC_NAND_PCC_MEM_BUS_WIDTH_8;                //8λ���ݿ��
    NAND_Handler.Init.EccComputation    = FSMC_NAND_ECC_DISABLE;                        //��ʹ��ECC
    NAND_Handler.Init.ECCPageSize       = FSMC_NAND_ECC_PAGE_SIZE_2048BYTE;             //ECCҳ��СΪ2k
    NAND_Handler.Init.TCLRSetupTime     = 1;                                            //����TCLR(tCLR=CLE��RE����ʱ)=(TCLR+TSET+2)*THCLK,THCLK=1/180M=5.5ns
    NAND_Handler.Init.TARSetupTime      = 1;                                            //����TAR(tAR=ALE��RE����ʱ)=(TAR+TSET+2)*THCLK,THCLK=1/180M=5.5n��   

    ComSpaceTiming.SetupTime        = 2;        //����ʱ��
    ComSpaceTiming.WaitSetupTime    = 5;        //�ȴ�ʱ��
    ComSpaceTiming.HoldSetupTime    = 3;        //����ʱ��
    ComSpaceTiming.HiZSetupTime     = 1;        //����̬ʱ��
    
    AttSpaceTiming.SetupTime        = 2;        //����ʱ��
    AttSpaceTiming.WaitSetupTime    = 5;        //�ȴ�ʱ��
    AttSpaceTiming.HoldSetupTime    = 3;        //����ʱ��
    AttSpaceTiming.HiZSetupTime     = 1;        //����̬ʱ��
    
    HAL_NAND_Init(&NAND_Handler,&ComSpaceTiming,&AttSpaceTiming); 
    NAND_Reset();       		        //��λNAND
    rt_thread_mdelay(100);

    return 0;
}

rt_uint8_t FSMC_NAND_ReadPage(rt_uint8_t *_pBuffer, rt_uint32_t _ulPageNo, rt_uint16_t _usAddrInPage, rt_uint16_t NumByteToRead)
{
	rt_uint32_t i;

    NAND_CMD_AREA = NAND_AREA_A;
    //���͵�ַ
    NAND_ADDR_AREA = _usAddrInPage;
	NAND_ADDR_AREA = _usAddrInPage >> 8;
	NAND_ADDR_AREA = _ulPageNo;
	NAND_ADDR_AREA = (_ulPageNo & 0xFF00) >> 8;
    NAND_ADDR_AREA = (_ulPageNo & 0xFF0000) >> 16;

    NAND_CMD_AREA = NAND_AREA_TRUE1;

	 /* ����ȴ���������������쳣, �˴�Ӧ���жϳ�ʱ */
	for (i = 0; i < 20; i++);
    while(rt_pin_read(NAND_RB)==0);

	/* �����ݵ�������pBuffer */
	for(i = 0; i < NumByteToRead; i++)
	{
		_pBuffer[i] = NAND_DATA_AREA;
	}

	return RT_EOK;
}


rt_uint8_t FSMC_NAND_WritePage(rt_uint8_t *_pBuffer, rt_uint32_t _ulPageNo, rt_uint16_t _usAddrInPage, rt_uint16_t NumByteToRead)
{
	rt_uint32_t i;
    rt_uint8_t ucStatus;

	NAND_CMD_AREA = NAND_WRITE0;
  //���͵�ַ
 	NAND_ADDR_AREA = _usAddrInPage;
	NAND_ADDR_AREA = _usAddrInPage >> 8;
	NAND_ADDR_AREA = _ulPageNo;
	NAND_ADDR_AREA = (_ulPageNo & 0xFF00) >> 8;
    NAND_ADDR_AREA = (_ulPageNo & 0xFF0000) >> 16;
	for (i = 0; i < 20; i++);

	for(i = 0; i < NumByteToRead; i++)
	{
        NAND_DATA_AREA = _pBuffer[i];
	}

	NAND_CMD_AREA = NAND_WRITE_TURE1; 

	for (i = 0; i < 20; i++);
	
  	ucStatus = FSMC_NAND_GetStatus();
	if(ucStatus == NAND_READY)   
	{
		ucStatus = RTV_NOERR;
	}
	else if(ucStatus == NAND_ERROR)
	{
		ucStatus = ERR_NAND_PROG;		
	}
	else if(ucStatus == NAND_TIMEOUT_ERROR)
	{
		ucStatus = ERR_NAND_HW_TOUT;		
	}
	
	return (ucStatus);
}

//����һ����
//BlockNum:Ҫ������BLOCK���,��Χ:0-(block_totalnum-1)
//����ֵ:0,�����ɹ�
//    ����,����ʧ��
rt_uint8_t NAND_EraseBlock(rt_uint32_t _ulBlockNo)
{
    rt_uint8_t ucStatus;
	
	NAND_CMD_AREA = NAND_ERASE0;

	_ulBlockNo <<= 6;	

    NAND_ADDR_AREA = _ulBlockNo;
    NAND_ADDR_AREA = _ulBlockNo >> 8;
    NAND_ADDR_AREA = _ulBlockNo >> 16;

	NAND_CMD_AREA = NAND_ERASE1;

	ucStatus = FSMC_NAND_GetStatus();
	if(ucStatus == NAND_READY)   
	{
		ucStatus = RTV_NOERR;
	}
	else if(ucStatus == NAND_ERROR)
	{
		ucStatus = ERR_NAND_PROG;		
	}
	else if(ucStatus == NAND_TIMEOUT_ERROR)
	{
		ucStatus = ERR_NAND_HW_TOUT;		
	}
	
	return (ucStatus);
} 

//ȫƬ����NAND FLASH
void NAND_EraseChip(void)
{
    rt_uint8_t status;
    rt_uint16_t i=0;
    for(i=0;i<2048;i++)     //ѭ���������еĿ�
    {
        status=NAND_EraseBlock(i);
        if(status)
            NAND_DEBUG("Erase %d block fail!!��ERRORCODE %d\r\n",i,status);//����ʧ��
    }
}


 void DotFormat(uint64_t _ullVal, char *_sp) 
{
	/* ��ֵ���ڵ���10^9 */
	if (_ullVal >= (U64)1e9) 
	{
		_sp += sprintf (_sp, "%d.", (uint32_t)(_ullVal / (uint64_t)1e9));
		_ullVal %= (uint64_t)1e9;
		_sp += sprintf (_sp, "%03d.", (uint32_t)(_ullVal / (uint64_t)1e6));
		_ullVal %= (uint64_t)1e6;
		sprintf (_sp, "%03d.%03d", (uint32_t)(_ullVal / 1000), (uint32_t)(_ullVal % 1000));
		return;
	}
	
	/* ��ֵ���ڵ���10^6 */
	if (_ullVal >= (uint64_t)1e6) 
	{
		_sp += sprintf (_sp,"%d.", (uint32_t)(_ullVal / (uint64_t)1e6));
		_ullVal %= (uint64_t)1e6;
		sprintf (_sp,"%03d.%03d", (uint32_t)(_ullVal / 1000), (uint32_t)(_ullVal % 1000));
		return;
	}
	
	/* ��ֵ���ڵ���10^3 */
	if (_ullVal >= 1000) 
	{
		sprintf (_sp, "%d.%03d", (uint32_t)(_ullVal / 1000), (uint32_t)(_ullVal % 1000));
		return;
	}
	
	/* ������ֵ */
	sprintf (_sp,"%d",(U32)(_ullVal));
}

void ViewNandCapacity(void)
{
	rt_uint8_t result;
	Media_INFO info;
	rt_uint64_t ullNANDCapacity;
	FAT_VI *mc0;  
	rt_uint8_t buf[15];
	
	// rt_hw_mtd_nand_init();

	/* ����nandflash */
	result = finit("M0:");
	if(result != NULL){
		NAND_DEBUG("Failed to mount file system (%s)\r\n", ReVal_Table[result]);
		NAND_DEBUG("Mount failed, FAT32 format required\r\n");
        NAND_DEBUG("FAT32 format in progress....\r\n");
        if (fformat ("M0:") != 0){            
            NAND_DEBUG ("Format failed\r\n");
        }else{
            NAND_DEBUG ("Format success\r\n");
        }
	}else{
		// NAND_DEBUG("Mount success (%s)\r\n", ReVal_Table[result]);
	}
	NAND_DEBUG("------------------------------------------------------------------\r\n");
	/* ��ȡvolume label */
	if (fvol ("M0:", (char *)buf) == 0) 
	{
		if (buf[0]){
			NAND_DEBUG ("Volume label of NAND Flash id %s\r\n", buf);
		}else{
			NAND_DEBUG ("No volume label\r\n");
		}
	}else{
		NAND_DEBUG ("Volume access error\r\n");
	}

	/* ��ȡNAND Flashʣ������ */
	ullNANDCapacity = ffree("M0:");
	DotFormat(ullNANDCapacity, (char *)buf);
	NAND_DEBUG("Free space of NAND Flash is %10s Bytes\r\n", buf);
	
	/* ж��NAND Flash */
	result = funinit("M0:");
	if(result != NULL){
		NAND_DEBUG("uMount failed\r\n");
	}else{
		NAND_DEBUG("uMount success\r\n");
	}
	
	/* ��ȡ��Ӧ�洢�豸�ľ�� */
	mc0 = ioc_getcb("M0");          
   
	/* ��ʼ��FAT�ļ�ϵͳ��ʽ�Ĵ洢�豸 */
	if (ioc_init (mc0) == 0) 
	{
		/* ��ȡ�洢�豸��������Ϣ */
		ioc_read_info (&info, mc0);

		/* �ܵ������� * ������С��NAND Flash��������С��512�ֽ� */
		ullNANDCapacity = (uint64_t)info.block_cnt << 9;
		DotFormat(ullNANDCapacity, (char *)buf);
		NAND_DEBUG("Total space = %10s Bytes\r\nTotal Sectors = %d \r\n", buf, info.block_cnt);
	
		NAND_DEBUG("Read Sectors = %d Bytes\r\n", info.read_blen);
		NAND_DEBUG("Write Sectors = %d Bytes\r\n", info.write_blen);
	}else{
		NAND_DEBUG("ioc_init failed\r\n");
	}
	
	/* ж��NAND Flash */
	if(ioc_uninit (mc0) != NULL){
		NAND_DEBUG("uMount failed\r\n");		
	}else{
		// NAND_DEBUG("uMount success\r\n");	
	}
	NAND_DEBUG("------------------------------------------------------------------\r\n");
}


static void ViewRootDir(const char *path)
{
	char filepath[50];
	memset(filepath,0x0,50);
	rt_uint8_t result;
	FINFO info;
	rt_uint64_t ullNANDCapacity;
	rt_uint8_t buf[15];
	
    info.fileID = 0;                /* ÿ��ʹ��ffind����ǰ��info.fileID�����ʼ��Ϊ0 */

	/* ����NAND Flash */
	result = finit("N0:");
	if(result != NULL){
		/* �������ʧ�ܣ���ز�Ҫ�ٵ���FlashFS������API��������ֹ����Ӳ���쳣 */
		NAND_DEBUG("Failed to mount file system (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}else{
		// NAND_DEBUG("Mount success (%s)\r\n", ReVal_Table[result]);
	}
	NAND_DEBUG("------------------------------------------------------------------\r\n");
	NAND_DEBUG("Filename                  |  File size     | FileID  |    property   |   Date\r\n");
	
	/* 
	   ����Ŀ¼�µ������ļ��г�����
	   1. "*" ���� "*.*" ����ָ��·���µ������ļ�
	   2. "abc*"         ����ָ��·������abc��ͷ�������ļ�
	   3. "*.jpg"        ����ָ��·������.jpg��β�������ļ�
	   4. "abc*.jpg"     ����ָ��·������abc��ͷ��.jpg��β�������ļ�
	
	   ������ʵ��������Ŀ¼�������ļ�
	*/
	if(path != NULL){
		sprintf(filepath,"N0:\\%s*.*",path);
	}else{
		memcpy(filepath,"N0:*.*",sizeof("N0:*.*"));
	}
	// while(ffind ("N0:*.*", &info) == 0) 
	while(ffind (filepath, &info) == 0)   
	{ 
		/* �����ļ���ʾ��С��ʽ */
		DotFormat(info.size, (char *)buf);
		
		/* ��ӡ��Ŀ¼�µ������ļ� */
		NAND_DEBUG ("%-20s %12s bytes       %04d  ",
				info.name,
				buf,
				info.fileID);
		
		/* �ж����ļ�������Ŀ¼ */
		if (info.attrib & ATTR_DIRECTORY)
		{
			NAND_DEBUG("  (0x%02x) doc", info.attrib);
		}
		else
		{
			NAND_DEBUG("  (0x%02x) File", info.attrib);
		}
		
		/* ��ʾ�ļ����� */
		NAND_DEBUG ("     %04d.%02d.%02d  %02d:%02d\r\n",
                 info.time.year, info.time.mon, info.time.day,
               info.time.hr, info.time.min);
    }
	
	if (info.fileID == 0)  
	{
		NAND_DEBUG ("No file found in Nandflash\r\n");
	}
	
	/* ��ȡNAND Flashʣ������ */
	ullNANDCapacity = ffree("N0:");
	DotFormat(ullNANDCapacity, (char *)buf);
	NAND_DEBUG("Free space of NAND FLASH is %10s Bytes\r\n", buf);

access_fail:
	/* ж��NAND Flash */
	result = funinit("N0:");
	if(result != NULL){
		NAND_DEBUG("uMount failed\r\n");		
	}else{
		// NAND_DEBUG("uMount success\r\n");	
	}
	
	NAND_DEBUG("------------------------------------------------------------------\r\n");
}

static void EchotextFile(const char* txt,const char *file)
{
	char filepath[50];
	memset(filepath,0x0,50);
	const rt_uint8_t WriteText[] = {"Ԥ�����͹�״����\r\n2020-04-07\r\n2019-nCov"};
	FILE *fout;
	rt_uint32_t bw;
	rt_uint32_t i = 2;
	rt_uint8_t result;

	/* ����NAND Flash */
	result = finit("N0:");
	if(result != NULL){
		/* �������ʧ�ܣ���ز�Ҫ�ٵ���FlashFS������API��������ֹ����Ӳ���쳣 */
		NAND_DEBUG("Failed to mount file system (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}else{
		// NAND_DEBUG("Mount success (%s)\r\n", ReVal_Table[result]);
	}
	NAND_DEBUG("------------------------------------------------------------------\r\n");
	
	/**********************************************************************************************************/
	sprintf(filepath,"N0:\\%s",file);
	fout = fopen (filepath, "a+"); 
	if (fout != NULL) 
	{
		// NAND_DEBUG("open N0:\\glz\\test1.txt Sucess\r\n");
		/* д���� */
		bw = fwrite (txt, sizeof(rt_uint8_t), strlen(txt)/sizeof(rt_uint8_t), fout);
		if(bw == strlen(txt)/sizeof(rt_uint8_t))
		{
			NAND_DEBUG("write [%s] to %s done\r\n",txt,filepath);
		}
		else
		{ 
			NAND_DEBUG("write data error\r\n");
		}
		
		/* �ر��ļ� */
		fclose(fout);
	}
	else
	{
		NAND_DEBUG("open %s falied\r\n",file);
	}

access_fail:	
	/* ж��NAND Flash */
	result = funinit("N0:");
	if(result != NULL){
		NAND_DEBUG("uMount failed\r\n");		
	}else{
		// NAND_DEBUG("uMount success\r\n");	
	}

	NAND_DEBUG("------------------------------------------------------------------\r\n");
}

void Formatflash(void)
{
	rt_uint8_t result;

	/* ����NAND Flash */
	result = finit("N0:");
	if(result != NULL){
		/* �������ʧ�ܣ���ز�Ҫ�ٵ���FlashFS������API��������ֹ����Ӳ���쳣 */
		NAND_DEBUG("Failed to mount file system (%s)\r\n", ReVal_Table[result]);
		//goto access_fail;
	}else{
		// NAND_DEBUG("Mount success (%s)\r\n", ReVal_Table[result]);
	}
	NAND_DEBUG("------------------------------------------------------------------\r\n");
	
	if (fformat ("N0:") != 0){            
		NAND_DEBUG ("Format failed\r\n");
	}else{
		NAND_DEBUG ("Format flash success\r\n");
	}

	NAND_DEBUG("------------------------------------------------------------------\r\n");
access_fail:
	result = funinit("N0:");
	if(result != NULL){
		NAND_DEBUG("uMount failed\r\n");		
	}else{
		// NAND_DEBUG("uMount success\r\n");	
	}
}

static void CreateNewFile(const char *docname)
{
	char filepath[50];
	memset(filepath,0x0,50);
	FILE *fout;
	rt_uint8_t result;

	/* ����NAND Flash */
	result = finit("N0:");
	if(result != NULL){
		/* �������ʧ�ܣ���ز�Ҫ�ٵ���FlashFS������API��������ֹ����Ӳ���쳣 */
		NAND_DEBUG("Failed to mount file system (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}else{
		// NAND_DEBUG("Mount success (%s)\r\n", ReVal_Table[result]);
	}
	NAND_DEBUG("------------------------------------------------------------------\r\n");
	sprintf(filepath,"N0:\\%s\\",docname);
	/**********************************************************************************************************/
	fout = fopen (filepath, "w"); 
	if (fout != NULL) 
	{
		NAND_DEBUG("creat doc[%s] success\r\n", docname);
		/* �ر��ļ� */
		fclose(fout);
	}
	else
	{
		NAND_DEBUG("fail to creat doc[%s]\r\n", docname);
	}
	
access_fail:	
	/* ж��NAND Flash */
	result = funinit("N0:");
	if(result != NULL){
		NAND_DEBUG("uMount failed\r\n");		
	}else{
		// NAND_DEBUG("uMount success\r\n");	
	}

	NAND_DEBUG("------------------------------------------------------------------\r\n");
}


static void SeekFileData(void)
{
	const rt_uint8_t WriteText[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"};
	FILE *fin, *fout;
	rt_uint32_t bw;
	rt_uint32_t uiPos;
	rt_uint8_t ucChar;
	rt_uint8_t result;

	/* ����NAND Flash */
	result = finit("N0:");
	if(result != NULL){
		/* �������ʧ�ܣ���ز�Ҫ�ٵ���FlashFS������API��������ֹ����Ӳ���쳣 */
		NAND_DEBUG("Failed to mount file system (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}else{
		NAND_DEBUG("Mount success (%s)\r\n", ReVal_Table[result]);
	}
	NAND_DEBUG("------------------------------------------------------------------\r\n");
	
	/**********************************************************************************************************/
	/* ���ļ���test�е��ļ�test1.txt�����û�����ļ��к�txt�ļ����Զ�����*/
	fout = fopen ("N0:\\test.txt", "w"); 
	if (fout != NULL){
		NAND_DEBUG("open N0:\\test.txt success\r\n");
		/* д���� */
		bw = fwrite (WriteText, sizeof(rt_uint8_t), sizeof(WriteText)/sizeof(rt_uint8_t), fout);
		if(bw == sizeof(WriteText)/sizeof(rt_uint8_t)){
			NAND_DEBUG("write data ok\r\n");
		}else{ 
			NAND_DEBUG("write data error\r\n");
		}
			
		/* �ر��ļ� */
		fclose(fout);
	}else{
		NAND_DEBUG("open N0:\\test.txt fail\r\n");
	}
	
	/***********************************************/
	fin = fopen ("N0:\\test.txt","r");
	if (fin != NULL)  
	{
		NAND_DEBUG("\r\n open N0:\\test.txt success\r\n");
		
		/* ��ȡ�ļ�test.txt��λ��0���ַ� */
		int err = fseek (fin, 0L, SEEK_SET);
		uiPos = ftell(fin); 	
		ucChar = fgetc (fin);	
		NAND_DEBUG("err[%d] file test.txt current read position��%02d,byte��%c\r\n",err, uiPos, ucChar);
		
		/* ��ȡ�ļ�test.txt��λ��1���ַ� */
		fseek (fin, 1L, SEEK_SET);
		uiPos = ftell(fin); 	
		ucChar = fgetc (fin);		
		NAND_DEBUG("file test.txt current read position��%02d,byte��%c\r\n", uiPos, ucChar);

		/* ��ȡ�ļ�test.txt��λ��25���ַ� */
		fseek (fin, 25L, SEEK_SET);
		uiPos = ftell(fin); 	
		ucChar = fgetc (fin);		
		NAND_DEBUG("file test.txt current read position��%02d,byte��%c\r\n", uiPos, ucChar);
		
		/* ͨ�����溯���Ĳ�������ǰ��дλ����26
		   ���溯�����ڵ�ǰλ�õĻ����Ϻ���2��λ�ã�Ҳ����24�����ú���fgetc��λ�þ���25
		 */
		fseek (fin, -2L, SEEK_CUR);
		uiPos = ftell(fin); 	
		ucChar = fgetc (fin);		
		NAND_DEBUG("file test.txt current read position��%02d,byte��%c\r\n", uiPos, ucChar);
		
		/* ��ȡ�ļ�test.txt�ĵ�����2���ַ�, ���һ����'\0' */
		fseek (fin, -2L, SEEK_END); 
		uiPos = ftell(fin); 
		ucChar = fgetc (fin);
		NAND_DEBUG("file test.txt current read position��%02d,byte��%c\r\n", uiPos, ucChar);
		
		/* ����ȡλ�����¶�λ���ļ���ͷ */
		rewind(fin);  
		uiPos = ftell(fin); 
		ucChar = fgetc (fin);
		NAND_DEBUG("file test.txt current read position��%02d,byte��%c\r\n", uiPos, ucChar);	
		
		/* 
		   ��������ʾһ��ungetc�����ã��˺������ǽ���ǰ�Ķ�ȡλ��ƫ�ƻ�һ���ַ���
		   ��fgetc���ú�λ������һ���ַ���
		 */
		fseek (fin, 0L, SEEK_SET);
		ucChar = fgetc (fin);
		uiPos = ftell(fin); 
		NAND_DEBUG("file test.txt current read position��%02d,byte��%c\r\n", uiPos, ucChar);
		ungetc(ucChar, fin); 
		uiPos = ftell(fin); 
		NAND_DEBUG("file test.txt current read position��%02d,byte��%c\r\n", uiPos, ucChar);
		
		/* �ر�*/
		fclose (fin);
	}
	else
	{
		NAND_DEBUG("open N0:\\test.txt fail\r\n");
	}
	
	/***********************************************/
	fin = fopen ("N0:\\test.txt","r+");
	if (fin != NULL)  
	{
		NAND_DEBUG("\r\n open N0:\\test.txt success\r\n");
		
		/* �ļ�test.txt��λ��2�������ַ� '��' */
		fseek (fin, 2L, SEEK_SET);
		// ucChar = fputc ('!', fin);
		fwrite ("!", sizeof(rt_uint8_t), sizeof("!")/sizeof(rt_uint8_t), fin);
		/* ˢ�����ݵ��ļ��� */
		// fflush(fin);		

		fseek (fin, 2L, SEEK_SET);
		uiPos = ftell(fin); 
		ucChar = fgetc (fin);				
		NAND_DEBUG("file test.txt position��%02d,New characters have been inserted��%c\r\n", uiPos, ucChar);
	
		
		/* �ļ�test.txt�ĵ�����2���ַ�, �������ַ� ��&�� �����һ����'\0' */
		fflush(fin);	
		fseek (fin, -2L, SEEK_END); 
		// ucChar = fputc ('&', fin);
		fwrite ("&", sizeof(rt_uint8_t), sizeof("&")/sizeof(rt_uint8_t), fin);
		/* ˢ�����ݵ��ļ��� */
		fflush(fin);	

		fseek (fin, -2L, SEEK_END); 
		uiPos = ftell(fin); 
		ucChar = fgetc (fin);	
		NAND_DEBUG("file test.txt position��%02d,New characters have been inserted��%c\r\n", uiPos, ucChar);
		
		/* �ر�*/
		fclose (fin);
	}
	else
	{
		NAND_DEBUG("\r\n open N0:\\test.txt fail\r\n");
	}
	
access_fail:	
	/* ж��NAND Flash */
	result = funinit("N0:");
	if(result != NULL){
		NAND_DEBUG("uMount failed\r\n");		
	}else{
		NAND_DEBUG("uMount success\r\n");	
	}

	NAND_DEBUG("------------------------------------------------------------------\r\n");

}
FINSH_FUNCTION_EXPORT_ALIAS(SeekFileData, __cmd_seek, file seek);

static void ReadFileData(const char* file)
{
	rt_uint8_t Readbuf[50];
	char filepath[50];
	memset(filepath,0x0,50);
	FILE *fin;
	rt_uint32_t bw;
	rt_uint8_t result;

	/* ����NAND Flash */
	result = finit("N0:");
	if(result != NULL){
		/* �������ʧ�ܣ���ز�Ҫ�ٵ���FlashFS������API��������ֹ����Ӳ���쳣 */
		NAND_DEBUG("Failed to mount file system (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}else{
		// NAND_DEBUG("Mount success (%s)\r\n", ReVal_Table[result]);
	}
	NAND_DEBUG("------------------------------------------------------------------\r\n");
	
	/**********************************************************************************************************/
	sprintf(filepath,"N0:\\%s",file);
	fin = fopen (filepath, "r"); 
	if (fin != NULL) 
	{
		// NAND_DEBUG("<1>open %s success\r\n",filepath);
		
		/* ��ֹ���� */
		(void) NULL;
		
		/* ������ */
		bw = fread(Readbuf, sizeof(rt_uint8_t), 50/sizeof(rt_uint8_t), fin);
		if(bw > 0)
		{
			Readbuf[bw] = NULL;
			NAND_DEBUG("%s\r\n", Readbuf);
		}
		else
		{ 
			NAND_DEBUG("read failed\r\n");
		}
		
		/* �ر��ļ� */
		fclose(fin);
	}
	else
	{
		NAND_DEBUG("<1>open %s failed and Maybe the file doesn't exist\r\n",filepath);
	}
	
access_fail:
	/* ж��NAND Flash */
	result = funinit("N0:");
	if(result != NULL){
		NAND_DEBUG("uMount failed\r\n");		
	}else{
		// NAND_DEBUG("uMount success\r\n");	
	}

	NAND_DEBUG("------------------------------------------------------------------\r\n");
}

static void DeleteDirFile(const char* file)
{
	char filepath[50];
	memset(filepath,0x0,50);
	rt_uint8_t result;
	
	/* ����NAND Flash */
	result = finit("N0:");
	if(result != NULL){
		/* �������ʧ�ܣ���ز�Ҫ�ٵ���FlashFS������API��������ֹ����Ӳ���쳣 */
		NAND_DEBUG("Failed to mount file system (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}else{
		// NAND_DEBUG("Mount success (%s)\r\n", ReVal_Table[result]);
	}
	NAND_DEBUG("------------------------------------------------------------------\r\n");
	
	/* ɾ���ļ� speed1.txt */
	sprintf(filepath, "N0:\\%s", file);		/* ÿд1�Σ���ŵ��� */
	result = fdelete (filepath);
	if (result != NULL) 
	{
		printf("[%s] is not exist��return��%d��\r\n", filepath, result);
	}
	else
	{
		printf("delete [%s] done\r\n", filepath);
	}
	
access_fail:
	/* ж��NAND Flash */
	result = funinit("N0:");
	if(result != NULL){
		NAND_DEBUG("uMount failed\r\n");		
	}else{
		// NAND_DEBUG("uMount success\r\n");	
	}

	NAND_DEBUG("------------------------------------------------------------------\r\n");
}

static void glz_nand(int argc, char **argv) 
{
    /* If the number of arguments less than 2 */
    if (argc < 2)
    {
help:
        rt_kprintf("\n");
        rt_kprintf("glz_nand [OPTION] [PARAM ...]\n");
        rt_kprintf("         ls                           ��ʾָ������Ŀ¼��֮����\n");
        rt_kprintf("         cat        <filename>        ��ʾ�ļ�����\n");
        rt_kprintf("         mkdir      <docname>         �����ļ���\n");
        rt_kprintf("         rm         <filename>        ɾ���ļ�\n");
        rt_kprintf("         formatall                    ���̸�ʽ��\n");
		rt_kprintf("         df                           ��ʾ���̿ռ�\n");
		rt_kprintf("         echo <text> > <filename>     ��ʾ���̿ռ�\n");
        return ;
    }
    else if (!strcmp(argv[1], "ls"))
    {
		if(argv[2]  != NULL){
			ViewRootDir(argv[2]);
		}else{
			ViewRootDir(NULL);
		}
    }
    else if (!strcmp(argv[1], "cat"))
    {
        if (argc < 3)
        {
            rt_kprintf("The input parameters are too few!\n");
            goto help;
        }
		ReadFileData(argv[2]);
    }
    else if (!strcmp(argv[1], "echo"))
    {
        if (argc < 4)
        {
            rt_kprintf("The input parameters are too few!\n");
            goto help;
        }
		if(!strcmp(argv[3], ">")){
			EchotextFile(argv[2],argv[4]);
		}else{
			rt_kprintf("bad parameters\n");
		}
    }
    else if (!strcmp(argv[1], "formatall"))
    {
		Formatflash();
    }
	else if (!strcmp(argv[1], "df"))
    {
		ViewNandCapacity();
    }
	else if (!strcmp(argv[1], "df"))
    {
		ViewNandCapacity();
    }
	else if (!strcmp(argv[1], "mkdir"))
    {
		if (argc < 2)
        {
            rt_kprintf("The input parameters are too few!\n");
            goto help;
        }
		CreateNewFile(argv[2]);
    }
	else if (!strcmp(argv[1], "rm"))
    {
		if (argc < 2)
        {
            rt_kprintf("The input parameters are too few!\n");
            goto help;
        }
		DeleteDirFile(argv[2]);
    }
    else
    {
        rt_kprintf("Input parameters are not supported!\n");
        goto help;
    }
}
MSH_CMD_EXPORT(glz_nand, GLZ nand RL-FLASHFS test function);
