#include "bsp.h"
//��ַ�궨��
//��һҳ
#define ADDR_LOCAL_IP 1
#define ADDR_LOCAL_PORT 5
#define ADDR_SERVER_IP 7
#define ADDR_SERVER_PORT 11
#define ADDR_OPEN_TIME 13
#define ADDR_RELATION_READER_A 14
#define ADDR_RELATION_READER_B 15
#define ADDR_RELATION_BUTTON_A 16
#define ADDR_RELATION_BUTTON_B 17
#define ADDR_WAIT_TIME 18
#define NEXT_START_SECTOR_H 28
#define NEXT_START_SECTOR_L 29
#define NEXT_START_ADDR_H 30
#define NEXT_START_ADDR_L 31
//�ڶ�ҳ
#define FIRST_CARD_ID       EE_PAGE_SIZE+0
#define IS_OPEN_FIRST_CARD  EE_PAGE_SIZE+3
#define SUPER_CARD_ID       EE_PAGE_SIZE+4
#define IS_OPEN_SUPER_CARD  EE_PAGE_SIZE+7
#define THREAT_CARD_ID      EE_PAGE_SIZE+8//�������3�ſ�
#define IS_OPEN_THREAT_CARD EE_PAGE_SIZE+17
#define IS_OPEN_INTERLOCK   EE_PAGE_SIZE+18
#define THREAT_PASSWORD_ID  EE_PAGE_SIZE+19
#define IS_OPEN_THREAT_PASSWORD EE_PAGE_SIZE+22
#define SUPER_PASSWORD_ID   EE_PAGE_SIZE+23
#define IS_OPEN_SUPER_PASSWORD EE_PAGE_SIZE+26
//����ҳ
#define IS_OPEN_MULTIPLE_CARD EE_PAGE_SIZE*2+0
#define MULTIPLE_CARD_ID      EE_PAGE_SIZE*2+1//�����10��
//����ҳ���Ժ�
#define IS_OPEN_FINGER      EE_PAGE_SIZE*3+0
#define KEY_BOARD_ID        EE_PAGE_SIZE*3+1
#define FINGER_ID           EE_PAGE_SIZE*3+4//�����500��ָ��,ռ1500�ֽ�
#define GENERAL_CARD_ID     EE_PAGE_SIZE*3+1504//�����500������
#define END_ADDR            EE_PAGE_SIZE*3+3004//�ܹ�3101��0--3100�����ֽ�

Param_T g_tParam;

void updateRelation(Relation_T *relation, enum ReaderOrButton_Enum type);
void updateNetCfg(NetCfg_T *netCfg, enum NetCfg_Enum type);
void updateSystemCfg(SystemCfg_T *systemCfg, enum OpenCfg_Enum type);
void updateNextStartAddr(NextStartAddr_T *nextStartAddr);
void updateMultipleCardID(uint8_t *data, uint16_t len, enum ID_Enum type);//ֱ�Ӵ������ַ+����+flag
static uint8_t compareID(uint8_t *id, uint8_t *idLibrary, uint16_t len);


