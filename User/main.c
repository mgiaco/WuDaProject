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
#define HEART_TIME 15//����������Ϊ15s

static void AppTaskCreate (void);//������start���������������
static void AppObjCreate (void);//������ʼ������������ʱ��

static void ReaderAlarm(enum ReaderOrButton_Enum type);//��ͷ����
static void SysLed(void);//ϵͳ״̬����˸
static void OpenTips(enum ReaderOrButton_Enum type);//������ʾ��
    
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
    
    #if Test
//    bsp_LedOn(1);
//    bsp_LedOn(2);
    #endif
    
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
    uint8_t type;
    SingleReader_T *readerMsg;
    const uint16_t usMaxBlockTime = 100; /* �ӳ����� */
    while(1)
    {
        //����Ҫ�Ĳ���
//        1.�ȴ�Τ������������������Ϣ����ת��һ�Σ�Ȼ���Լ��ٴ���
//        2.�ж϶��ֿ��Ź��ܵĿ��أ���SingleReader_T�ŵ�����������
//        3���׿�����ؿ������յ�������Ϣ�󣬽���ID���������ŷ�Ӧ����Ӽ������������ 
//        if(A)//�׿�
//        {ת����A}
//        if(B)//���ؿ�
//        {ת����B}
//        if(C)//����
//        {ת����C}
//        
//        type = searchID(readerMsg->ID);
//        if(type Ϊ������/���в�ȿ�/��)
//        {�ж�AB��ͷ��AB�̵����󣬿��Ż򱨾�}
        
        if(os_mbx_wait(&mailboxCardRX, (void *)&readerMsg, usMaxBlockTime) != OS_R_TMO)
        {
            
            if(g_tParam.systemCfg.multipleOpenCfg[1] == 1)//�׿�������
            {
                g_tRunInfo.isCardUsed = 1;
                os_mbx_send(&mailboxCardFirst, readerMsg, 100);//����Ϣ���䷢���ݣ������Ϣ�������ˣ��ȴ�100��ʱ�ӽ���
            }
            else if((g_tParam.systemCfg.multipleOpenCfg[2]&0x0F) == 1)//���ؿ�������//����λ��ʾ����
            {
                g_tRunInfo.isCardUsed = 1;
                os_mbx_send(&mailboxCardMulti, readerMsg, 100);//����Ϣ���䷢���ݣ������Ϣ�������ˣ��ȴ�100��ʱ�ӽ���
            }
            else if(g_tParam.systemCfg.multipleOpenCfg[0] == 1)//����������
            {
                g_tRunInfo.isCardUsed = 1;
                os_mbx_send(&mailboxCardInterLock, readerMsg, 100);//����Ϣ���䷢���ݣ������Ϣ�������ˣ��ȴ�100��ʱ�ӽ���
            }
            else g_tRunInfo.isCardUsed = 0;
            
            if(!g_tRunInfo.isCardUsed)
            {
                //�������������в�ȿ�����
                switch(readerMsg->readerID)
                {
                case e_READER_A:                    
                    g_tReader.dataCheck(readerMsg);
                    type = searchID(readerMsg->ID);//searchID�����ȶ����ã��ټ�⿨����û��

                    if(type == e_superCardID || type == e_superPasswordID)//�������򳬼�������㿪
                    {
                        if(g_tDoorStatus.doorA.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            //�ѿ����ϴ���������
                            SendDataToServer(0x06, e_READER_A, g_tReader.readerA.ID, 3);
                        }
                    }
                    if(type == e_threatCardID || type == e_threatPasswordID)//в�ȿ���в����
                    {
                        if(g_tDoorStatus.doorA.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            
                            SendDataToServer(0x22, e_READER_A, g_tReader.readerA.ID, 3);//�ϴ�������Ϣ
                        }
                    }
                    if(type==e_keyBoardID || type==e_generalCardID || type==e_fingerID)
                    {
                        if(g_tDoorStatus.doorA.switcherStatus == NC)
                        {   
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            OpenTips(e_READER_A);
                            
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            //�ѿ����ϴ���������
                            //add3-8 ��дλ0��ʾA��1��ʾB
                            SendDataToServer(0x06, e_READER_A, g_tReader.readerA.ID, 3);
                        }
                    }
                    if(type == 0xFF)
                    {
                        //�ѿ����ϴ���������
                        SendDataToServer(0x06, e_READER_A, g_tReader.readerA.ID, 3);
                    }
                    break;
                
                case e_READER_B:
                    g_tReader.dataCheck(readerMsg);
                    type = searchID(readerMsg->ID);//searchID�����ȶ����ã��ټ�⿨����û��

                    if(type == e_superCardID || type == e_superPasswordID)//�������򳬼�������㿪
                    {
                        if(g_tDoorStatus.doorB.switcherStatus == NC)
                        { 
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            OneTimerB = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            //�ѿ����ϴ���������
                            SendDataToServer(0x06, e_READER_B, g_tReader.readerB.ID, 3);
                        }
                    }
                    if(type == e_threatCardID || type == e_threatPasswordID)//в�ȿ���в����
                    {
                        if(g_tDoorStatus.doorB.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                            
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            
                            SendDataToServer(0x22, e_READER_B, g_tReader.readerB.ID, 3);//�ϴ�������Ϣ
                        }
                    }
                    if(type==e_keyBoardID || type==e_generalCardID || type==e_fingerID)
                    {
                        if(g_tDoorStatus.doorB.switcherStatus == NC)
                        {              
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            
                            OneTimerB = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 3);
                            OpenTips(e_READER_B);
                            
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            //�ѿ����ϴ���������
                            SendDataToServer(0x06, e_READER_B, g_tReader.readerB.ID, 3);
                        }
                    }
                    if(type == 0xFF)
                    {
                        //�ѿ����ϴ���������
                        SendDataToServer(0x06, e_READER_B, g_tReader.readerB.ID, 3);
                    }
                    break;
                    
                
                default:
                    break;
            }//end of switch
        }//end of if
           
        //��תϵͳ״̬��
        SysLed();
            
        }//end of if
    }//end of while
}

