/*
*********************************************************************************************************
*
*	ģ������ : ������ģ�顣
*	�ļ����� : main.c(��������ȼ���ֵԽС���ȼ�Խ�ͣ������uCOS�෴)

*********************************************************************************************************
*/

#include "bsp.h"			/* �ײ�Ӳ������ */
/*
**********************************************************************************************************
											��������
**********************************************************************************************************
*/
//#define HEART_TIME 15//����������Ϊ15s

static void AppTaskCreate (void);//������start���������������
static void AppObjCreate (void);//������ʼ������������ʱ��

    
//������
__task void AppTaskReader(void);

__task void AppTaskFirst(void);
__task void AppTaskMulti(void);
__task void AppTaskInterLock(void);

__task void AppTaskKey(void);
__task void AppTaskButton(void);
__task void AppTaskNet(void);
__task void AppTaskStart(void);

/*
**********************************************************************************************************
											 ����
**********************************************************************************************************
*/
static uint64_t AppTaskReaderStk[512/8];   /* ����ջ */

static uint64_t AppTaskFirstStk[512/8];   /* ����ջ */
static uint64_t AppTaskMultiStk[512/8];     /* ����ջ */
static uint64_t AppTaskInterLockStk[512/8];   /* ����ջ */

static uint64_t AppTaskKeyStk[512/8];   /* ����ջ */
static uint64_t AppTaskButtonStk[512/8];   /* ����ջ */
static uint64_t AppTaskNetStk[512/8];     /* ����ջ */
static uint64_t AppTaskStartStk[512/8];   /* ����ջ */

/* ������ */
OS_TID HandleTaskReader = NULL;

OS_TID HandleTaskFirst = NULL;
OS_TID HandleTaskMulti = NULL;
OS_TID HandleTaskInterLock = NULL;

OS_TID HandleTaskKey = NULL;
OS_TID HandleTaskButton = NULL;
OS_TID HandleTaskNet = NULL;
OS_TID HandleTaskStart = NULL;

/* ����֧��8����Ϣ����Ϣ���� */
os_mbx_declare (mailboxCardRX, 8);

os_mbx_declare (mailboxCardFirst, 8);
os_mbx_declare (mailboxCardMulti, 8);
os_mbx_declare (mailboxCardInterLock, 8);

/* ��ʱ����� */
OS_ID  OneTimerA;//�����̵���A
OS_ID  OneTimerB;//�����̵���B

uint8_t FB_data;

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
	                  8,                        /* �������ȼ� */
	                  &AppTaskStartStk,         /* ����ջ */
	                  sizeof(AppTaskStartStk)); /* ����ջ��С����λ�ֽ��� */
	while(1);
}


/*
*********************************************************************************************************
*	�� �� ��: AppTaskReader
*	��Ҫ����: ���������ź�Զ�̿����¼�
*********************************************************************************************************
*/
__task void AppTaskReader(void)
{
    const uint16_t usMaxBlockTime = 100; /* �ӳ����� */
    while(1)
    {
        os_dly_wait(usMaxBlockTime);
    }//end of while
}

//�����׿�������
__task void AppTaskFirst(void)
{

    const uint16_t usMaxBlockTime = 100; /* �ӳ����� */
    while(1)
    {
           
        os_dly_wait(usMaxBlockTime);
    }//end of while
}

//������ؿ�������
__task void AppTaskMulti(void)
{

    const uint16_t usMaxBlockTime = 100; /* �ӳ����� */
    while(1)
    {
        os_dly_wait(usMaxBlockTime);
    }//end of while
}


