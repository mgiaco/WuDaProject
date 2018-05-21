#include "bsp.h"

static void OpenWiegandDelayTimerA(void);
static void HandlDelayTimerA(void);
static void OpenWiegandDelayTimerB(void);
static void HandlDelayTimerB(void);

static uint8_t DataCheck(SingleReader_T *reader);

Reader_T g_tReader;

/***********************************************************
* 函数名：WiegandInit
* 功能  ：外部中断引脚初始化 
* 输入  : 无
* 输出  ：无
**********************************************************/
void WiegandInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    
    g_tReader.readerA.readerID = e_READER_A;
    g_tReader.readerA.writeIndex = 0;
    g_tReader.readerA.isDelayTimerOpen = 0;
    
    g_tReader.readerB.readerID = e_READER_B;
    g_tReader.readerB.writeIndex = 0;
    g_tReader.readerB.isDelayTimerOpen = 0;
    
    g_tReader.dataCheck = DataCheck;
    
    DetectReader();
    
	//韦根信号线 PA0,1,2,3
	/* 使能 GPIO 时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	  	//上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* 使能 AFIO 时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	//读头A
    //Data0 中断线	PA0
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0); 
	EXTI_InitStructure.EXTI_Line	= EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode 	= EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	//下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 
	
	//中断优先级，尽可能设为最高优先级
	NVIC_InitStructure.NVIC_IRQChannel 	= EXTI0_IRQn;//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 	= 0x00;//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd 	= ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 
	
	//Data1 中断线  PA1
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);
	EXTI_InitStructure.EXTI_Line	= EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode 	= EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	  	//下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	
	
	//中断优先级，尽可能设为最高优先级
	NVIC_InitStructure.NVIC_IRQChannel 	= EXTI1_IRQn;	//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//抢占优先级 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 	= 0x00;	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd 	= ENABLE;	//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 
    
    //读头B
    //Data0 中断线	PA2
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource2); 
	EXTI_InitStructure.EXTI_Line	= EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode 	= EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	//下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 
	
	//中断优先级，尽可能设为最高优先级
	NVIC_InitStructure.NVIC_IRQChannel 	= EXTI2_IRQn;//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 	= 0x00;//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd 	= ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 
	
	//Data1 中断线  PA3
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource3);
	EXTI_InitStructure.EXTI_Line	= EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode 	= EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	  	//下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	
	
	//中断优先级，尽可能设为最高优先级
	NVIC_InitStructure.NVIC_IRQChannel 	= EXTI3_IRQn;	//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//抢占优先级 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 	= 0x00;	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd 	= ENABLE;	//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 
}

//读头A的data0
void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		//Data0	-> 低电平表示1位0
        if(g_tReader.readerA.isDelayTimerOpen == FALSE)
        {
            OpenWiegandDelayTimerA();
        }
        
        g_tReader.readerA.buf[g_tReader.readerA.writeIndex++] = 0;
        if(g_tReader.readerA.writeIndex == 26)
        {
            g_tReader.readerA.writeIndex = 0;
            
            /* 如果消息邮箱还有空间，向消息邮箱发送数据 */
            if (isr_mbx_check (&mailboxCardRX) != 0)
            {
                isr_mbx_send(&mailboxCardRX, &g_tReader.readerA);		
            }
            //发送任务事件标志
            //isr_evt_set(READER_A_BIT, HandleTaskDoor);
        }
		
		EXTI_ClearITPendingBit(EXTI_Line0);	
	}
}

//读头A的data1
void EXTI1_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line1) != RESET)
	{
		//Data1 -> 低电平表示1位1
        if(g_tReader.readerA.isDelayTimerOpen == FALSE)
        {
            OpenWiegandDelayTimerA();
        }
        
        g_tReader.readerA.buf[g_tReader.readerA.writeIndex++] = 1;
        if(g_tReader.readerA.writeIndex == 26)
        {
            g_tReader.readerA.writeIndex = 0;           
            
            /* 如果消息邮箱还有空间，向消息邮箱发送数据 */
            if (isr_mbx_check (&mailboxCardRX) != 0)
            {
                isr_mbx_send(&mailboxCardRX, &g_tReader.readerA);		
            }
            //发送任务事件标志
            //isr_evt_set(READER_A_BIT, HandleTaskDoor);
        }
		
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

//读头B的data0
void EXTI2_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line2) != RESET)
	{
		//Data0	-> 低电平表示1位0
        if(g_tReader.readerB.isDelayTimerOpen == FALSE)
        {
            OpenWiegandDelayTimerB();
        }
        
        g_tReader.readerB.buf[g_tReader.readerB.writeIndex++] = 0;
        if(g_tReader.readerB.writeIndex == 26)
        {
            g_tReader.readerB.writeIndex = 0;
            
            /* 如果消息邮箱还有空间，向消息邮箱发送数据 */
            if (isr_mbx_check (&mailboxCardRX) != 0)
            {
                isr_mbx_send(&mailboxCardRX, &g_tReader.readerB);		
            }
            //发送任务事件标志
            //isr_evt_set(READER_B_BIT, HandleTaskDoor);
        }
		
		EXTI_ClearITPendingBit(EXTI_Line2);	
	}
}

