#ifndef __PARAM_H
#define __PARAM_H

#define PARAM_ADDR		0			/* 参数区地址 */

//默认参数
#define DTU_ADDRESS 0xFFFE //DTU地址

//硬件引脚相关
#define LORA_PORT    GPIOB
#define RCC_LORA_PORT  (RCC_APB2Periph_GPIOB)

#define AUX_PIN     GPIO_Pin_0
#define READ_AUX    GPIO_ReadInputDataBit(LORA_PORT, AUX_PIN)//读取AUX状态指示引脚电平

#define LORA_M0_PIN	GPIO_Pin_1//LORA_M0
#define LORA_M1_PIN	GPIO_Pin_2//LORA_M1

typedef struct
{
    uint8_t isUpdate;//0xFF表示启动到默认区，1表示需要升级，0表示启动到app区
    //lora模块和DTU的通信参数
    uint8_t loraAddress[2];
    uint8_t dtuAddress[2];
    uint8_t channel;
}Param_T;

extern Param_T g_tParam;


void LoadParam(void);
void SaveParam(void);

void resetParam(void);
void paramInit(void);//初始化参数结构体
void setLoraParam(void);//设置lora模块参数
uint8_t getAuxStatus(void);
#endif

