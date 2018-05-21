/*
*********************************************************************************************************
*
*	ģ������ : ����EEPROM 24xx02����ģ��
*	�ļ����� : bsp_eeprom_24xx.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2012-10-12 armfly  ST�̼���汾 V2.1.0
*
*	Copyright (C), 2012-2013, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_EEPROM_24XX_H
#define _BSP_EEPROM_24XX_H

//#define AT24C02
//#define AT24C128
//#define AT24CM01
#define AT24C64

#ifdef AT24C02
	#define EE_MODEL_NAME		"AT24C02"
	#define EE_DEV_ADDR			0xA0		/* �豸��ַ */
	#define EE_PAGE_SIZE		8			/* ҳ���С(�ֽ�) */
	#define EE_SIZE				256			/* ������(�ֽ�) */
	#define EE_ADDR_BYTES		1			/* ��ַ�ֽڸ��� */
#endif

#ifdef AT24C128
	#define EE_MODEL_NAME		"AT24C128"
	#define EE_DEV_ADDR			0xA0		/* �豸��ַ */
	#define EE_PAGE_SIZE		64			/* ҳ���С(�ֽ�) */
	#define EE_SIZE				(64*256)	/* ������(�ֽ�) */
	#define EE_ADDR_BYTES		2			/* ��ַ�ֽڸ��� */
#endif

#ifdef AT24CM01
	#define EE_MODEL_NAME		"AT24CM01"
	#define EE_DEV_ADDR			0xA0		/* �豸��ַ */
	#define EE_PAGE_SIZE		256			/* ҳ���С(�ֽ�) */
	#define EE_SIZE				(512*256)	/* ������(�ֽ�) */
	#define EE_ADDR_BYTES		3			/* ��ַ�ֽڸ��� ͨ��p0��������һ���ֽ�*/
#endif

#ifdef AT24C64
	#define EE_MODEL_NAME		"AT24C64"
	#define EE_DEV_ADDR			0xA0		/* �豸��ַ */
	#define EE_PAGE_SIZE		32			/* ҳ���С(�ֽ�) */
	#define EE_SIZE				(32*256)	/* ������(�ֽ�) 8K�ֽ�*/
	#define EE_ADDR_BYTES		2			/* ��ַ�ֽڸ��� */
#endif

uint8_t ee_CheckOk(void);
uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint32_t _usAddress, uint16_t _usSize);
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint32_t _usAddress, uint16_t _usSize);
uint8_t ee_WriteOneBytes(uint8_t DataWrite, uint32_t usAddr);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
