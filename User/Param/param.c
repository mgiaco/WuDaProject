#include "bsp.h"

/****����lora�ĵ�ַ��Ҳ��Ϊ�豸ID�ţ�1-500������ȡ0��FFFF��*/
#define LORA_ADDRESS         1
/*********************************************************/

Param_T g_tParam;

//��ʼ��loraģ��
void loraInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_LORA_PORT, ENABLE);//ʹ��PORT_Bʱ��
	
	GPIO_InitStructure.GPIO_Pin  = AUX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(LORA_PORT, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	/* �������ģʽ */	
	GPIO_InitStructure.GPIO_Pin = LORA_M0_PIN | LORA_M1_PIN;
	GPIO_Init(LORA_PORT, &GPIO_InitStructure);
}

//��ʼ�������ṹ��
void paramInit(void)
{
    //��ʼ��loraģ��
    loraInit();
    
    //test����IIC flashΪ0xFF
//    resetParam();
//    bsp_DelayMS(100);
    
    
    //�ȶ�ȡ�������ж��Ƿ���Ч�������Ч��ʹ�ú궨��Ĳ���
    LoadParam();
    
    //���û�д洢��������ʹ��Ĭ��ֵ
    if((0xFF == g_tParam.loraAddress[0]) && (0xFF == g_tParam.loraAddress[1]))
    {
        g_tParam.loraAddress[1] = LORA_ADDRESS & 0xFF;
        g_tParam.loraAddress[0] = LORA_ADDRESS >> 8;
    }
    
    if((0xFF == g_tParam.dtuAddress[0]) && (0xFF == g_tParam.dtuAddress[1]))
    {
        g_tParam.dtuAddress[1] = DTU_ADDRESS & 0xFF;
        g_tParam.dtuAddress[0] = DTU_ADDRESS >> 8;
    }
    
    if(0xFF == g_tParam.channel)
    {
        g_tParam.channel = CHANNEL;
    }
    
    //����������ɺ󣬸���ʹ�ò�����loraģ��������ã�
    //��Ҫ�л�ģʽ�����ڷ�����������
    setLoraParam();
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

//����IIC����
void resetParam(void)
{
    uint16_t i;
    uint8_t buf[EE_PAGE_SIZE];  
    
    for(i = 0; i < EE_PAGE_SIZE; i++)
	{
		buf[i] = 0xFF;
	}
    
    ee_WriteBytes(buf, 0, EE_PAGE_SIZE);
}

//����loraģ��ģʽ��0--������1-��������
void setLoraMode(uint8_t mode)
{
    if(mode == 0)//�õ͵�ƽ
    {
        GPIO_ResetBits(LORA_PORT, LORA_M0_PIN);
        GPIO_ResetBits(LORA_PORT, LORA_M1_PIN);
    }
    else //�øߵ�ƽ
    {
        GPIO_SetBits(LORA_PORT, LORA_M0_PIN);
        GPIO_SetBits(LORA_PORT, LORA_M1_PIN);
    }
}


//����loraģ�����
void setLoraParam(void)
{
    uint8_t loraParam[6];
    
    loraParam[0]=0xC0;//�������籣��
    loraParam[1]=g_tParam.loraAddress[0];
    loraParam[2]=g_tParam.loraAddress[1];
    loraParam[3]=0x1A;//��������Ĭ��9600������Ĭ��2.4K
    loraParam[4]=g_tParam.channel;//�ŵ�Ϊ30,410+30=440MHz
    loraParam[5]=0xC0;//���䷽ʽѡ�񶨵㴫�䣬���书�ʺ����߻���ʱ�䡢FEC����ʹ��Ĭ��ֵ
    //������M0��M1
    setLoraMode(1);
    bsp_DelayMS(100);
    //�����������
    LoraSendData(loraParam, sizeof(loraParam));
    bsp_DelayMS(100);
    //������M0��M1
    setLoraMode(0);
    bsp_DelayMS(100);
}

//��ȡAUX���ŵ�ƽ״̬
uint8_t getAuxStatus(void)
{
    return READ_AUX;
}

//---------------------------
