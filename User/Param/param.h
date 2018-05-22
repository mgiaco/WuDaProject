#ifndef __PARAM_H
#define __PARAM_H

#define PARAM_ADDR		0			/* ��������ַ */
//Ĭ�ϲ���
#define VERSION 20180521 // �汾��

#define CHANNEL 30 //lora���ŵ�������410+30=440MHz(�ŵ���ͬ���ܻ���ͨ��)
#define DTU_ADDRESS 0xFFFE //DTU��ַ

typedef struct
{
    uint8_t isUpdate;//0xFF��ʾ������Ĭ������1��ʾ��Ҫ������0��ʾ������app��
    //loraģ���DTU��ͨ�Ų���
    uint8_t loraAddress[2];
    uint8_t dtuAddress[2];
    uint8_t channel;
    
}Param_T;

extern Param_T g_tParam;

void paramInit(void);//��ʼ�������ṹ��
void LoadParam(void);
void SaveParam(void);

#endif

