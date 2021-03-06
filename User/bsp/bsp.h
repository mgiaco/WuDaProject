/*
*********************************************************************************************************
*
*	模块名称 : BSP模块
*	文件名称 : bsp.h
*	说    明 : 这是底层驱动模块所有的h文件的汇总文件。 应用程序只需 #include bsp.h 即可，
*			  不需要#include 每个模块的 h 文件
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_H_
#define _BSP_H_

#define  USE_RTX    1
/* 开关全局中断的宏 */
#define ENABLE_INT()	__set_PRIMASK(0)	/* 使能全局中断 */
#define DISABLE_INT()	__set_PRIMASK(1)	/* 禁止全局中断 */

#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include  <stdarg.h>
#include  <math.h>
#include "RTL.h"//OS


/* 通过取消注释或者添加注释的方式控制是否包含底层驱动模块 */
#include "main.h"

#include "uart.h"
#include "bsp_timer.h"
#include "bsp_cpu_flash.h"
#include "bsp_i2c_gpio.h"
#include "bsp_eeprom_24xx.h"

#include "bsp_spi_bus.h"
#include "bsp_spi_flash.h"


#include "CRC8_16.h"
#include "param.h"


#include "Reader.h"
#include "protocal.h"

#include "bsp_dwt.h"
#include "bsp_iwdg.h"
#include "adc.h"

/* 提供给其他C文件调用的函数 */
void bsp_Init(void);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
