#ifndef __PARAM_H
#define __PARAM_H

#define PARAM_ADDR		0			/* 参数区地址 */
//默认参数
#define VERSION 20180521 // 版本号

#define CHANNEL 30 //lora的信道参数，410+30=440MHz(信道相同才能互相通信)
#define DTU_ADDRESS 0xFFFE //DTU地址

typedef struct
{
    uint8_t isUpdate;//0xFF表示启动到默认区，1表示需要升级，0表示启动到app区
    //lora模块和DTU的通信参数
    uint8_t loraAddress[2];
    uint8_t dtuAddress[2];
    uint8_t channel;
    
}Param_T;

extern Param_T g_tParam;

void paramInit(void);//初始化参数结构体
void LoadParam(void);
void SaveParam(void);

#endif

