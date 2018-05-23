#include "bsp.h"

Param_T g_tParam;

//��ʼ�������ṹ��
void paramInit(void)
{
    //�ȶ�ȡ�������ж��Ƿ���Ч�������Ч��ʹ�ú궨��Ĳ���
    LoadParam();
    //���û�д洢��������ʹ��Ĭ��ֵ
    if((0xFF == g_tParam.loraAddress[0]) && (0xFF == g_tParam.loraAddress[1]))
    {
        g_tParam.loraAddress[0] = LORA_ADDRESS & 0xFF;
        g_tParam.loraAddress[1] = LORA_ADDRESS >> 8;
    }
    
    if((0xFF == g_tParam.dtuAddress[0]) && (0xFF == g_tParam.dtuAddress[1]))
    {
        g_tParam.dtuAddress[0] = DTU_ADDRESS & 0xFF;
        g_tParam.dtuAddress[1] = DTU_ADDRESS >> 8;
    }
    
    if(0xFF == g_tParam.channel)
    {
        g_tParam.channel = CHANNEL;
    }
    
    //TODO:����������ɺ󣬸���ʹ�ò�����loraģ��������ã�
    //��Ҫ�л�ģʽ�����ڷ�����������
}

/*
*********************************************************************************************************
*	�� �� ��: LoadParam
*	����˵��: ��Flash��������g_tParam
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LoadParam(void)
{
	ee_ReadBytes((uint8_t *)&g_tParam, PARAM_ADDR, sizeof(Param_T));
}

/*
*********************************************************************************************************
*	�� �� ��: SaveParam
*	����˵��: ��ȫ�ֱ���g_tParam д�뵽IIC Flash
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SaveParam(void)
{
	/* ��ȫ�ֵĲ����������浽EEPROM */
	ee_WriteBytes((uint8_t *)&g_tParam, PARAM_ADDR, sizeof(Param_T));
}