//�����׿�������
__task void AppTaskFirst(void)
{
    uint8_t type;
    SingleReader_T *readerMsg;
    const uint16_t usMaxBlockTime = 100; /* �ӳ����� */
    while(1)
    {
        if(os_mbx_wait(&mailboxCardFirst, (void *)&readerMsg, usMaxBlockTime) != OS_R_TMO)
        {
            switch(readerMsg->readerID)
            {
                case e_READER_A:
                    g_tReader.dataCheck(readerMsg);
                    type = searchID(readerMsg->ID);//searchID�����ȶ����ã��ټ�⿨����û��
                    if(type == e_firstCardID)//�׿�
                    {
                        g_tRunInfo.firstCardStatus = 1;//�׿���ˢ�����������Լ���ˢ
                        if(g_tDoorStatus.doorA.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            //�ѿ����ϴ���������
                            SendDataToServer(0x06, 0, g_tReader.readerA.ID, 3);
                        }
                    }
                    //Ȩ�޸ߵĿ��Ŵ���
                    if(type == e_superCardID || type == e_superPasswordID)//�������򳬼�������㿪
                    {
                        if(g_tDoorStatus.doorA.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            //�ѿ����ϴ���������
                            SendDataToServer(0x06, e_READER_A, g_tReader.readerA.ID, 3);
                        }
                    }
                    if(type == e_threatCardID || type == e_threatPasswordID)//в�ȿ���в����
                    {
                        if(g_tDoorStatus.doorA.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            
                            SendDataToServer(0x22, e_READER_A, g_tReader.readerA.ID, 3);//�ϴ�������Ϣ
                        }
                    }
                    //�����׿�����
                    if(g_tRunInfo.firstCardStatus == 1)
                    {
                        if(type==e_keyBoardID || type==e_generalCardID || type==e_fingerID)
                        {
                            if(g_tDoorStatus.doorA.switcherStatus == NC)
                            {
                                g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                                OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                                
                                //�洢��¼��spi
                                storeRecord(readerMsg->ID, e_READER_A);
                                //�ѿ����ϴ���������
                                SendDataToServer(0x06, 0, g_tReader.readerA.ID, 3);
                            }
                        }
                    }
                    break;
                
                case e_READER_B:
                    g_tReader.dataCheck(readerMsg);
                    type = searchID(readerMsg->ID);//searchID�����ȶ����ã��ټ�⿨����û��
                    if(type == e_firstCardID)//�׿�
                    {
                        g_tRunInfo.firstCardStatus = 1;//�׿���ˢ�����������Լ���ˢ
                        if(g_tDoorStatus.doorB.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            OneTimerB = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                                                              
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            //�ѿ����ϴ���������
                            SendDataToServer(0x06, 0, g_tReader.readerB.ID, 3);
                        }
                    }
                    //Ȩ�޸ߵĿ��Ŵ���
                    if(type == e_superCardID || type == e_superPasswordID)//�������򳬼�������㿪
                    {
                        if(g_tDoorStatus.doorB.switcherStatus == NC)
                        { 
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            OneTimerB = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            //�ѿ����ϴ���������
                            SendDataToServer(0x06, e_READER_B, g_tReader.readerB.ID, 3);
                        }
                    }
                    if(type == e_threatCardID || type == e_threatPasswordID)//в�ȿ���в����
                    {
                        if(g_tDoorStatus.doorB.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                            
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            
                            SendDataToServer(0x22, e_READER_B, g_tReader.readerB.ID, 3);//�ϴ�������Ϣ
                        }
                    }
                    //�����׿�����
                    if(g_tRunInfo.firstCardStatus == 1)
                    {
                        if(type==e_keyBoardID || type==e_generalCardID || type==e_fingerID)
                        {
                            if(g_tDoorStatus.doorB.switcherStatus == NC)
                            {
                                g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                                OneTimerB = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                                                                  
                                //�洢��¼��spi
                                storeRecord(readerMsg->ID, e_READER_B);
                                //�ѿ����ϴ���������
                                SendDataToServer(0x06, 0, g_tReader.readerB.ID, 3);
                            }
                        }
                    }                 
                    break;
                
                default:
                    break;
            }//end of switch
        }//if
    }//end of while
}

