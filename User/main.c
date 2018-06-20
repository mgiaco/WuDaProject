/*
*********************************************************************************************************
*	�ļ����� : main.c(��������ȼ���ֵԽС���ȼ�Խ�ͣ������uCOS�෴)
*********************************************************************************************************
*/

#include "bsp.h"			/* �ײ�Ӳ������ */

static void AppTaskCreate (void);//������start���������������

/*����1�����Ź�
*����2��������3��lora���ݣ�ͨ���¼���־�飩���ͷ��������������
*/
//������
__task void AppTaskStart(void);
__task void AppTaskNet(void);
 
static uint64_t AppTaskStartStk[512/8];   /* ����ջ */
static uint64_t AppTaskNetStk[512/8];     /* ����ջ */
/* ������ */
OS_TID HandleTaskStart = NULL;
OS_TID HandleTaskNet = NULL;

/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: ��׼c������ڡ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int main(void) 
{	
	/* ��ʼ������ */
	bsp_Init();
    
	/* ������������ */
 	os_sys_init_user(AppTaskStart,             /* ������ */
	                  4,                        /* �������ȼ� */
	                  &AppTaskStartStk,         /* ����ջ */
	                  sizeof(AppTaskStartStk)); /* ����ջ��С����λ�ֽ��� */
	while(1);
}



/*
*********************************************************************************************************
*	�� �� ��: AppTaskStart
*	��Ҫ����: ����ι��
*********************************************************************************************************
*/

__task void AppTaskStart(void)
{  
    uint16_t ret_flags;
    /* ��ȡ��������ľ�� */
	HandleTaskStart = os_tsk_self();
    //ͨ��start�����ӵش�����������
	AppTaskCreate();
	
    while(1)
    {        
        //���û�ȴ���ʽ;�˳�ǰbitλ�����
        if(os_evt_wait_or(BIT_ALL, 0xFFFF) == OS_R_EVT)
        {
            ret_flags = os_evt_get();//����ֵΪ��Ӧ��bit
            
            switch(ret_flags)
            {
                case TASK_NET_BIT://���յ������¼���־λ                 
                    IWDG_Feed();//ι��
                    break; 
                
                default:
                    break;
            }//end of switch
            
        }//end of if
    }
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskNet
*	����˵��: ���紦������
*********************************************************************************************************
*/
__task void AppTaskNet(void)
{
    uint16_t ret_flags;
    const uint16_t usMaxBlockTime = 0x0FFF; /* �ӳ�����,4095ms */
    while(1)
    {
        //���û�ȴ���ʽ;�˳�ǰbitλ�����
        if(os_evt_wait_or(BIT_ALL, usMaxBlockTime) == OS_R_EVT)
        {
            ret_flags = os_evt_get();//����ֵΪ��Ӧ��bit
            
            switch(ret_flags)
            {
                case LORA_RECV_BIT://���յ����������                 
                    processCommand(g_tLora.buf, g_tLora.len);
                    break; 
                
                default:
                    break;
            }//end of switch
            
        }//end of if
        
        //�жϱ�־λ��ʱ��ִ�еĲ���
        //���Ź��������¼���־λ
        os_evt_set(TASK_NET_BIT, HandleTaskStart);//post
    }
    
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{ 
	HandleTaskNet = os_tsk_create_user(AppTaskNet,              /* ������ */ 
	                                   3,                       /* �������ȼ� */ 
	                                   &AppTaskNetStk,          /* ����ջ */
	                                   sizeof(AppTaskNetStk));  /* ����ջ��С����λ�ֽ��� */
}


/******************************************************/
