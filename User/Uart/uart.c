#include "bsp.h"

Gps_T g_tGps;
Lora_T g_tLora;

/*
*********************************************************************************************************
*	函 数 名: InitHardUart
*	功能说明: 配置串口的硬件参数（波特率，数据位，停止位，起始位，校验位，中断使能）
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void uartInit(void)
{       
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    g_tGps.len = 0;
    g_tGps.at_state = at_statIdle;
    
    g_tLora.len = 0;
    g_tLora.at_state = at_statIdle;
    
    /* 串口1 TX = PA9   RX = PA10 */
    /* 第1步：打开GPIO和USART部件的时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* 第2步：将USART Tx的GPIO配置为推挽复用模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* 第3步：将USART Rx的GPIO配置为浮空输入模式
		由于CPU复位后，GPIO缺省都是浮空输入模式，因此下面这个步骤不是必须的
		但是，我还是建议加上便于阅读，并且防止其它地方修改了这个口线的设置参数
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* 第4步： 配置串口硬件参数 */
	USART_InitStructure.USART_BaudRate = UART1_BAUD;	/* 波特率 */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	/* 使能接收中断 */
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		注意: 不要在此处打开发送中断
		发送中断使能在SendUart()函数打开
	*/
	USART_Cmd(USART1, ENABLE);		/* 使能串口 */

	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
		如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */
    
    /* 串口3 TX = PB10   RX = PB11 */
    /* 第1步： 开启GPIO和UART时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	/* 第2步：将USART Tx的GPIO配置为推挽复用模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* 第3步：将USART Rx的GPIO配置为浮空输入模式
		由于CPU复位后，GPIO缺省都是浮空输入模式，因此下面这个步骤不是必须的
		但是，我还是建议加上便于阅读，并且防止其它地方修改了这个口线的设置参数
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* 第4步： 配置串口硬件参数 */
	USART_InitStructure.USART_BaudRate = UART3_BAUD;	/* 波特率 */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	/* 使能接收中断 */
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		注意: 不要在此处打开发送中断
		发送中断使能在SendUart()函数打开
	*/
	USART_Cmd(USART3, ENABLE);		/* 使能串口 */

	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
		如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(USART3, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */

    //配置串口硬件中断
    /* 使能串口1中断 */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 	= 0x00;//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
    /* 使能串口3中断t */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;//抢占优先级 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 	= 0x00;//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//USART1(GPS)中断服务程序
void USART1_IRQHandler(void)
{
	/* 处理接收中断  */
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		/* 从串口接收数据寄存器读取数据存放到接收FIFO */
		g_tGps.temp = USART_ReceiveData(USART1);
        switch(g_tGps.at_state)
        {
            case at_statIdle:
                g_tGps.atHead[0] = g_tGps.atHead[1];
                g_tGps.atHead[1] = g_tGps.temp;
                //解析GPGLL的LL，作为报文头
                if(('L' == g_tGps.atHead[0]) && ('L' == g_tGps.atHead[1]))
                {
                    g_tGps.at_state = at_statRecving;
                    g_tGps.buf[0] = 'L';
                    g_tGps.buf[1] = 'L';
                    g_tGps.len = 2;     
                    //TODO:打开超时定时器
                }
                break;
            
            case at_statRecving:
                g_tGps.buf[g_tGps.len++] = g_tGps.temp;
                //检查长度和报文尾
                if((g_tGps.len > 8) && ('\r' == g_tGps.buf[g_tGps.len-2]) && ('\n' == g_tGps.buf[g_tGps.len-1]))
                {
                    //TODO：直接在这里解析出经纬度和时间算了
                    if('A' == g_tGps.buf[g_tGps.len-8])//有信号时,位置有效状态是A
                    {
                        g_tGps.status = 'A';
                        //保存原始数据，等发送命令时再做进一步的处理
                        memcpy(g_tGps.latitude, &g_tGps.buf[3], sizeof(g_tGps.latitude));
                        memcpy(g_tGps.longiitude, &g_tGps.buf[17], sizeof(g_tGps.longiitude));
                        memcpy(g_tGps.time, &g_tGps.buf[g_tGps.len-19], sizeof(g_tGps.time));
                    }
                    else if('V' == g_tGps.buf[g_tGps.len-8])//没有信号时,位置有效状态是V
                    {
                        g_tGps.status = 'V';
                    }
                }
                else if((g_tGps.len-1) == GPS_DATA_LEN)//Data full, reset at_state!
                {
	    		  g_tGps.len = 0;
	    		  memset(g_tGps.atHead, 0, sizeof(g_tGps.atHead));
	    		  g_tGps.at_state = at_statIdle;
                }
                break;
            
            default:
                break;
        }//end of switch
	}//end of if
}

//USART3(Lora)中断服务程序
void USART3_IRQHandler(void)
{
	/* 处理接收中断  */
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		/* 从串口接收数据寄存器读取数据存放到接收FIFO */
		g_tLora.temp = USART_ReceiveData(USART3);
        switch(g_tLora.at_state)
        {
            case at_statIdle:
                g_tLora.atHead[0] = g_tLora.atHead[1];
                g_tLora.atHead[1] = g_tLora.temp;
                //解析GPGLL的LL，作为报文头
                if((0xA5 == g_tGps.atHead[0]) && (0xA5 == g_tLora.atHead[1]))
                {
                    g_tLora.at_state = at_statRecving;
                    g_tLora.buf[0] = 0xA5;
                    g_tLora.buf[1] = 0xA5;
                    g_tLora.len = 2;     
                    //TODO:打开超时定时器
                }
                break;
            
            case at_statRecving:
                g_tLora.buf[g_tLora.len++] = g_tLora.temp;
                //检查长度和报文尾，成功则发送事件标志
                if((g_tLora.len > 8) && (0x5A == g_tLora.buf[g_tLora.len-2]) && (0x5A == g_tLora.buf[g_tLora.len-1]))
                {
                    //TODO：发送事件标志，让任务来处理

                }
                else if((g_tLora.len-1) == LORA_DATA_LEN)//Data full, reset at_state!
                {
	    		  g_tLora.len = 0;
	    		  memset(g_tLora.atHead, 0, sizeof(g_tLora.atHead));
	    		  g_tLora.at_state = at_statIdle;
                }
                break;
            
            default:
                break;
        }//end of switch
	}//end of if
}

//通过串口3发送数据
void LoraSendData(uint8_t *data, uint8_t len)
{
    uint8_t num;
    while(num<len)
    {
        USART_SendData(USART3, data[num]);
        num++;
        while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    }
}

//-----------------------------------------------