//������ؿ�������
__task void AppTaskMulti(void)
{
    uint8_t type;
    SingleReader_T *readerMsg;
    const uint16_t usMaxBlockTime = 100; /* �ӳ����� */
    while(1)
    {
        if(os_mbx_wait(&mailboxCardMulti, (void *)&readerMsg, usMaxBlockTime) != OS_R_TMO)
        {
            switch(readerMsg->readerID)
            {
                case e_READER_A:
                    g_tReader.dataCheck(readerMsg);
                    type = searchID(readerMsg->ID);//searchID�����ȶ����ã��ټ�⿨����û��
                    if(type == e_multipleCardID)//�׿�
                    {
                        //TODO:����ֱ���ۼӼ���
                        g_tRunInfo.multipleCardStatus++;//����ˢ���ۼӼ���
                    }
                    //Ȩ�޸ߵĿ��Ŵ���
                    if(type == e_superCardID || type == e_superPasswordID)//�������򳬼�������㿪
                    {
                        if(g_tDoorStatus.doorA.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            //�ѿ����ϴ���������
                            SendDataToServer(0x06, e_READER_A, g_tReader.readerA.ID, 3);
                        }
                    }
                    if(type == e_threatCardID || type == e_threatPasswordID)//в�ȿ���в����
                    {
                        if(g_tDoorStatus.doorA.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            
                            SendDataToServer(0x22, e_READER_A, g_tReader.readerA.ID, 3);//�ϴ�������Ϣ
                        }
                    }
                    //���϶��ؿ�����
                    if(g_tRunInfo.multipleCardStatus == (g_tParam.systemCfg.multipleOpenCfg[2]>>4))
                    {
                        
                        if(g_tDoorStatus.doorA.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            //�ѿ����ϴ���������
                            SendDataToServer(0x06, 0, g_tReader.readerA.ID, 3);      
                        }
                        g_tRunInfo.multipleCardStatus = 0;//���ź�λ״̬
                    }                 
                    break;
                
                case e_READER_B:
                    g_tReader.dataCheck(readerMsg);
                    type = searchID(readerMsg->ID);//searchID�����ȶ����ã��ټ�⿨����û��
                    if(type == e_multipleCardID)//�׿�
                    {
                        g_tRunInfo.multipleCardStatus++;//����ˢ���ۼӼ���
                    }
                    //Ȩ�޸ߵĿ��Ŵ���
                    if(type == e_superCardID || type == e_superPasswordID)//�������򳬼�������㿪
                    {
                        if(g_tDoorStatus.doorB.switcherStatus == NC)
                        { 
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            OneTimerB = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            //�ѿ����ϴ���������
                            SendDataToServer(0x06, e_READER_B, g_tReader.readerB.ID, 3);
                        }
                    }
                    if(type == e_threatCardID || type == e_threatPasswordID)//в�ȿ���в����
                    {
                        if(g_tDoorStatus.doorB.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                            
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            
                            SendDataToServer(0x22, e_READER_B, g_tReader.readerB.ID, 3);//�ϴ�������Ϣ
                        }
                    }
                    //���϶��ؿ�����
                    if(g_tRunInfo.multipleCardStatus == (g_tParam.systemCfg.multipleOpenCfg[2]>>4))
                    {
                        
                        if(g_tDoorStatus.doorB.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            OneTimerB = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                            
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            //�ѿ����ϴ���������
                            SendDataToServer(0x06, 0, g_tReader.readerB.ID, 3);
                        }
                        g_tRunInfo.multipleCardStatus = 0;//���ź�λ״̬
                    }
                    break;
                
                default:
                    break;
            }//end of switch
        }//if
    }//end of while
}


