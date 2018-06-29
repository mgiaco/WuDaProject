#ifndef _MAIN_H_
#define _MAIN_H_

//运行时的状态
typedef struct
{
    uint8_t isTriggered;//探测器是否触发
    uint8_t readTimes;//读取能级次数
}RunInfo_T;

//事件标志位宏定义
#define BIT_ALL 0xFF
#define LORA_RECV_BIT	(1<<0)
#define TASK_NET_BIT	(1<<1)
#define REAET_LEVEL_BIT	(1<<2)

extern OS_TID HandleTaskNet;
extern RunInfo_T g_tRunInfo;
#endif

/******************************************/
