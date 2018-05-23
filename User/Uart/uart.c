#include "bsp.h"

Gps_T g_tGps;
Lora_T g_tLora;

/*
*********************************************************************************************************
*	�� �� ��: InitHardUart
*	����˵��: ���ô��ڵ�Ӳ�������������ʣ�����λ��ֹͣλ����ʼλ��У��λ���ж�ʹ�ܣ�
*	��    ��: ��
*	�� �� ֵ: ��
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
    
    /* ����1 TX = PA9   RX = PA10 */
    /* ��1������GPIO��USART������ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* ��2������USART Tx��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ��3������USART Rx��GPIO����Ϊ��������ģʽ
		����CPU��λ��GPIOȱʡ���Ǹ�������ģʽ���������������費�Ǳ����
		���ǣ��һ��ǽ�����ϱ����Ķ������ҷ�ֹ�����ط��޸���������ߵ����ò���
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* ��4���� ���ô���Ӳ������ */
	USART_InitStructure.USART_BaudRate = UART1_BAUD;	/* ������ */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		ע��: ��Ҫ�ڴ˴��򿪷����ж�
		�����ж�ʹ����SendUart()������
	*/
	USART_Cmd(USART1, ENABLE);		/* ʹ�ܴ��� */

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
    
    /* ����3 TX = PB10   RX = PB11 */
    /* ��1���� ����GPIO��UARTʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	/* ��2������USART Tx��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* ��3������USART Rx��GPIO����Ϊ��������ģʽ
		����CPU��λ��GPIOȱʡ���Ǹ�������ģʽ���������������費�Ǳ����
		���ǣ��һ��ǽ�����ϱ����Ķ������ҷ�ֹ�����ط��޸���������ߵ����ò���
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* ��4���� ���ô���Ӳ������ */
	USART_InitStructure.USART_BaudRate = UART3_BAUD;	/* ������ */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		ע��: ��Ҫ�ڴ˴��򿪷����ж�
		�����ж�ʹ����SendUart()������
	*/
	USART_Cmd(USART3, ENABLE);		/* ʹ�ܴ��� */

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(USART3, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */

    //���ô���Ӳ���ж�
    /* ʹ�ܴ���1�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//��ռ���ȼ� 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 	= 0x00;//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
    /* ʹ�ܴ���3�ж�t */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;//��ռ���ȼ� 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 	= 0x00;//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//USART1(GPS)�жϷ������
void USART1_IRQHandler(void)
{
	/* ��������ж�  */
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		/* �Ӵ��ڽ������ݼĴ�����ȡ���ݴ�ŵ�����FIFO */
		g_tGps.temp = USART_ReceiveData(USART1);
        switch(g_tGps.at_state)
        {
            case at_statIdle:
                g_tGps.atHead[0] = g_tGps.atHead[1];
                g_tGps.atHead[1] = g_tGps.temp;
                //����GPGLL��LL����Ϊ����ͷ
                if(('L' == g_tGps.atHead[0]) && ('L' == g_tGps.atHead[1]))
                {
                    g_tGps.at_state = at_statRecving;
                    g_tGps.buf[0] = 'L';
                    g_tGps.buf[1] = 'L';
                    g_tGps.len = 2;     
                    //TODO:�򿪳�ʱ��ʱ��
                }
                break;
            
            case at_statRecving:
                g_tGps.buf[g_tGps.len++] = g_tGps.temp;
                //��鳤�Ⱥͱ���β
                if((g_tGps.len > 8) && ('\r' == g_tGps.buf[g_tGps.len-2]) && ('\n' == g_tGps.buf[g_tGps.len-1]))
                {
                    //TODO��ֱ���������������γ�Ⱥ�ʱ������
                    if('A' == g_tGps.buf[g_tGps.len-8])//���ź�ʱ,λ����Ч״̬��A
                    {
                        g_tGps.status = 'A';
                        //����ԭʼ���ݣ��ȷ�������ʱ������һ���Ĵ���
                        memcpy(g_tGps.latitude, &g_tGps.buf[3], sizeof(g_tGps.latitude));
                        memcpy(g_tGps.longiitude, &g_tGps.buf[17], sizeof(g_tGps.longiitude));
                        memcpy(g_tGps.time, &g_tGps.buf[g_tGps.len-19], sizeof(g_tGps.time));
                    }
                    else if('V' == g_tGps.buf[g_tGps.len-8])//û���ź�ʱ,λ����Ч״̬��V
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

//USART3(Lora)�жϷ������
void USART3_IRQHandler(void)
{
	/* ��������ж�  */
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		/* �Ӵ��ڽ������ݼĴ�����ȡ���ݴ�ŵ�����FIFO */
		g_tLora.temp = USART_ReceiveData(USART3);
        switch(g_tLora.at_state)
        {
            case at_statIdle:
                g_tLora.atHead[0] = g_tLora.atHead[1];
                g_tLora.atHead[1] = g_tLora.temp;
                //����GPGLL��LL����Ϊ����ͷ
                if((0xA5 == g_tGps.atHead[0]) && (0xA5 == g_tLora.atHead[1]))
                {
                    g_tLora.at_state = at_statRecving;
                    g_tLora.buf[0] = 0xA5;
                    g_tLora.buf[1] = 0xA5;
                    g_tLora.len = 2;     
                    //TODO:�򿪳�ʱ��ʱ��
                }
                break;
            
            case at_statRecving:
                g_tLora.buf[g_tLora.len++] = g_tLora.temp;
                //��鳤�Ⱥͱ���β���ɹ������¼���־
                if((g_tLora.len > 8) && (0x5A == g_tLora.buf[g_tLora.len-2]) && (0x5A == g_tLora.buf[g_tLora.len-1]))
                {
                    //TODO�������¼���־��������������

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

//ͨ������3��������
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