//������������
__task void AppTaskInterLock(void)
{
    uint8_t type;
    SingleReader_T *readerMsg;
    const uint16_t usMaxBlockTime = 100; /* �ӳ����� */
    while(1)
    {
        if(os_mbx_wait(&mailboxCardInterLock, (void *)&readerMsg, usMaxBlockTime) != OS_R_TMO)
        {
            switch(readerMsg->readerID)
            {
                case e_READER_A:
                    g_tReader.dataCheck(readerMsg);
                    type = searchID(readerMsg->ID);//searchID�����ȶ����ã��ټ�⿨����û��
                    //Ȩ�޸ߵĿ��Ŵ���
                    if(type == e_superCardID || type == e_superPasswordID)//�������򳬼�������㿪
                    {
                        if(g_tDoorStatus.doorA.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            //�ѿ����ϴ���������
                            SendDataToServer(0x06, e_READER_A, g_tReader.readerA.ID, 3);
                        }
                    }
                    if(type == e_threatCardID || type == e_threatPasswordID)//в�ȿ���в����
                    {
                        if(g_tDoorStatus.doorA.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            
                            SendDataToServer(0x22, e_READER_A, g_tReader.readerA.ID, 3);//�ϴ�������Ϣ
                        }
                    }
                    //�жϻ�������
                    if(type==e_keyBoardID || type==e_generalCardID || type==e_fingerID)
                    {
                        //��A��ʱ��B�Ų����Ǵ򿪵�
                        if(g_tDoorStatus.doorA.switcherStatus == NC && g_tDoorStatus.doorB.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            //�ѿ����ϴ���������
                            SendDataToServer(0x06, 0, g_tReader.readerA.ID, 3);
                        }
                    }
                    break;
                
                case e_READER_B:
                    g_tReader.dataCheck(readerMsg);
                    type = searchID(readerMsg->ID);//searchID�����ȶ����ã��ټ�⿨����û��
                    //Ȩ�޸ߵĿ��Ŵ���
                    if(type == e_superCardID || type == e_superPasswordID)//�������򳬼�������㿪
                    {
                        if(g_tDoorStatus.doorB.switcherStatus == NC)
                        { 
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            OneTimerB = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            //�ѿ����ϴ���������
                            SendDataToServer(0x06, e_READER_B, g_tReader.readerB.ID, 3);
                        }
                    }
                    if(type == e_threatCardID || type == e_threatPasswordID)//в�ȿ���в����
                    {
                        if(g_tDoorStatus.doorB.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                            
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            
                            SendDataToServer(0x22, e_READER_B, g_tReader.readerB.ID, 3);//�ϴ�������Ϣ
                        }
                    }
                    //�жϻ�������
                    if(type==e_keyBoardID || type==e_generalCardID || type==e_fingerID)
                    {
                        //��B��ʱ��B�Ų����Ǵ򿪵�
                        if(g_tDoorStatus.doorA.switcherStatus == NC && g_tDoorStatus.doorB.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            OneTimerB = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                            
                            //�洢��¼��spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            //�ѿ����ϴ���������
                            SendDataToServer(0x06, 0, g_tReader.readerB.ID, 3);
                        } 
                    }                        
                    break;
                
                default:
                    break;
            }//end of switch
        }//if
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
    uint16_t ret_flags;
    SingleRelation_T t_remoteOpen;
    while(1)
    {
        //���û�ȴ���ʽ;�˳�ǰbitλ�����
        if(os_evt_wait_or(BIT_ALL, 0xFFFF) == OS_R_EVT)
        {
            ret_flags = os_evt_get();//����ֵΪ��Ӧ��bit
            SysLed();
            
            switch(ret_flags)
            {
                case REMOTE_OPEN_BIT://Զ�̿���,���ð������ŷ�ʽ                   
                    t_remoteOpen.button_switcher = g_tRunInfo.remoteOpen;
                    g_tDoorStatus.openDoor(&t_remoteOpen, e_BUTTON_A);
                    OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 0);  /* ��ʱ1000*n��ϵͳʱ�ӽ��� ��0�ǻص������Ĳ��������������ֲ�ͬ�Ķ�ʱ�� */
                    break;
                
                case BUTTON_A_BIT://����A
                    if(g_tDoorStatus.doorA.switcherStatus == NC)
                    {
                        g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_BUTTON_A);
                        OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 4);  /* ��ʱ1000*n��ϵͳʱ�ӽ��� ��1�ǻص������Ĳ��������������ֲ�ͬ�Ķ�ʱ�� */
                    }
                    break;
                
                case BUTTON_B_BIT://����B
                    if(g_tDoorStatus.doorB.switcherStatus == NC)
                    {
                        g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_BUTTON_B);
                        OneTimerB = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);  /* ��ʱ1000*n��ϵͳʱ�ӽ��� ��2�ǻص������Ĳ��������������ֲ�ͬ�Ķ�ʱ�� */
                    }
                    break; 
                
                default:
                    break;
            }//end of switch
            
            
        //��תϵͳ״̬��
        SysLed();
            
        }//end of if
    }
}

