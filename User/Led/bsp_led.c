/*
*********************************************************************************************************
*
*	ģ������ : LEDָʾ������ģ��
*	�ļ����� : bsp_led.c
*	��    �� : V1.0
*	˵    �� : ����LEDָʾ��
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2013-02-01 armfly  ��ʽ����
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

static void openDoor(SingleRelation_T *singleRelation, enum ReaderOrButton_Enum openWay);//���������Ƕ�ͷA/����A����B�Ķ�Ӧ��ϵ
static void closeDoor(SingleRelation_T *singleRelation, enum ReaderOrButton_Enum openWay);
static void readFeedBack(void);

DoorStatus_T g_tDoorStatus;

/*
	�ó��������ڰ�����STM32-V4 ������
	�����������Ӳ�������޸�GPIO����	
	����û���LEDָʾ�Ƹ���С��4�������Խ������LEDȫ������Ϊ�͵�1��LEDһ��������Ӱ�������
*/

/* �����ڶ�Ӧ��RCCʱ�� */
//2-23 ���D��ʱ��
#define RCC_FB_LED 	(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD)

#define GPIO_PORT_LED1  GPIOC
#define GPIO_PIN_LED1	GPIO_Pin_10//out1

#define GPIO_PORT_LED2  GPIOC
#define GPIO_PIN_LED2	GPIO_Pin_12//out2

#define GPIO_PORT_LED3  GPIOB
#define GPIO_PIN_LED3	GPIO_Pin_5//sysStatus

#define GPIO_PORT_ALARM  GPIOC
#define GPIO_PIN_ALARM_LED1	    GPIO_Pin_0
#define GPIO_PIN_ALARM_BEEP1	GPIO_Pin_1
#define GPIO_PIN_ALARM_LED2	    GPIO_Pin_2
#define GPIO_PIN_ALARM_BEEP2	GPIO_Pin_3

#define GPIO_PORT_FB1  GPIOC
#define GPIO_PIN_FB1	GPIO_Pin_11//fb1
#define READ_FB1    GPIO_ReadInputDataBit(GPIO_PORT_FB1, GPIO_PIN_FB1)//��ȡfb1

