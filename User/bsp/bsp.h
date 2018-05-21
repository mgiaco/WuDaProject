/*
*********************************************************************************************************
*
*	ģ������ : BSPģ��
*	�ļ����� : bsp.h
*	˵    �� : ���ǵײ�����ģ�����е�h�ļ��Ļ����ļ��� Ӧ�ó���ֻ�� #include bsp.h ���ɣ�
*			  ����Ҫ#include ÿ��ģ��� h �ļ�
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_H_
#define _BSP_H_

#define LORA_ID 1        //  �豸ID�ţ�Ҳ����lora�ĵ�ַ

#define VERSION 20180521 // �汾��

#define  USE_RTX    1
/* ����ȫ���жϵĺ� */
#define ENABLE_INT()	__set_PRIMASK(0)	/* ʹ��ȫ���ж� */
#define DISABLE_INT()	__set_PRIMASK(1)	/* ��ֹȫ���ж� */


#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include  <stdarg.h>
#include  <math.h>
#include "RTL.h"//OS

#ifndef TRUE
	#define TRUE  1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

#define DATA_LEN 128


/* ͨ��ȡ��ע�ͻ������ע�͵ķ�ʽ�����Ƿ�����ײ�����ģ�� */
#include "main.h"

//#include "bsp_uart.h"
#include "bsp_timer.h"
#include "bsp_cpu_flash.h"

//#include "param.h"

#include "bsp_i2c_gpio.h"
#include "bsp_eeprom_24xx.h"

#include "bsp_spi_bus.h"
#include "bsp_spi_flash.h"

//#include "Wiegand.h"
//#include "protocal.h"

#include "bsp_dwt.h"
#include "bsp_iwdg.h"

/* �ṩ������C�ļ����õĺ��� */
void bsp_Init(void);
void bsp_Idle(void);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