//�����ͷ�����ɨ������
__task void AppTaskKey(void)
{
    static uint16_t timesA, timesB;
    while(1)
    {
        
        DetectReader();//����ͷ����״̬
        g_tDoorStatus.readFeedBack();//��ȡ��������
        //����ŷ����ǿ����Ҽ̵����ǹصģ���ôN�����ñ�����ֱ��״̬��
        //���ݼ�⵽�Ķ�ͷ״̬����������δ���ӵģ�ֻ�������ϵı���
        if(g_tReader.readerA.status == 1)
        {
            if(g_tDoorStatus.doorA.feedBackStatus == NO && g_tDoorStatus.doorA.switcherStatus == NC)
            {
                timesA++;
                //TODO:���waitTime���0���򲻱���
                if((g_tParam.systemCfg.waitTime!=0) && (timesA >= g_tParam.systemCfg.waitTime*10))
                {
                    if(timesA == g_tParam.systemCfg.waitTime*10*25)
                    {
                        timesA = g_tParam.systemCfg.waitTime*10;//����times���
                    }
                    //alram������os_dly�ͷ�ת
                    //���ö�ͷ�ϵķ�������led
                    ReaderAlarm(e_READER_A);
                }
            }
            else 
            {
                timesA = 0;
            }
        }
        
        if(g_tReader.readerB.status == 1)
        {
            if(g_tDoorStatus.doorB.feedBackStatus == NO && g_tDoorStatus.doorB.switcherStatus == NC)
            {
                timesB++;
                if((g_tParam.systemCfg.waitTime!=0) && (timesB >= g_tParam.systemCfg.waitTime*10))
                {
                    if(timesB == g_tParam.systemCfg.waitTime*10*25)
                    {
                        timesB = g_tParam.systemCfg.waitTime*10;//����times���
                    }
                    //alram������os_dly�ͷ�ת
                    //���ö�ͷ�ϵķ�������led
                    ReaderAlarm(e_READER_B);
                }
            }
            else 
            {
                timesB = 0;
            }
        }
        
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
    static uint16_t n;
    static uint8_t data;
    static const uint8_t netDelayTime = 50;//��������ÿ50msһ��
    
    static uint16_t heartNum = (HEART_TIME*1000)/netDelayTime;//��������ʱ����
    
    while(1)
    {
        
        KEY_Scan();//�������
        //��ȡ����״̬
        wiz_read_buf(PHYCFGR, &data, 1);//��ȡphy���ж������Ƿ���
        if(data & 0x01 == 1)
        {
            g_tNetData.status = e_Link;//���߲����

        }
        else g_tNetData.status = e_NoLink;//����û���
        
        if(g_tNetData.status == e_Link)
        {
            /*Socket״̬����MCUͨ����Sn_SR(0)��ֵ�����ж�SocketӦ�ô��ں���״̬*/
            switch(getSn_SR(0))	// ��ȡsocket0��״̬
            {
            case SOCK_UDP:		// Socket���ڳ�ʼ�����(��)״̬
                if(getSn_IR(0) & Sn_IR_RECV)
                {
                    setSn_IR(0, Sn_IR_RECV);// Sn_IR��RECVλ��1
                }
                //���ݽ���
                g_tNetData.len = getSn_RX_RSR(0);
                if((g_tNetData.len>0) && (g_tNetData.len <= (DATA_LEN+13+8)))
                { 
                    g_tNetData.len -= 8;//ȥ��8�ֽڳ��ȵ�ipͷ��Ϣ
                    // W5500��������Զ����λ�������ݣ���ͨ��SPI���͸�MCU
                    recvfrom(0, g_tNetData.buf, g_tNetData.len, g_tParam.netCfg.server_ip ,&g_tParam.netCfg.server_port);
                    DEBUG(COM1, g_tNetData.buf, g_tNetData.len);
                    processCommand(g_tNetData.buf, g_tNetData.len);
                }
                break;
                
            case SOCK_CLOSED:// Socket���ڹر�״̬
                socket(0, Sn_MR_UDP, g_tParam.netCfg.local_port, 0);	// ��Socket0��������ΪUDPģʽ����һ�����ض˿�
                break;
            
            default:
                break;         
            }//end of switch
            
            if(n >= heartNum)//�ȵ���������ﵽһ����������������
            {
                n = 0;
                FB_data = g_tDoorStatus.doorA.feedBackStatus;
                FB_data += g_tDoorStatus.doorB.feedBackStatus << 4;
                SendDataToServer(0, 0, &FB_data, 1);//����������
            }
            n++;
        }
        
        /* os_itv_wait���������ӳ٣�os_dly_wait������ӳ١�*/
		os_dly_wait(netDelayTime);
    }
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
        
        ds1302_readtime(g_tRunInfo.time, 5);//��ȡʱ��

        //�ж��Ƿ�0�㣬��Ҫ��λ�׿����߶��ؿ���״̬
        if(g_tRunInfo.time[3] == 0 && (g_tRunInfo.firstCardStatus != 0 || g_tRunInfo.multipleCardStatus != 0))
        {
            //״̬��0
            g_tRunInfo.firstCardStatus = 0;
            g_tRunInfo.multipleCardStatus = 0;
        }
 
        //��תϵͳ״̬��
        SysLed();
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

//����
static void ReaderAlarm(enum ReaderOrButton_Enum type)
{
    alarmOn(type);
    os_dly_wait(500);
    alarmOff(type);
}

//ϵͳ״̬����˸
static void SysLed(void)
{
    bsp_LedToggle(3);
    os_dly_wait(500);
    bsp_LedToggle(3);
}

//������ʾ��
static void OpenTips(enum ReaderOrButton_Enum type)
{
//    uint8_t delayTime = 50;
//    os_dly_wait(200);
//    
//    alarmOn(type);
//    os_dly_wait(delayTime);
//    alarmOff(type);
//    
//    os_dly_wait(delayTime);
//    
//    alarmOn(type);
//    os_dly_wait(delayTime);
//    alarmOff(type);
//    
//    os_dly_wait(delayTime);
//    
//    alarmOn(type);
//    os_dly_wait(delayTime);
//    alarmOff(type);
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
