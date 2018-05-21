/*
*********************************************************************************************************
*
*	模块名称 : LED指示灯驱动模块
*	文件名称 : bsp_led.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "param.h"  

//单个门
typedef struct
{
    uint8_t switcherStatus;//继电器状态
    uint8_t feedBackStatus;//门吸反馈状态
}SingleDoor_T;   

//所有门
typedef struct
{
    SingleDoor_T doorA;//读头A
    SingleDoor_T doorB;//读头B
    void (*readFeedBack)(void);//读取门吸反馈，结果存入结构体
    void (*openDoor)(SingleRelation_T *singleRelation, enum ReaderOrButton_Enum openWay);//传进来的是读头A/按键A或者B的对应关系
    void (*closeDoor)(SingleRelation_T *singleRelation, enum ReaderOrButton_Enum openWay);
}DoorStatus_T;


//关灯（高电平）表示继电器（NC常闭）关门，反之开门
/* 供外部调用的函数声明 */
extern DoorStatus_T g_tDoorStatus;

void bsp_InitLed(void);
void bsp_LedOn(uint8_t _no);
void bsp_LedOff(uint8_t _no);
void bsp_LedToggle(uint8_t _no);//结合毫秒延时可以用来使系统状态灯闪烁

void alarmOn(enum ReaderOrButton_Enum type);
void alarmOff(enum ReaderOrButton_Enum type);
#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
