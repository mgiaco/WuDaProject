/*
*********************************************************************************************************
*
*	模块名称 : LED指示灯驱动模块
*	文件名称 : bsp_led.c
*	版    本 : V1.0
*	说    明 : 驱动LED指示灯
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2013-02-01 armfly  正式发布
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

static void openDoor(SingleRelation_T *singleRelation, enum ReaderOrButton_Enum openWay);//传进来的是读头A/按键A或者B的对应关系
static void closeDoor(SingleRelation_T *singleRelation, enum ReaderOrButton_Enum openWay);
static void readFeedBack(void);

DoorStatus_T g_tDoorStatus;

/*
	该程序适用于安富莱STM32-V4 开发板
	如果用于其它硬件，请修改GPIO定义	
	如果用户的LED指示灯个数小于4个，可以将多余的LED全部定义为和第1个LED一样，并不影响程序功能
*/

/* 按键口对应的RCC时钟 */
//2-23 添加D口时钟
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
#define READ_FB1    GPIO_ReadInputDataBit(GPIO_PORT_FB1, GPIO_PIN_FB1)//读取fb1

#define GPIO_PORT_FB2  GPIOD
#define GPIO_PIN_FB2	GPIO_Pin_2//fb2
#define READ_FB2    GPIO_ReadInputDataBit(GPIO_PORT_FB2, GPIO_PIN_FB2)//读取fb2

/*
*********************************************************************************************************
*	函 数 名: bsp_InitLed
*	功能说明: 配置LED指示灯相关的GPIO,  该函数被 bsp_Init() 调用。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitLed(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    //复制对应的函数指针
    g_tDoorStatus.openDoor = openDoor;
    g_tDoorStatus.closeDoor = closeDoor;
    g_tDoorStatus.readFeedBack = readFeedBack;
	/* 打开GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_FB_LED, ENABLE);

	/*
		配置所有的LED指示灯GPIO为推挽输出模式
		由于将GPIO设置为输出时，GPIO输出寄存器的值缺省是0，因此会驱动LED点亮.
		这是我不希望的，因此在改变GPIO为输出前，先关闭LED指示灯
	*/
    //初始状态为关闭
    //继电器不用先关闭，初始化模式后为低电平
	bsp_LedOff(1);//继电器out1
    g_tDoorStatus.doorA.switcherStatus = NC;
	bsp_LedOff(2);//继电器out2
    g_tDoorStatus.doorB.switcherStatus = NC;
	bsp_LedOff(3);//系统状态灯

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	/* 推挽输出模式 */
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED1;
	GPIO_Init(GPIO_PORT_LED1, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED2;
	GPIO_Init(GPIO_PORT_LED2, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED3;
	GPIO_Init(GPIO_PORT_LED3, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ALARM_LED1 | GPIO_PIN_ALARM_BEEP1 | GPIO_PIN_ALARM_LED2 | GPIO_PIN_ALARM_BEEP2;
	GPIO_Init(GPIO_PORT_ALARM, &GPIO_InitStructure);
    

//两根门吸反馈线
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
*	函 数 名: bsp_LedOn
*	功能说明: 点亮指定的LED指示灯。
*	形    参:  _no : 指示灯序号，范围 1 - 3
*	返 回 值: 无
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
*	函 数 名: bsp_LedOff
*	功能说明: 熄灭指定的LED指示灯。
*	形    参:  _no : 指示灯序号，范围 1 - 3
*	返 回 值: 无
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
*	函 数 名: bsp_LedToggle
*	功能说明: 翻转指定的LED指示灯。
*	形    参:  _no : 指示灯序号，范围 1 - 3
*	返 回 值: 按键代码
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
*	函 数 名: bsp_IsLedOn
*	功能说明: 判断LED指示灯是否已经点亮。
*	形    参:  _no : 指示灯序号，范围 1 - 3
*	返 回 值: 1表示已经点亮，0表示未点亮
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

//根据对应关系开门
static void openDoor(SingleRelation_T *singleRelation, enum ReaderOrButton_Enum openWay)
{
    uint8_t low_a;
    uint8_t high_b;
    switch(openWay)
    {
        case e_READER_A:
        case e_READER_B:
            low_a = singleRelation->reader_switcher & 0x01;//继电器a
            high_b = (singleRelation->reader_switcher >> 4) & 0x01;//继电器b
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
            low_a = singleRelation->button_switcher & 0x01;//继电器a
            high_b = (singleRelation->button_switcher >> 4) & 0x01;//继电器b
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

//根据对应关系关门,还要传入何种关门方式
static void closeDoor(SingleRelation_T *singleRelation, enum ReaderOrButton_Enum openWay)
{
    uint8_t low_a;
    uint8_t high_b;
    switch(openWay)
    {
        case e_READER_A:
        case e_READER_B:
            low_a = singleRelation->reader_switcher & 0x01;//继电器a
            high_b = (singleRelation->reader_switcher >> 4) & 0x01;//继电器b
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
            low_a = singleRelation->button_switcher & 0x01;//继电器a
            high_b = (singleRelation->button_switcher >> 4) & 0x01;//继电器b
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

//检测门吸反馈
static void readFeedBack(void)
{
    if(READ_FB1 == 0)//0表示低电平
    {
        //门吸反馈的低电平表示常闭NC
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


//关闭报警
void alarmOff(enum ReaderOrButton_Enum type)
{
    switch(type)
    {
        case e_READER_A:
            //GPIO_ResetBits(GPIO_PORT_ALARM, GPIO_PIN_ALARM_LED1);//置低电平
            GPIO_ResetBits(GPIO_PORT_ALARM, GPIO_PIN_ALARM_BEEP1);
            break;
        
        case e_READER_B:
            //GPIO_ResetBits(GPIO_PORT_ALARM, GPIO_PIN_ALARM_LED2);//置低电平
            GPIO_ResetBits(GPIO_PORT_ALARM, GPIO_PIN_ALARM_BEEP2);
            break;
                
        default:
            break;
    }
}

//打开报警
void alarmOn(enum ReaderOrButton_Enum type)
{
    switch(type)
    {
        case e_READER_A:
            //GPIO_SetBits(GPIO_PORT_ALARM, GPIO_PIN_ALARM_LED1);//置高电平
            GPIO_SetBits(GPIO_PORT_ALARM, GPIO_PIN_ALARM_BEEP1);
            break;
        
        case e_READER_B:
            //GPIO_SetBits(GPIO_PORT_ALARM, GPIO_PIN_ALARM_LED2);//置高电平
            GPIO_SetBits(GPIO_PORT_ALARM, GPIO_PIN_ALARM_BEEP2);
            break;
                
        default:
            break;
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