//��ȡ����
void paramInit(void)
{
    uint8_t temp[300];//�ݴ��IIC��ȡ�����ݣ��ɷ�100��ID
    uint8_t i;
    uint16_t len=300;
    //���ƺ���ָ��
    g_tParam.updateRelation = updateRelation;
    g_tParam.updateNetCfg = updateNetCfg;
    g_tParam.updateSystemCfg = updateSystemCfg;
    g_tParam.updateNextStartAddr = updateNextStartAddr;
    g_tParam.updateMultipleCardID = updateMultipleCardID;
    
    ee_ReadBytes(temp, 0, EE_PAGE_SIZE*3);
    
    //first page
    if(temp[ADDR_LOCAL_IP] == 0xFF)
    {
        //��IPû�����ù���������
        resetParam();
        //����ͨ��������ȫ��Ϊ0xFF,�������ú��������
        //memset(g_tParam.multipleCardID.generalCardID, 0xFF, sizeof(g_tParam.multipleCardID.generalCardID));
        return;
    }

    memcpy(g_tParam.netCfg.local_ip,    &temp[ADDR_LOCAL_IP], sizeof(g_tParam.netCfg.local_ip));
    //memcpy(g_tParam.netCfg.local_port,  &temp[ADDR_LOCAL_PORT], sizeof(g_tParam.netCfg.local_port));
    g_tParam.netCfg.local_port = (temp[ADDR_LOCAL_PORT]<<8) + temp[ADDR_LOCAL_PORT+1];
    memcpy(g_tParam.netCfg.server_ip,   &temp[ADDR_SERVER_IP], sizeof(g_tParam.netCfg.server_ip));
    //memcpy(g_tParam.netCfg.server_port, &temp[ADDR_SERVER_PORT], sizeof(g_tParam.netCfg.server_port));
    g_tParam.netCfg.server_port = (temp[ADDR_SERVER_PORT]<<8) + temp[ADDR_SERVER_PORT+1];

    memcpy(&g_tParam.systemCfg.openTime, &temp[ADDR_OPEN_TIME], sizeof(g_tParam.systemCfg.openTime));
    memcpy(&g_tParam.systemCfg.waitTime, &temp[ADDR_WAIT_TIME], sizeof(g_tParam.systemCfg.waitTime));
    
    memcpy(&g_tParam.relation.relationA.reader_switcher, &temp[ADDR_RELATION_READER_A], sizeof(g_tParam.relation.relationA.reader_switcher));
    memcpy(&g_tParam.relation.relationB.reader_switcher, &temp[ADDR_RELATION_READER_B], sizeof(g_tParam.relation.relationB.reader_switcher));
    memcpy(&g_tParam.relation.relationA.button_switcher, &temp[ADDR_RELATION_BUTTON_A], sizeof(g_tParam.relation.relationA.reader_switcher));
    memcpy(&g_tParam.relation.relationB.button_switcher, &temp[ADDR_RELATION_BUTTON_B], sizeof(g_tParam.relation.relationB.reader_switcher));
    
    //i = sizeof(g_tParam.nextStartAddr.nextStartSector);
    memcpy(g_tParam.nextStartAddr.nextStartSector, &temp[NEXT_START_SECTOR_H], sizeof(g_tParam.nextStartAddr.nextStartSector));
    //i = sizeof(g_tParam.nextStartAddr.nextStartAddr);
    memcpy(g_tParam.nextStartAddr.nextStartAddr, &temp[NEXT_START_ADDR_H], sizeof(g_tParam.nextStartAddr.nextStartAddr));
    
    //�ڶ�ҳ���Ժ�
    //8�����ÿ���
    //modify 3-9 ˳������
    memcpy(&g_tParam.systemCfg.multipleOpenCfg[0], &temp[IS_OPEN_INTERLOCK], 1);
    memcpy(&g_tParam.systemCfg.multipleOpenCfg[1], &temp[IS_OPEN_FIRST_CARD], 1);
    memcpy(&g_tParam.systemCfg.multipleOpenCfg[2], &temp[IS_OPEN_MULTIPLE_CARD], 1);
    memcpy(&g_tParam.systemCfg.multipleOpenCfg[3], &temp[IS_OPEN_SUPER_CARD], 1);
    memcpy(&g_tParam.systemCfg.multipleOpenCfg[4], &temp[IS_OPEN_SUPER_PASSWORD], 1);
    memcpy(&g_tParam.systemCfg.multipleOpenCfg[5], &temp[IS_OPEN_THREAT_CARD], 1);
    memcpy(&g_tParam.systemCfg.multipleOpenCfg[6], &temp[IS_OPEN_THREAT_PASSWORD], 1);
    memcpy(&g_tParam.systemCfg.multipleOpenCfg[7], &temp[IS_OPEN_FINGER], 1);
    
    //last page
    //9����������
    memcpy(g_tParam.multipleCardID.firstCardID,     &temp[FIRST_CARD_ID], sizeof(g_tParam.multipleCardID.firstCardID));
    memcpy(g_tParam.multipleCardID.superCardID,     &temp[SUPER_CARD_ID], sizeof(g_tParam.multipleCardID.superCardID));
    memcpy(g_tParam.multipleCardID.threatCardID,    &temp[THREAT_CARD_ID], sizeof(g_tParam.multipleCardID.threatCardID));
    memcpy(g_tParam.multipleCardID.threatPasswordID,&temp[THREAT_PASSWORD_ID], sizeof(g_tParam.multipleCardID.threatPasswordID));
    memcpy(g_tParam.multipleCardID.superPasswordID, &temp[SUPER_PASSWORD_ID], sizeof(g_tParam.multipleCardID.superPasswordID));
    memcpy(g_tParam.multipleCardID.multipleCardID, &temp[MULTIPLE_CARD_ID], sizeof(g_tParam.multipleCardID.multipleCardID));
    memcpy(g_tParam.multipleCardID.keyBoardID, &temp[KEY_BOARD_ID], sizeof(g_tParam.multipleCardID.keyBoardID));
    //һ�ζ�300�ֽڣ���100�����ţ���5�ζ�ȡ
    for(i=0;i<5;i++)
    {
        ee_ReadBytes(temp, FINGER_ID+i*300, len);//ָ��ID
        memcpy(&g_tParam.multipleCardID.fingerID[i*len], temp, sizeof(temp));
    }
    for(i=0;i<5;i++)
    {
        ee_ReadBytes(temp, GENERAL_CARD_ID+i*300, len);//��ͨ��ID
        memcpy(&g_tParam.multipleCardID.generalCardID[i*len], temp, sizeof(temp));
    }
       
}