#define GPIO_PORT_FB2  GPIOD
#define GPIO_PIN_FB2	GPIO_Pin_2//fb2
#define READ_FB2    GPIO_ReadInputDataBit(GPIO_PORT_FB2, GPIO_PIN_FB2)//��ȡfb2

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitLed
*	����˵��: ����LEDָʾ����ص�GPIO,  �ú����� bsp_Init() ���á�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitLed(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    //���ƶ�Ӧ�ĺ���ָ��
    g_tDoorStatus.openDoor = openDoor;
    g_tDoorStatus.closeDoor = closeDoor;
    g_tDoorStatus.readFeedBack = readFeedBack;
	/* ��GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_FB_LED, ENABLE);

	/*
		�������е�LEDָʾ��GPIOΪ�������ģʽ
		���ڽ�GPIO����Ϊ���ʱ��GPIO����Ĵ�����ֵȱʡ��0����˻�����LED����.
		�����Ҳ�ϣ���ģ�����ڸı�GPIOΪ���ǰ���ȹر�LEDָʾ��
	*/
    //��ʼ״̬Ϊ�ر�
    //�̵��������ȹرգ���ʼ��ģʽ��Ϊ�͵�ƽ
	bsp_LedOff(1);//�̵���out1
    g_tDoorStatus.doorA.switcherStatus = NC;
	bsp_LedOff(2);//�̵���out2
    g_tDoorStatus.doorB.switcherStatus = NC;
	bsp_LedOff(3);//ϵͳ״̬��

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	/* �������ģʽ */
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED1;
	GPIO_Init(GPIO_PORT_LED1, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED2;
	GPIO_Init(GPIO_PORT_LED2, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED3;
	GPIO_Init(GPIO_PORT_LED3, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ALARM_LED1 | GPIO_PIN_ALARM_BEEP1 | GPIO_PIN_ALARM_LED2 | GPIO_PIN_ALARM_BEEP2;
	GPIO_Init(GPIO_PORT_ALARM, &GPIO_InitStructure);
    

//��������������
    GPIO_InitStructure.GPIO_Pin  = GPIO_PIN_FB1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIO_PORT_FB1, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin  = GPIO_PIN_FB2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIO_PORT_FB2, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_LedOn
*	����˵��: ����ָ����LEDָʾ�ơ�
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 3
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_LedOn(uint8_t _no)
{
	_no--;

	if (_no == 0)
	{
		GPIO_PORT_LED1->BRR = GPIO_PIN_LED1;
	}
	else if (_no == 1)
	{
		GPIO_PORT_LED2->BRR = GPIO_PIN_LED2;
	}
	else if (_no == 2)
	{
		GPIO_PORT_LED3->BRR = GPIO_PIN_LED3;
	}
//	else if (_no == 3)
//	{
//		GPIO_PORT_LED4->BRR = GPIO_PIN_LED4;
//	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_LedOff
*	����˵��: Ϩ��ָ����LEDָʾ�ơ�
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 3
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_LedOff(uint8_t _no)
{
	_no--;

	if (_no == 0)
	{
		GPIO_PORT_LED1->BSRR = GPIO_PIN_LED1;
	}
	else if (_no == 1)
	{
		GPIO_PORT_LED2->BSRR = GPIO_PIN_LED2;
	}
	else if (_no == 2)
	{
		GPIO_PORT_LED3->BSRR = GPIO_PIN_LED3;
	}
//	else if (_no == 3)
//	{
//		GPIO_PORT_LED4->BSRR = GPIO_PIN_LED4;
//	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_LedToggle
*	����˵��: ��תָ����LEDָʾ�ơ�
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 3
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
void bsp_LedToggle(uint8_t _no)
{
	if (_no == 1)
	{
		GPIO_PORT_LED1->ODR ^= GPIO_PIN_LED1;
	}
	else if (_no == 2)
	{
		GPIO_PORT_LED2->ODR ^= GPIO_PIN_LED2;
	}
	else if (_no == 3)
	{
		GPIO_PORT_LED3->ODR ^= GPIO_PIN_LED3;
	}
//	else if (_no == 4)
//	{
//		GPIO_PORT_LED4->ODR ^= GPIO_PIN_LED4;
//	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_IsLedOn
*	����˵��: �ж�LEDָʾ���Ƿ��Ѿ�������
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 3
*	�� �� ֵ: 1��ʾ�Ѿ�������0��ʾδ����
*********************************************************************************************************
*/
uint8_t bsp_IsLedOn(uint8_t _no)
{
	if (_no == 1)
	{
		if ((GPIO_PORT_LED1->ODR & GPIO_PIN_LED1) == 0)
		{
			return 1;
		}
		return 0;
	}
	else if (_no == 2)
	{
		if ((GPIO_PORT_LED2->ODR & GPIO_PIN_LED2) == 0)
		{
			return 1;
		}
		return 0;
	}
	else if (_no == 3)
	{
		if ((GPIO_PORT_LED3->ODR & GPIO_PIN_LED3) == 0)
		{
			return 1;
		}
		return 0;
	}
//	else if (_no == 4)
//	{
//		if ((GPIO_PORT_LED4->ODR & GPIO_PIN_LED4) == 0)
//		{
//			return 1;
//		}
//		return 0;
//	}

	return 0;
}

//���ݶ�Ӧ��ϵ����
static void openDoor(SingleRelation_T *singleRelation, enum ReaderOrButton_Enum openWay)
{
    uint8_t low_a;
    uint8_t high_b;
    switch(openWay)
    {
        case e_READER_A:
        case e_READER_B:
            low_a = singleRelation->reader_switcher & 0x01;//�̵���a
            high_b = (singleRelation->reader_switcher >> 4) & 0x01;//�̵���b
            if(low_a == 1)
            {
                bsp_LedOn(1);
                g_tDoorStatus.doorA.switcherStatus = NO;
            }
            if(high_b == 1)
            {
                bsp_LedOn(2);
                g_tDoorStatus.doorB.switcherStatus = NO;
            }
            break;
        
        case e_BUTTON_A:
        case e_BUTTON_B:
            low_a = singleRelation->button_switcher & 0x01;//�̵���a
            high_b = (singleRelation->button_switcher >> 4) & 0x01;//�̵���b
            if(low_a == 1)
            {
                bsp_LedOn(1);
                g_tDoorStatus.doorA.switcherStatus = NO;
            }
            if(high_b == 1)
            {
                bsp_LedOn(2);
                g_tDoorStatus.doorB.switcherStatus = NO;
            }
            break;
            
        default:
            break;
    }  
}

//���ݶ�Ӧ��ϵ����,��Ҫ������ֹ��ŷ�ʽ
static void closeDoor(SingleRelation_T *singleRelation, enum ReaderOrButton_Enum openWay)
{
    uint8_t low_a;
    uint8_t high_b;
    switch(openWay)
    {
        case e_READER_A:
        case e_READER_B:
            low_a = singleRelation->reader_switcher & 0x01;//�̵���a
            high_b = (singleRelation->reader_switcher >> 4) & 0x01;//�̵���b
            if(low_a == 1)
            {
                bsp_LedOff(1);
                g_tDoorStatus.doorA.switcherStatus = NC;
            }
            if(high_b == 1)
            {
                bsp_LedOff(2);
                g_tDoorStatus.doorB.switcherStatus = NC;
            }
            break;
        
        case e_BUTTON_A:
        case e_BUTTON_B:
            low_a = singleRelation->button_switcher & 0x01;//�̵���a
            high_b = (singleRelation->button_switcher >> 4) & 0x01;//�̵���b
            if(low_a == 1)
            {
                bsp_LedOff(1);
                g_tDoorStatus.doorA.switcherStatus = NC;
            }
            if(high_b == 1)
            {
                bsp_LedOff(2);
                g_tDoorStatus.doorB.switcherStatus = NC;
            }
            break;
            
        default:
            break;
    } 
}

//�����������
static void readFeedBack(void)
{
    if(READ_FB1 == 0)//0��ʾ�͵�ƽ
    {
        //���������ĵ͵�ƽ��ʾ����NC
        g_tDoorStatus.doorA.feedBackStatus = NC;
    }
    else g_tDoorStatus.doorA.feedBackStatus = NO;
    
    //if(READ_FB2 == 0)
    if(GPIO_ReadInputDataBit(GPIO_PORT_FB2, GPIO_PIN_FB2) == 0)
    {
        g_tDoorStatus.doorB.feedBackStatus = NC;
    }
    else g_tDoorStatus.doorB.feedBackStatus = NO;
}


//�رձ���
void alarmOff(enum ReaderOrButton_Enum type)
{
    switch(type)
    {
        case e_READER_A:
            //GPIO_ResetBits(GPIO_PORT_ALARM, GPIO_PIN_ALARM_LED1);//�õ͵�ƽ
            GPIO_ResetBits(GPIO_PORT_ALARM, GPIO_PIN_ALARM_BEEP1);
            break;
        
        case e_READER_B:
            //GPIO_ResetBits(GPIO_PORT_ALARM, GPIO_PIN_ALARM_LED2);//�õ͵�ƽ
            GPIO_ResetBits(GPIO_PORT_ALARM, GPIO_PIN_ALARM_BEEP2);
            break;
                
        default:
            break;
    }
}

//�򿪱���
void alarmOn(enum ReaderOrButton_Enum type)
{
    switch(type)
    {
        case e_READER_A:
            //GPIO_SetBits(GPIO_PORT_ALARM, GPIO_PIN_ALARM_LED1);//�øߵ�ƽ
            GPIO_SetBits(GPIO_PORT_ALARM, GPIO_PIN_ALARM_BEEP1);
            break;
        
        case e_READER_B:
            //GPIO_SetBits(GPIO_PORT_ALARM, GPIO_PIN_ALARM_LED2);//�øߵ�ƽ
            GPIO_SetBits(GPIO_PORT_ALARM, GPIO_PIN_ALARM_BEEP2);
            break;
                
        default:
            break;
    }
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
