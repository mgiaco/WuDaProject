#include "bsp.h"

#define  DWT_CYCCNT  *(volatile unsigned int *)0xE0001004
    
#define DETECTOR_RESET_PORT    GPIOA
#define DETECTOR_RESET_PIN	 GPIO_Pin_3//PA3，高电平有效

//20180619：现在的能级不是按编码二进制方式来算的
//从OUT8开始，高电平个数即为能级
//0UT1--4对应PB12--15
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

////0UT5--8对应PC6--9
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
static uint32_t dwtTime;//两个中断之间的时间--微秒

/***********************************************************
* 函数名：ReaderInit
* 功能  ：外部中断引脚初始化 
* 输入  : 无
* 输出  ：无
**********************************************************/
void ReaderInit(void)
{   
    GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
   
	//信号线 PA0,1
	/* 使能 GPIO 时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//输入浮空模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    //设置PA3
    GPIO_InitStructure.GPIO_Pin  = DETECTOR_RESET_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	/* 推挽输出模式 */	
	GPIO_Init(DETECTOR_RESET_PORT, &GPIO_InitStructure);
    
    
	/* 使能 AFIO 时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    //光电探测器中断	PA0
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0); 
	EXTI_InitStructure.EXTI_Line	= EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode 	= EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 
	
	//中断优先级，尽可能设为最高优先级
	NVIC_InitStructure.NVIC_IRQChannel 	= EXTI0_IRQn;//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 	= 0x00;//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd 	= ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 
	
	//光电探测器中断  PA1
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);
	EXTI_InitStructure.EXTI_Line	= EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode 	= EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	
	
	//中断优先级，尽可能设为最高优先级
	NVIC_InitStructure.NVIC_IRQChannel 	= EXTI1_IRQn;	//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//抢占优先级 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 	= 0x00;	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd 	= ENABLE;	//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);   

    //探测器复位引脚初始化，每次上电执行
    ResetDetector();
}

//光电探测器中断//优先级0
void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0) != RESET)
	{		
        g_tReader.tStop = DWT_CYCCNT;    
        
        dwtTime = (g_tReader.tStop - g_tReader.tStart)/72;
        //复制GPS时间，并做处理
        g_tReader.preciseTime[0] = (g_tGps.time[0]-0x30)*10+(g_tGps.time[1]-0x30) + 8;//小时，转换成中国时区加8
        g_tReader.preciseTime[1] = (g_tGps.time[2]-0x30)*10+(g_tGps.time[3]-0x30);//分钟
        g_tReader.preciseTime[2] = (g_tGps.time[4]-0x30)*10+(g_tGps.time[5]-0x30);//秒
        //微秒
        g_tReader.preciseTime[3] = dwtTime>>16;
        g_tReader.preciseTime[4] = (dwtTime>>8)&0xFF;
        g_tReader.preciseTime[5] = dwtTime&0xFF;
        //读取能级状态
        //20180620
        //电平读取不准确，添加延时
        //加事件标志组，在任务中做
        isr_evt_set(GET_LEVEL_BIT, HandleTaskNet);//post
        
		EXTI_ClearITPendingBit(EXTI_Line0);	
	}
}

//1pps中断,会每秒触发一次//优先级1
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

//探测器引脚初始化
void DetectorInit(void)
{
   //设置成浮空输入，高电平有效
    GPIO_InitTypeDef GPIO_InitStructure;
    //PB12-15
 	RCC_APB2PeriphClockCmd(RCC_DETECTOR_PORT_B, ENABLE);//使能PORT_B时钟
	
	GPIO_InitStructure.GPIO_Pin  = OUT1_PIN | OUT2_PIN | OUT3_PIN | OUT4_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DETECTOR_PORT_B, &GPIO_InitStructure);
    
    //PC6-9
    RCC_APB2PeriphClockCmd(RCC_DETECTOR_PORT_C, ENABLE);//使能PORT_C时钟
	
	GPIO_InitStructure.GPIO_Pin  = OUT5_PIN | OUT6_PIN | OUT7_PIN | OUT8_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DETECTOR_PORT_C, &GPIO_InitStructure);
}

//获取探测器能级
uint8_t GetDetectorLevel(void)
{
    //依次获取电平状态，然后合成一个数
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

//复位探测器状态
void ResetDetector(void)
{
    //置低
    GPIO_ResetBits(DETECTOR_RESET_PORT, DETECTOR_RESET_PIN);
    //延时
    bsp_DelayMS(100);   
    //置高
    GPIO_SetBits(DETECTOR_RESET_PORT, DETECTOR_RESET_PIN);
}
    
//---------------------------------------------------------------