//��λ����, ��Ҫ��������w5500
void resetParam(void)
{
    uint16_t i;
    uint8_t buf[EE_PAGE_SIZE];
    uint8_t temp[2]; 
    //uint8_t card[3];    
    
    for(i = 0; i < EE_PAGE_SIZE; i++)
	{
		buf[i] = 0xFF;
	}
      
    for(i = 1; i<97; i++)//ֻ�ѵڶ�ҳ���Ժ���0xFF��97*32=3104>3100�����������
    {
        ee_WriteBytes(buf, i*32, EE_PAGE_SIZE);
    }
    //��λ�ṹ���Ա��ֵ
    //��һҳ��Ĭ�ϲ���������0xFF���
    g_tParam.netCfg.local_ip[0]=192;
    g_tParam.netCfg.local_ip[1]=168;
    g_tParam.netCfg.local_ip[2]=3;
    g_tParam.netCfg.local_ip[3]=100;
    g_tParam.netCfg.local_port = 6000;
    
    //server IP:255.255.255.255,broadcast
    g_tParam.netCfg.server_ip[0]=255;
    g_tParam.netCfg.server_ip[1]=255;
    g_tParam.netCfg.server_ip[2]=255;
    g_tParam.netCfg.server_ip[3]=255; 
    g_tParam.netCfg.server_port = 8085;
    
    g_tParam.systemCfg.openTime = 5;
    g_tParam.systemCfg.waitTime = 0;
    memset(g_tParam.systemCfg.multipleOpenCfg, 0, sizeof(g_tParam.systemCfg.multipleOpenCfg));
    
    g_tParam.relation.relationA.reader_switcher = 1;
    g_tParam.relation.relationB.reader_switcher = (1<<4);
    g_tParam.relation.relationA.button_switcher = 1;
    g_tParam.relation.relationB.button_switcher = (1<<4);
    
    g_tParam.nextStartAddr.nextStartSector[0] = 0;
    g_tParam.nextStartAddr.nextStartSector[1] = 64;
    g_tParam.nextStartAddr.nextStartAddr[0] = 0;
    g_tParam.nextStartAddr.nextStartAddr[1] = 0;
    
    //��ʼд��
    //net
    ee_WriteBytes(g_tParam.netCfg.local_ip, ADDR_LOCAL_IP, sizeof(g_tParam.netCfg.local_ip));
    temp[0] = g_tParam.netCfg.local_port >> 8;
    temp[1] = g_tParam.netCfg.local_port & 0xFF;
    ee_WriteBytes(temp, ADDR_LOCAL_PORT, sizeof(temp));
    ee_WriteBytes(g_tParam.netCfg.server_ip, ADDR_SERVER_IP, sizeof(g_tParam.netCfg.server_ip));
    temp[0] = g_tParam.netCfg.server_port >> 8;
    temp[1] = g_tParam.netCfg.server_port & 0xFF;
    ee_WriteBytes(temp, ADDR_SERVER_PORT, sizeof(temp));
    //���ÿ���
    ee_WriteOneBytes(g_tParam.systemCfg.openTime, ADDR_OPEN_TIME);
    ee_WriteOneBytes(g_tParam.systemCfg.waitTime, ADDR_WAIT_TIME);   
    ee_WriteOneBytes(g_tParam.systemCfg.multipleOpenCfg[0], IS_OPEN_INTERLOCK);
    ee_WriteOneBytes(g_tParam.systemCfg.multipleOpenCfg[1], IS_OPEN_FIRST_CARD);
    ee_WriteOneBytes(g_tParam.systemCfg.multipleOpenCfg[2], IS_OPEN_MULTIPLE_CARD);
    ee_WriteOneBytes(g_tParam.systemCfg.multipleOpenCfg[3], IS_OPEN_SUPER_CARD);
    ee_WriteOneBytes(g_tParam.systemCfg.multipleOpenCfg[4], IS_OPEN_SUPER_PASSWORD);
    ee_WriteOneBytes(g_tParam.systemCfg.multipleOpenCfg[5], IS_OPEN_THREAT_CARD);
    ee_WriteOneBytes(g_tParam.systemCfg.multipleOpenCfg[6], IS_OPEN_THREAT_PASSWORD);
    ee_WriteOneBytes(g_tParam.systemCfg.multipleOpenCfg[7], IS_OPEN_FINGER);
    //��Ӧ��ϵ
    ee_WriteOneBytes(g_tParam.relation.relationA.reader_switcher, ADDR_RELATION_READER_A);
    ee_WriteOneBytes(g_tParam.relation.relationB.reader_switcher, ADDR_RELATION_READER_B);
    ee_WriteOneBytes(g_tParam.relation.relationA.button_switcher, ADDR_RELATION_BUTTON_A);
    ee_WriteOneBytes(g_tParam.relation.relationB.button_switcher, ADDR_RELATION_BUTTON_B);
    //��һ����ʷ��¼��ַ
    ee_WriteOneBytes(g_tParam.nextStartAddr.nextStartSector[0], NEXT_START_SECTOR_H);
    ee_WriteOneBytes(g_tParam.nextStartAddr.nextStartSector[1], NEXT_START_SECTOR_L);
    ee_WriteOneBytes(g_tParam.nextStartAddr.nextStartAddr[0], NEXT_START_ADDR_H);
    ee_WriteOneBytes(g_tParam.nextStartAddr.nextStartAddr[1], NEXT_START_ADDR_L);
    
    //������������
    set_default(&g_tParam.netCfg);
}

