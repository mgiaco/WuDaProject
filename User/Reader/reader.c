#include "bsp.h"

#define  DWT_CYCCNT  *(volatile unsigned int *)0xE0001004
    
#define DETECTOR_RESET_PORT    GPIOA
#define DETECTOR_RESET_PIN	 GPIO_Pin_3//PA3���ߵ�ƽ��Ч

//20180619�����ڵ��ܼ����ǰ���������Ʒ�ʽ�����
//��OUT8��ʼ���ߵ�ƽ������Ϊ�ܼ�
//0UT1--4��ӦPB12--15
#define DETECTOR_PORT_B    GPIOB
#define RCC_DETECTOR_PORT_B  (RCC_APB2Periph_GPIOB)

#define OUT1_PIN     GPIO_Pin_12
#define READ_OUT1    GPIO_ReadInputDataBit(DETECTOR_PORT_B,OUT1_PIN)

#define OUT2_PIN     GPIO_Pin_13
#define READ_OUT2    GPIO_ReadInputDataBit(DETECTOR_PORT_B,OUT2_PIN)

#define OUT3_PIN     GPIO_Pin_14
#define READ_OUT3    GPIO_ReadInputDataBit(DETECTOR_PORT_B,OUT3_PIN)

#define OUT4_PIN     GPIO_Pin_15
#define READ_OUT4    GPIO_ReadInputDataBit(DETECTOR_PORT_B,OUT4_PIN)

////0UT5--8��ӦPC6--9
#define DETECTOR_PORT_C    GPIOC
#define RCC_DETECTOR_PORT_C  (RCC_APB2Periph_GPIOC)

#define OUT5_PIN     GPIO_Pin_6
#define READ_OUT5    GPIO_ReadInputDataBit(DETECTOR_PORT_C,OUT5_PIN)

#define OUT6_PIN     GPIO_Pin_7
#define READ_OUT6    GPIO_ReadInputDataBit(DETECTOR_PORT_C,OUT6_PIN)

#define OUT7_PIN     GPIO_Pin_8
#define READ_OUT7    GPIO_ReadInputDataBit(DETECTOR_PORT_C,OUT7_PIN)

#define OUT8_PIN     GPIO_Pin_9
#define READ_OUT8    GPIO_ReadInputDataBit(DETECTOR_PORT_C,OUT8_PIN)

Reader_T g_tReader;
static uint32_t dwtTime;//�����ж�֮���ʱ��--΢��

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
   
	//�ź��� PA0,1
	/* ʹ�� GPIO ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//���븡��ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    //����PA3
    GPIO_InitStructure.GPIO_Pin  = DETECTOR_RESET_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	/* �������ģʽ */	
	GPIO_Init(DETECTOR_RESET_PORT, &GPIO_InitStructure);
    
    
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

    //̽������λ���ų�ʼ����ÿ���ϵ�ִ��
    ResetDetector();
}

//���̽�����ж�//���ȼ�0
void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0) != RESET)
	{		
        g_tReader.tStop = DWT_CYCCNT;    
        
        dwtTime = (g_tReader.tStop - g_tReader.tStart)/72;
        //����GPSʱ�䣬��������
        g_tReader.preciseTime[0] = (g_tGps.time[0]-0x30)*10+(g_tGps.time[1]-0x30) + 8;//Сʱ��ת�����й�ʱ����8
        g_tReader.preciseTime[1] = (g_tGps.time[2]-0x30)*10+(g_tGps.time[3]-0x30);//����
        g_tReader.preciseTime[2] = (g_tGps.time[4]-0x30)*10+(g_tGps.time[5]-0x30);//��
        //΢��
        g_tReader.preciseTime[3] = dwtTime>>16;
        g_tReader.preciseTime[4] = (dwtTime>>8)&0xFF;
        g_tReader.preciseTime[5] = dwtTime&0xFF;
        //��ȡ�ܼ�״̬
        //20180620
        //��ƽ��ȡ��׼ȷ�������ʱ
        //���¼���־�飬����������
        isr_evt_set(GET_LEVEL_BIT, HandleTaskNet);//post
        
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
        dwtTime = 0;        
		
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

//̽�������ų�ʼ��
void DetectorInit(void)
{
   //���óɸ������룬�ߵ�ƽ��Ч
    GPIO_InitTypeDef GPIO_InitStructure;
    //PB12-15
 	RCC_APB2PeriphClockCmd(RCC_DETECTOR_PORT_B, ENABLE);//ʹ��PORT_Bʱ��
	
	GPIO_InitStructure.GPIO_Pin  = OUT1_PIN | OUT2_PIN | OUT3_PIN | OUT4_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DETECTOR_PORT_B, &GPIO_InitStructure);
    
    //PC6-9
    RCC_APB2PeriphClockCmd(RCC_DETECTOR_PORT_C, ENABLE);//ʹ��PORT_Cʱ��
	
	GPIO_InitStructure.GPIO_Pin  = OUT5_PIN | OUT6_PIN | OUT7_PIN | OUT8_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DETECTOR_PORT_C, &GPIO_InitStructure);
}

//��ȡ̽�����ܼ�
uint8_t GetDetectorLevel(void)
{
    //���λ�ȡ��ƽ״̬��Ȼ��ϳ�һ����
    uint8_t level=0;  
    
    if(READ_OUT8)
    {
        level++;
        
        if(READ_OUT7)
        {
            level++;
            
            if(READ_OUT6)
            {
                level++;
                
                if(READ_OUT5)
                {
                    level++;
                    
                    if(READ_OUT4)
                    {
                        level++;
                        
                        if(READ_OUT3)
                        {
                            level++;
                            
                            if(READ_OUT2)
                            {
                                level++;
                                
                                if(READ_OUT1)
                                {
                                    level++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    return level;
}

//��λ̽����״̬
void ResetDetector(void)
{
    //�õ�
    GPIO_ResetBits(DETECTOR_RESET_PORT, DETECTOR_RESET_PIN);
    //��ʱ
    bsp_DelayMS(100);   
    //�ø�
    GPIO_SetBits(DETECTOR_RESET_PORT, DETECTOR_RESET_PIN);
}
    
//---------------------------------------------------------------
