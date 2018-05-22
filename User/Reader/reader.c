#include "bsp.h"

#define  DWT_CYCCNT  *(volatile unsigned int *)0xE0001004
    
Reader_T g_tReader;
static uint32_t dwtTime;

/***********************************************************
* 函数名：ReaderInit
* 功能  ：外部中断引脚初始化 
* 输入  : 无
* 输出  ：无
**********************************************************/
void ReaderInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

    
	//信号线 PA0,1
	/* 使能 GPIO 时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//输入浮空模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* 使能 AFIO 时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	//读头A
    //Data0 中断线	PA0
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0); 
	EXTI_InitStructure.EXTI_Line	= EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode 	= EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 
	
	//中断优先级，尽可能设为最高优先级
	NVIC_InitStructure.NVIC_IRQChannel 	= EXTI0_IRQn;//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 	= 0x00;//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd 	= ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 
	
	//Data1 中断线  PA1
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);
	EXTI_InitStructure.EXTI_Line	= EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode 	= EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	
	
	//中断优先级，尽可能设为最高优先级
	NVIC_InitStructure.NVIC_IRQChannel 	= EXTI1_IRQn;	//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//抢占优先级 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 	= 0x00;	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd 	= ENABLE;	//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 
    
}

//读头A的data0//光电探测器中断
void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0) != RESET)
	{		
        g_tReader.tStop = DWT_CYCCNT;
        
        dwtTime = (g_tReader.tStop - g_tReader.tStart)/72;
          
        //memcpy(g_tReader.preciseTime, g_tGPS.time, sizeof(g_tGPS.time));     
        g_tReader.preciseTime[6] = dwtTime>>16;
        g_tReader.preciseTime[7] = (dwtTime>>8)&0xFF;
        g_tReader.preciseTime[8] = dwtTime&0xFF;
        
		EXTI_ClearITPendingBit(EXTI_Line0);	
	}
}

//读头A的data1//1pps中断,会每秒触发一次
void EXTI1_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line1) != RESET)
	{
        g_tReader.tStart = DWT_CYCCNT;
        g_tReader.tStop = 0;        
		
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}