//���²���--��Ӧ��ϵ
void updateRelation(Relation_T *relation, enum ReaderOrButton_Enum type)
{
    switch(type)
    {
        case e_READER_A:
            ee_WriteOneBytes(relation->relationA.reader_switcher, ADDR_RELATION_READER_A);
            break;
        case e_READER_B:
            ee_WriteOneBytes(relation->relationB.reader_switcher, ADDR_RELATION_READER_B);
            break;
        case e_BUTTON_A:
            ee_WriteOneBytes(relation->relationA.button_switcher, ADDR_RELATION_BUTTON_A);
            break;
        case e_BUTTON_B:
            ee_WriteOneBytes(relation->relationB.button_switcher, ADDR_RELATION_BUTTON_B);
            break;
        default:
            break;
    }
}

//���²���--��������
void updateNetCfg(NetCfg_T *netCfg, enum NetCfg_Enum type)
{
    uint8_t temp[2];
    switch(type)
    {
        case e_local_ip:
            ee_WriteBytes(netCfg->local_ip, ADDR_LOCAL_IP, sizeof(netCfg->local_ip));
            break;
        case e_local_port:
            temp[0] = netCfg->local_port >> 8;
            temp[1] = netCfg->local_port & 0xFF;
            ee_WriteBytes(temp, ADDR_LOCAL_PORT, sizeof(temp));
            break;
        case e_server_ip:
            ee_WriteBytes(netCfg->server_ip, ADDR_SERVER_IP, sizeof(netCfg->server_ip));
            break;
        case e_server_port:
            temp[0] = netCfg->server_port >> 8;
            temp[1] = netCfg->server_port & 0xFF;
            ee_WriteBytes(temp, ADDR_SERVER_PORT, sizeof(temp));
            break;
        default:
            break;
    }
}

