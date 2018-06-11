#include "bsp.h"

#define  DWT_CYCCNT  *(volatile unsigned int *)0xE0001004
  
void DetectorGPIOInit(void);
uint8_t GetLevel(void);
void ResetDetector(void);

Reader_T g_tReader;
static uint32_t dwtTime;

/***********************************************************
* ��������ReaderInit
* ����  ���ⲿ�ж����ų�ʼ�� 
* ����  : ��
* ���  ����
**********************************************************/
void ReaderInit(void)
{   
    GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    
    DetectorGPIOInit();//�ܼ���������
   
	//�ź��� PA0,1
	/* ʹ�� GPIO ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//���븡��ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* ʹ�� AFIO ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    //���̽�����ж�	PA0
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0); 
	EXTI_InitStructure.EXTI_Line	= EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode 	= EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//�����ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 
	
	//�ж����ȼ�����������Ϊ������ȼ�
	NVIC_InitStructure.NVIC_IRQChannel 	= EXTI0_IRQn;//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ� 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 	= 0x00;//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd 	= ENABLE;//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure); 
	
	//���̽�����ж�  PA1
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);
	EXTI_InitStructure.EXTI_Line	= EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode 	= EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//�����ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	
	
	//�ж����ȼ�����������Ϊ������ȼ�
	NVIC_InitStructure.NVIC_IRQChannel 	= EXTI1_IRQn;	//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//��ռ���ȼ� 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 	= 0x00;	//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd 	= ENABLE;	//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);     
}

//���̽�����ж�//���ȼ�0
void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0) != RESET)
	{		
        g_tReader.tStop = DWT_CYCCNT;
        
        dwtTime = (g_tReader.tStop - g_tReader.tStart)/72;
        //TODO
        //����GPSʱ��
        //memcpy(g_tReader.preciseTime, g_tGPS.time, sizeof(g_tGPS.time));     
        g_tReader.preciseTime[6] = dwtTime>>16;
        g_tReader.preciseTime[7] = (dwtTime>>8)&0xFF;
        g_tReader.preciseTime[8] = dwtTime&0xFF;
        
		EXTI_ClearITPendingBit(EXTI_Line0);	
	}
}

//1pps�ж�,��ÿ�봥��һ��//���ȼ�1
void EXTI1_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line1) != RESET)
	{
        g_tReader.tStart = DWT_CYCCNT;
        g_tReader.tStop = 0;        
		
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

//̽�������ų�ʼ��
void DetectorGPIOInit(void)
{
   //TODO 
}

//��ȡ̽�����ܼ�
uint8_t GetLevel(void)
{
    //TODO 
    return 0;
}

//��λ̽����״̬
void ResetDetector(void)
{
    //TODO 
}
    
//---------------------------------------------------------------
