/*
*********************************************************************************************************
*
*	模块名称 : SPI接口串行FLASH 读写模块
*	文件名称 : bsp_spi_flash.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_SPI_FLASH_H
#define _BSP_SPI_FLASH_H

#define SF_MAX_PAGE_SIZE	(4 * 1024)

/* 定义串行Flash ID */
enum
{
	SST25VF016B_ID = 0xBF2541,
	MX25L1606E_ID  = 0xC22015,
	W25Q64_ID    = 0xEF4017,
	W25Q128_ID   = 0xEF4018
};

//结构体待修改，page大小为256字节，sector大小为4K字节
typedef struct
{
	uint32_t ChipID;		/* 芯片ID */
	char ChipName[16];		/* 芯片型号字符串，主要用于显示 */
	uint32_t TotalSize;		/* 总容量 */
	uint16_t PageSize;		/* 页面大小 */
}SFLASH_T;


//menjin need
extern SFLASH_T g_tSF;
void bsp_InitSFlash(void);
void sf_ReadBuffer(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize);
void sf_EraseSector(uint32_t _uiSectorAddr);
void sf_exWrite8Bytes(uint8_t* _pBuf, uint32_t _uiWriteAddr);//8字节
void sf_exWriteOnePage(uint8_t* _pBuf, uint32_t _uiWriteAddr);//256字节
void sf_exWrite_128(uint8_t* _pBuf, uint32_t _uiWriteAddr);//128字节
void sf_exWrite_64(uint8_t* _pBuf, uint32_t _uiWriteAddr);//128字节
void sf_PageWrite(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize);
void storeRecord(uint8_t *id, uint8_t reader);//函数内部自己获取时间和写入地址

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