//���²���--ϵͳ����
void updateSystemCfg(SystemCfg_T *systemCfg, enum OpenCfg_Enum type)
{
    switch(type)
    {
        case e_openTime:
            ee_WriteOneBytes(systemCfg->openTime, ADDR_OPEN_TIME);
            break;
        case e_waitTime:
            ee_WriteOneBytes(systemCfg->waitTime, ADDR_WAIT_TIME);
            break;
        case e_interlock:
            ee_WriteOneBytes(systemCfg->multipleOpenCfg[0], IS_OPEN_INTERLOCK);
            break;
        case e_firstCard:
            ee_WriteOneBytes(systemCfg->multipleOpenCfg[1], IS_OPEN_FIRST_CARD);
            break;
        case e_multipleCard:
            ee_WriteOneBytes(systemCfg->multipleOpenCfg[2], IS_OPEN_MULTIPLE_CARD);
            break;
        case e_superCard:
            ee_WriteOneBytes(systemCfg->multipleOpenCfg[3], IS_OPEN_SUPER_CARD);
            break;
        case e_superPassword:
            ee_WriteOneBytes(systemCfg->multipleOpenCfg[4], IS_OPEN_SUPER_PASSWORD);
            break;
        case e_threatCard:
            ee_WriteOneBytes(systemCfg->multipleOpenCfg[5], IS_OPEN_THREAT_CARD);
            break;
        case e_threatPassword:
            ee_WriteOneBytes(systemCfg->multipleOpenCfg[6], IS_OPEN_THREAT_PASSWORD);
            break;
        case e_finger:
            ee_WriteOneBytes(systemCfg->multipleOpenCfg[7], IS_OPEN_FINGER);
            break;
        default:
            break;
    }
}

//���²���--�´���ʷ��¼��ַ
void updateNextStartAddr(NextStartAddr_T *nextStartAddr)
{
    ee_WriteOneBytes(nextStartAddr->nextStartSector[0], NEXT_START_SECTOR_H);
    ee_WriteOneBytes(nextStartAddr->nextStartSector[1], NEXT_START_SECTOR_L);
    ee_WriteOneBytes(nextStartAddr->nextStartAddr[0], NEXT_START_ADDR_H);
    ee_WriteOneBytes(nextStartAddr->nextStartAddr[1], NEXT_START_ADDR_L);
}

//���²���--��������
void updateMultipleCardID(uint8_t *data, uint16_t len, enum ID_Enum type)
{
    uint8_t i = 0;
    //uint8_t temp[300]={0};
    switch(type)
    {
        case e_firstCardID:
            ee_WriteBytes(data, FIRST_CARD_ID, len);
            break;
        case e_superCardID:
            ee_WriteBytes(data, SUPER_CARD_ID, len);
            break;
        case e_superPasswordID:
            ee_WriteBytes(data, SUPER_PASSWORD_ID, len);
            break;
        case e_threatCardID:
            ee_WriteBytes(data, THREAT_CARD_ID, len);
            break;
        case e_threatPasswordID:
            ee_WriteBytes(data, THREAT_PASSWORD_ID, len);
            break;
        case e_keyBoardID:
            ee_WriteBytes(data, KEY_BOARD_ID, len);
            break;
        case e_multipleCardID:
            ee_WriteBytes(data, MULTIPLE_CARD_ID, len);
            break;
        case e_generalCardID://len=300
            len=len/5;
            //һ��д300�ֽڣ���100�����ţ���5��д��
            for(i=0;i<5;i++)
            {
                ee_WriteBytes(data, GENERAL_CARD_ID+i*300, len);
                data += 300;
            }
            //test
            //bsp_DelayMS(100);
            //ee_ReadBytes(temp, GENERAL_CARD_ID, 300);
            break;
        case e_fingerID://len=300
            len=len/5;
            //һ��д300�ֽڣ���100�����ţ���5��д��
            for(i=0;i<5;i++)
            {
                ee_WriteBytes(data, GENERAL_CARD_ID+i*300, len);
                data += 300;
            }
            break;
        default:
            break;
    }
}