//读头B的data1
void EXTI3_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line3) != RESET)
	{
		//Data1 -> 低电平表示1位1
        if(g_tReader.readerB.isDelayTimerOpen == FALSE)
        {
            OpenWiegandDelayTimerB();
        }
        
        g_tReader.readerB.buf[g_tReader.readerB.writeIndex++] = 1;
        if(g_tReader.readerB.writeIndex == 26)
        {
            g_tReader.readerB.writeIndex = 0;            
            
            /* 如果消息邮箱还有空间，向消息邮箱发送数据 */
            if (isr_mbx_check (&mailboxCardRX) != 0)
            {
                isr_mbx_send(&mailboxCardRX, &g_tReader.readerB);		
            }
            //发送任务事件标志
            //isr_evt_set(READER_B_BIT, HandleTaskDoor);
        }
		
		EXTI_ClearITPendingBit(EXTI_Line3);
	}
}

/***********************************************************
* 函数名：Check
* 功能  ：奇偶校验，确定读出数据是否正确 
* 输入  : 无
* 输出  ：0->校验成功,数据有效    1->校验失败,数据无效
**********************************************************/
uint8_t DataCheck(SingleReader_T *reader)
{
    uint8_t i=0;
    uint8_t j=0;
    uint8_t NumsOfOne=0;
    uint8_t len = 26;
    //偶校验
    for(i=1; i<13; i++)
    {
        if(reader->buf[i] == 1)
        {
            NumsOfOne++;
        }
    }
    if((NumsOfOne%2 == 0) && (reader->buf[0] == 1))
    {
            return 0;         
    }
    if((NumsOfOne%2 == 1) && (reader->buf[0] == 0))
    {
            return 0;         
    }
    //奇校验
    NumsOfOne = 0;
    for(i=13; i<25; i++)
    {
        if(reader->buf[i] == 1)
        {
            NumsOfOne++;
        }
    }
    if((NumsOfOne%2 == 1) && (reader->buf[len-1] == 1))
    {
            return 0;         
    }
    if((NumsOfOne%2 == 0) && (reader->buf[len-1] == 0))
    {
            return 0;         
    }
    
    //计算ID号
    reader->ID[0] = 0;
    reader->ID[1] = 0;
    reader->ID[2] = 0;
    for(i=1,j=7; i<9; i++,j--)
    {    
        reader->ID[0] += reader->buf[i]<<j;
    }
    for(i=9,j=7; i<17; i++,j--)
    {    
        reader->ID[1] += reader->buf[i]<<j;
    }
    for(i=17,j=7; i<25; i++,j--)
    {    
        reader->ID[2] += reader->buf[i]<<j;
    } 
    DEBUG(COM1, reader->ID, 3);
    return 1;
    
}

//打开韦根信号A超时定时器,TIM2定时器通道1
static void OpenWiegandDelayTimerA(void)
{
    if(g_tReader.readerA.isDelayTimerOpen == 0)
    {
        g_tReader.readerA.isDelayTimerOpen = 1;
        bsp_StartHardTimer(1, 200, (void *)HandlDelayTimerA);
    }
}

//打开韦根信号B超时定时器,TIM2定时器通道2
static void OpenWiegandDelayTimerB(void)
{
    if(g_tReader.readerB.isDelayTimerOpen == 0)
    {
        g_tReader.readerB.isDelayTimerOpen = 1;
        bsp_StartHardTimer(2, 200, (void *)HandlDelayTimerB);
    }
}

static void HandlDelayTimerA(void)
{   
    g_tReader.readerA.isDelayTimerOpen = 0;
    g_tReader.readerA.writeIndex = 0;
}

static void HandlDelayTimerB(void)
{   
    g_tReader.readerB.isDelayTimerOpen = 0;
    g_tReader.readerB.writeIndex = 0;
}

//检测读头是否连接
void DetectReader(void)
{
    if((GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)\
        && (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 1))//读取readerA
    {
        g_tReader.readerA.status = 1;
    }
    else g_tReader.readerA.status = 0;
    
    if((GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 1)\
        && (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == 1))//读取readerB
    {
        g_tReader.readerB.status = 1;
    }
    else g_tReader.readerB.status = 0;
}