//������������
__task void AppTaskInterLock(void)
{  
    const uint16_t usMaxBlockTime = 100; /* �ӳ����� */
    while(1)
    {
        os_dly_wait(usMaxBlockTime);
    }//end of while
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskButton
*	��Ҫ����: ���������ź�Զ�̿����¼�
*********************************************************************************************************
*/
__task void AppTaskButton(void)
{

    while(1)
    {
        
    }//end of while
}

//�����ͷ�����ɨ������
__task void AppTaskKey(void)
{
    while(1)
    {     
        
        os_dly_wait(100);
        
    }//end of while
           
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskNet
*	����˵��: ���紦�����񣬸��Ӱ���ɨ��,��ȡ��������	
*********************************************************************************************************
*/
__task void AppTaskNet(void)
{
    while(1)
    {    
        
        /* os_itv_wait���������ӳ٣�os_dly_wait������ӳ١�*/
		os_dly_wait(100);
    }//end of while
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskStart
*	��Ҫ����: ������������ι������DS1302ʱ�䣬����״̬
*********************************************************************************************************
*/

__task void AppTaskStart(void)
{
    
    /* ��ȡ��������ľ�� */
	HandleTaskStart = os_tsk_self();
    //ͨ��start�����ӵش�����������
	AppTaskCreate();
    /* ��������ͨ�Ż��� */
	AppObjCreate();
	
    while(1)
    {
        IWDG_Feed();//ι��
        
        os_dly_wait(15000);
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
    HandleTaskReader = os_tsk_create_user(AppTaskKey,              /* ������ */ 
                                       1,                       /* �������ȼ� */ 
                                       &AppTaskKeyStk,          /* ����ջ */
                                       sizeof(AppTaskKeyStk));  /* ����ջ��С����λ�ֽ��� */
    
    HandleTaskReader = os_tsk_create_user(AppTaskReader,              /* ������ */ 
                                       6,                       /* �������ȼ� */ 
                                       &AppTaskReaderStk,          /* ����ջ */
                                       sizeof(AppTaskReaderStk));  /* ����ջ��С����λ�ֽ��� */
    
    HandleTaskFirst = os_tsk_create_user(AppTaskFirst,              /* ������ */ 
                                       2,                       /* �������ȼ� */ 
                                       &AppTaskFirstStk,          /* ����ջ */
                                       sizeof(AppTaskFirstStk));  /* ����ջ��С����λ�ֽ��� */
    
    HandleTaskMulti = os_tsk_create_user(AppTaskMulti,              /* ������ */ 
	                                   5,                       /* �������ȼ� */ 
	                                   &AppTaskMultiStk,          /* ����ջ */
	                                   sizeof(AppTaskMultiStk));  /* ����ջ��С����λ�ֽ��� */
    
	HandleTaskInterLock = os_tsk_create_user(AppTaskInterLock,              /* ������ */ 
	                                   4,                       /* �������ȼ� */ 
	                                   &AppTaskInterLockStk,          /* ����ջ */
	                                   sizeof(AppTaskInterLockStk));  /* ����ջ��С����λ�ֽ��� */
    
    HandleTaskButton = os_tsk_create_user(AppTaskButton,              /* ������ */ 
	                                   3,                       /* �������ȼ� */ 
	                                   &AppTaskButtonStk,          /* ����ջ */
	                                   sizeof(AppTaskButtonStk));  /* ����ջ��С����λ�ֽ��� */
    
	HandleTaskNet = os_tsk_create_user(AppTaskNet,              /* ������ */ 
	                                   7,                       /* �������ȼ� */ 
	                                   &AppTaskNetStk,          /* ����ջ */
	                                   sizeof(AppTaskNetStk));  /* ����ջ��С����λ�ֽ��� */
}

/*
*********************************************************************************************************
*	�� �� ��: AppObjCreate
*	����˵��: ��������ͨ�Ż���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppObjCreate (void)
{
	/* ������Ϣ���� */
	os_mbx_init(&mailboxCardRX, sizeof(mailboxCardRX));
    
    os_mbx_init(&mailboxCardFirst, sizeof(mailboxCardFirst));
    os_mbx_init(&mailboxCardMulti, sizeof(mailboxCardMulti));
    os_mbx_init(&mailboxCardInterLock, sizeof(mailboxCardInterLock));
}


/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