//���������������ͣ����ض�Ӧö��ֵ��0xFF��ʾ������
//�жϳɹ����������ؿ������ͣ����ɹ�������ж�
uint8_t searchID(uint8_t *id)
{
    uint16_t i;
    
    for(i = 0; i<sizeof(g_tParam.systemCfg.multipleOpenCfg); i++)
    {
        if(g_tParam.systemCfg.multipleOpenCfg[i]!=0)
        {
            switch(i)
            {
                case 1://�׿�
                    if(compareID(id, g_tParam.multipleCardID.firstCardID, sizeof(g_tParam.multipleCardID.firstCardID)) == 1)
                    {
                        return e_firstCardID;
                    }
                    break;
                    
                case 2://���ؿ�
                    if(compareID(id, g_tParam.multipleCardID.multipleCardID, sizeof(g_tParam.multipleCardID.multipleCardID)) == 1)
                    {
                        return e_multipleCardID;
                    }
                    break;
                
                case 3://������
                    if(compareID(id, g_tParam.multipleCardID.superCardID, sizeof(g_tParam.multipleCardID.superCardID)) == 1)
                    {
                        return e_superCardID;
                    }
                    break;
                
                case 4://��������
                    if(compareID(id, g_tParam.multipleCardID.superPasswordID, sizeof(g_tParam.multipleCardID.superPasswordID)) == 1)
                    {
                        return e_superPasswordID;
                    }
                    break;
                
                case 5://в�ȿ�
                    if(compareID(id, g_tParam.multipleCardID.threatCardID, sizeof(g_tParam.multipleCardID.threatCardID)) == 1)
                    {
                        return e_threatCardID;
                    }
                    break;
                
                case 6://в����
                    if(compareID(id, g_tParam.multipleCardID.threatPasswordID, sizeof(g_tParam.multipleCardID.threatPasswordID)) == 1)
                    {
                        return e_threatPasswordID;
                    }
                    break;
                
                case 7://ָ��ģ��
                    if(compareID(id, g_tParam.multipleCardID.keyBoardID, sizeof(g_tParam.multipleCardID.keyBoardID)) == 1)
                    {
                        return e_keyBoardID;
                    }
                    if(compareID(id, g_tParam.multipleCardID.fingerID, sizeof(g_tParam.multipleCardID.fingerID)) == 1)
                    {
                        return e_threatCardID;
                    }                    
                    break;
                
                default:
                    break;
            }//end of switch
        }//end of if
    }//end of for
    
    //�ж���ͨ��
    if(compareID(id, g_tParam.multipleCardID.generalCardID, sizeof(g_tParam.multipleCardID.generalCardID)) == 1)
    {
        return e_generalCardID;
    }
    //�������ϣ�����0xFF
    return 0xFF;
}

//�ȽϿ���
static uint8_t compareID(uint8_t *id, uint8_t *idLibrary, uint16_t len)
{
    uint16_t i;
    for(i=0; i<len; i++)
    {
        if(id[0] == idLibrary[i])
        {
            if(id[1] == idLibrary[i+1])
            {
                if(id[2] == idLibrary[i+2])
                {
                    return 1;
                }
            }
        }
    }//end of for
    return 0;
}



