#ifndef __PARAM_H
#define __PARAM_H

#define PARAM_ADDR		0			/* ��������ַ */

//Ĭ�ϲ���
#define DTU_ADDRESS 0xFFFE //DTU��ַ

//Ӳ���������
#define LORA_PORT    GPIOB
#define RCC_LORA_PORT  (RCC_APB2Periph_GPIOB)

#define AUX_PIN     GPIO_Pin_0
#define READ_AUX    GPIO_ReadInputDataBit(LORA_PORT, AUX_PIN)//��ȡAUX״ָ̬ʾ���ŵ�ƽ

#define LORA_M0_PIN	GPIO_Pin_1//LORA_M0
#define LORA_M1_PIN	GPIO_Pin_2//LORA_M1

typedef struct
{
    uint8_t isUpdate;//0xFF��ʾ������Ĭ������1��ʾ��Ҫ������0��ʾ������app��
    //loraģ���DTU��ͨ�Ų���
    uint8_t loraAddress[2];
    uint8_t dtuAddress[2];
    uint8_t channel;
}Param_T;

extern Param_T g_tParam;


void LoadParam(void);
void SaveParam(void);

void resetParam(void);
void paramInit(void);//��ʼ�������ṹ��
void setLoraParam(void);//����loraģ�����
uint8_t getAuxStatus(void);
#endif

