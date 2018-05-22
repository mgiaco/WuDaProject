#include "bsp.h"
#define VERSION 0x10//�汾��

void SendDataToServer(uint8_t flag, uint8_t rw, uint8_t *data, uint16_t len);

NetData_T g_tNetData;

//����λ
void MCU_Reset(void)
{
    __disable_fault_irq();      
    NVIC_SystemReset();// STM32 ����λ
}

//���������ʽ,������д�������buf������
void makeCommmand(uint8_t cmdFlag, uint8_t rw, uint8_t *data, uint16_t len)
{
    uint16_t i;
    uint8_t crcRet;
    
    g_tNetData.buf[0]=0xA5;
    g_tNetData.buf[1]=0xA5;
    g_tNetData.buf[2]=cmdFlag;
    g_tNetData.buf[3]=g_tParam.netCfg.mcuID[0];
    g_tNetData.buf[4]=g_tParam.netCfg.mcuID[1];
    g_tNetData.buf[5]=g_tParam.netCfg.mcuID[2];
    g_tNetData.buf[6]=g_tParam.netCfg.mcuID[3];
    g_tNetData.buf[7]=rw;
    g_tNetData.buf[8]=len >> 8;
    g_tNetData.buf[9]=(len & 0xFF);
    
    for(i=0; i<len;i++)
    {
        g_tNetData.buf[10+i] = data[i];
    }
    
    crcRet = Get_Crc8(g_tNetData.buf, 10+len);
    
    g_tNetData.buf[10+len+0] = crcRet;
    g_tNetData.buf[10+len+1] = 0x5A;
    g_tNetData.buf[10+len+2] = 0x5A;   
}

//ִ������,д�����ʱ����Ҫ�Ѳ������µ�IIC�洢��
void processCommand(uint8_t *data, uint16_t len)
{
    uint8_t ret=0;
    //uint8_t temp[512];
    
    //��Ӧ�������Ĺ㲥����
    if(data[2] == 0x00)
    {
        ret = g_tDoorStatus.doorA.feedBackStatus;
        ret = g_tDoorStatus.doorB.feedBackStatus << 4;
        SendDataToServer(data[2], 0, &ret, 1);
    }
    
    //���ж�ID��
    if(data[0] != 0xA5 || data[1] != 0xA5  || \
       data[3] != g_tParam.netCfg.mcuID[0] || \
       data[4] != g_tParam.netCfg.mcuID[1] || \
       data[5] != g_tParam.netCfg.mcuID[2] || \
       data[6] != g_tParam.netCfg.mcuID[3] || \
       data[len-2] != 0x5A || data[len-1] != 0x5A)
    {
        return;
    }
    
    //�����������Ҫ���в�����Ч���ж�
    switch(data[2])
    {
    
    /*
        *�Ƶ�����
        case 0x00://��������echo
        ret = g_tDoorStatus.doorA.feedBackStatus;
        ret = g_tDoorStatus.doorB.feedBackStatus << 4;
        SendDataToServer(data[2], 0, &ret, 1);
        break;*/
    
    //0x01���������ϴ�ˢ���Ŀ��źͶ�ͷ��
    
    case 0x02://��дʵʱʱ��DS1302оƬ��ʱ��
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, g_tRunInfo.time, 5);
        }
        else if(data[7] == 1)
        {
            //�ж�ʱ�䷶Χ
            if(data[10]<17 || data[10]>99 || \
               data[11]<=0 || data[11]>12 || \
               data[12]<=0 || data[12]>31 || \
               data[13]<=0 || data[13]>24 || \
               data[14]<=0 || data[14]>60)
            {
                ret = 0xAA;
                SendDataToServer(data[2], 1, &ret, 1);
                return;
            }
            ds1302_settime(&data[10], 5);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        }
        break;
        
    case 0x03://��������ip����
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, g_tParam.netCfg.local_ip, 4);
        }
        else if(data[7] == 1)
        {
            memcpy(g_tParam.netCfg.local_ip, &data[10], 4);
            g_tParam.updateNetCfg(&g_tParam.netCfg, e_local_ip);
            //������������
            set_default(&g_tParam.netCfg);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
    
    case 0x04://��������port����//����������Ч
        if(data[7] == 0)
        {
            data[10] = g_tParam.netCfg.local_port >> 8;
            data[11] = g_tParam.netCfg.local_port & 0xFF;
            SendDataToServer(data[2], 0, &data[10], 2);
        }
        else if(data[7] == 1)
        {
            g_tParam.netCfg.local_port = (data[10]<<8) + data[10+1];
            g_tParam.updateNetCfg(&g_tParam.netCfg, e_local_port);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x05://����������ip����
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, g_tParam.netCfg.server_ip, 4);
        }
        else if(data[7] == 1)
        {
            memcpy(g_tParam.netCfg.server_ip, &data[10], 4);
            g_tParam.updateNetCfg(&g_tParam.netCfg, e_server_ip);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
    
    case 0x06://����������port����
        if(data[7] == 0)
        {
            data[10] = g_tParam.netCfg.server_port >> 8;
            data[11] = g_tParam.netCfg.server_port & 0xFF;
            SendDataToServer(data[2], 0, &data[10], 2);
        }
        else if(data[7] == 1)
        {
            g_tParam.netCfg.server_port = (data[10]<<8) + data[10+1];
            g_tParam.updateNetCfg(&g_tParam.netCfg, e_server_port);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x07://����ʱ��
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.systemCfg.openTime, 1);
        }
        else if(data[7] == 1)
        {
            if(data[10]==0)
            {
                ret = 0xAA;
                SendDataToServer(data[2], 1, &ret, 1);
                return;
            }
            memcpy(&g_tParam.systemCfg.openTime, &data[10], 1);
            g_tParam.updateSystemCfg(&g_tParam.systemCfg, e_openTime);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
    
    case 0x08://����ʱ��
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.systemCfg.waitTime, 1);
        }
        else if(data[7] == 1)
        {
            //add 3-6
            //�ر�ʱ��Ϊ0����ʾ����ⷴ��
            memcpy(&g_tParam.systemCfg.waitTime, &data[10], 1);
            g_tParam.updateSystemCfg(&g_tParam.systemCfg, e_waitTime);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x09://��ͷA��ϵ
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.relation.relationA.reader_switcher, 1);
        }
        else if(data[7] == 1)
        {
            memcpy(&g_tParam.relation.relationA.reader_switcher, &data[10], 1);
            g_tParam.updateRelation(&g_tParam.relation, e_READER_A);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x0A://��ͷB��ϵ
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.relation.relationB.reader_switcher, 1);
        }
        else if(data[7] == 1)
        {
            memcpy(&g_tParam.relation.relationB.reader_switcher, &data[10], 1);
            g_tParam.updateRelation(&g_tParam.relation, e_READER_B);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x0B://����A��ϵ
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.relation.relationA.button_switcher, 1);
        }
        else if(data[7] == 1)
        {
            memcpy(&g_tParam.relation.relationA.button_switcher, &data[10], 1);
            g_tParam.updateRelation(&g_tParam.relation, e_BUTTON_A);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x0C://����B��ϵ
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.relation.relationB.button_switcher, 1);
        }
        else if(data[7] == 1)
        {
            memcpy(&g_tParam.relation.relationB.button_switcher, &data[10], 1);
            g_tParam.updateRelation(&g_tParam.relation, e_BUTTON_B);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
//8�����ŷ�ʽ������        
    case 0x0D://����
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.systemCfg.multipleOpenCfg[0], 1);
        }
        else if(data[7] ==1 )
        {
            //�������׿������ؿ�����ͬʱ����
            if(data[10]>1 || \
                g_tParam.systemCfg.multipleOpenCfg[1]==1 || \
                g_tParam.systemCfg.multipleOpenCfg[2]==1)
            {
                ret = 0xAA;
                SendDataToServer(data[2], 1, &ret, 1);
                return;
            }
            memcpy(&g_tParam.systemCfg.multipleOpenCfg[0], &data[10], 1);
            g_tParam.updateSystemCfg(&g_tParam.systemCfg, e_interlock);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x0E://�׿�
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.systemCfg.multipleOpenCfg[1], 1);
        }
        else if(data[7] == 1)
        {
            //�������׿������ؿ�����ͬʱ����
            if(data[10]>1 || \
                g_tParam.systemCfg.multipleOpenCfg[0]==1 || \
                g_tParam.systemCfg.multipleOpenCfg[2]==1)
            {
                ret = 0xAA;
                SendDataToServer(data[2], 1, &ret, 1);
                return;
            }
            memcpy(&g_tParam.systemCfg.multipleOpenCfg[1], &data[10], 1);
            g_tParam.updateSystemCfg(&g_tParam.systemCfg, e_firstCard);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x0F://���ؿ�
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.systemCfg.multipleOpenCfg[2], 1);
        }
        else if(data[7] == 1)
        {
            //�������׿������ؿ�����ͬʱ����
            if((data[10]>>4)<2 || (data[10]>>4)>10 ||(data[10]&0x0F)>1 || \
                g_tParam.systemCfg.multipleOpenCfg[0]==1 || \
                g_tParam.systemCfg.multipleOpenCfg[1]==1)           
            {
                ret = 0xAA;
                SendDataToServer(data[2], 1, &ret, 1);
                return;
            }
            memcpy(&g_tParam.systemCfg.multipleOpenCfg[2], &data[10], 1);
            g_tParam.updateSystemCfg(&g_tParam.systemCfg, e_multipleCard);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x10://������
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.systemCfg.multipleOpenCfg[3], 1);
        }
        else if(data[7] == 1)
        {
            if(data[10]>1)
            {
                ret = 0xAA;
                SendDataToServer(data[2], 1, &ret, 1);
                return;
            }
            memcpy(&g_tParam.systemCfg.multipleOpenCfg[3], &data[10], 1);
            g_tParam.updateSystemCfg(&g_tParam.systemCfg, e_superCard);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x11://��������
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.systemCfg.multipleOpenCfg[4], 1);
        }
        else if(data[7] == 1)
        {
            if(data[10]>1)
            {
                ret = 0xAA;
                SendDataToServer(data[2], 1, &ret, 1);
                return;
            }
            memcpy(&g_tParam.systemCfg.multipleOpenCfg[4], &data[10], 1);
            g_tParam.updateSystemCfg(&g_tParam.systemCfg, e_superPassword);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x12://в�ȿ�
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.systemCfg.multipleOpenCfg[5], 1);
        }
        else if(data[7] == 1)
        {
            if(data[10]>1)
            {
                ret = 0xAA;
                SendDataToServer(data[2], 1, &ret, 1);
                return;
            }
            memcpy(&g_tParam.systemCfg.multipleOpenCfg[5], &data[10], 1);
            g_tParam.updateSystemCfg(&g_tParam.systemCfg, e_threatCard);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x13://в����
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.systemCfg.multipleOpenCfg[6], 1);
        }
        else if(data[7] == 1)
        {
            if(data[10]>1)
            {
                ret = 0xAA;
                SendDataToServer(data[2], 1, &ret, 1);
                return;
            }
            memcpy(&g_tParam.systemCfg.multipleOpenCfg[6], &data[10], 1);
            g_tParam.updateSystemCfg(&g_tParam.systemCfg, e_threatPassword);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;
        
    case 0x14://ָ��ģ��
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, &g_tParam.systemCfg.multipleOpenCfg[7], 1);
        }
        else if(data[7] == 1)
        {
            if(data[10]>1)
            {
                ret = 0xAA;
                SendDataToServer(data[2], 1, &ret, 1);
                return;
            }
            memcpy(&g_tParam.systemCfg.multipleOpenCfg[7], &data[10], 1);
            g_tParam.updateSystemCfg(&g_tParam.systemCfg, e_finger);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        } 
        break;

//9�����ŵ�����       
    case 0x15://�׿�
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, g_tParam.multipleCardID.firstCardID, 3);
        }
        else if(data[7] == 1)
        {
            memcpy(g_tParam.multipleCardID.firstCardID, &data[10], 3);
            g_tParam.updateMultipleCardID(g_tParam.multipleCardID.firstCardID, 3, e_firstCardID);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        }
        break;
        
    case 0x16://������
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, g_tParam.multipleCardID.superCardID, 3);
        }
        else if(data[7] == 1)
        {
            memcpy(g_tParam.multipleCardID.superCardID, &data[10], 3);
            g_tParam.updateMultipleCardID(g_tParam.multipleCardID.superCardID, 3, e_superCardID);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        }
        break;
        
    case 0x17://��������
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, g_tParam.multipleCardID.superPasswordID, 3);
        }
        else if(data[7] == 1)
        {
            memcpy(g_tParam.multipleCardID.superPasswordID, &data[10], 3);
            g_tParam.updateMultipleCardID(g_tParam.multipleCardID.superPasswordID, 3, e_superPasswordID);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        }
        break;
        
    case 0x18://в�ȿ�,3�ſ�
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, g_tParam.multipleCardID.threatCardID, 9);
        }
        else if(data[7] == 1)
        {
            memcpy(g_tParam.multipleCardID.threatCardID, &data[10], 9);
            g_tParam.updateMultipleCardID(g_tParam.multipleCardID.threatCardID, 9, e_threatCardID);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        }
        break;
        
    case 0x19://в����
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, g_tParam.multipleCardID.threatPasswordID, 3);
        }
        else if(data[7] == 1)
        {
            memcpy(g_tParam.multipleCardID.threatPasswordID, &data[10], 3);
            g_tParam.updateMultipleCardID(g_tParam.multipleCardID.threatPasswordID, 3, e_threatPasswordID);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        }
        break;
        
    case 0x1A://��������
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, g_tParam.multipleCardID.keyBoardID, 3);
        }
        else if(data[7] == 1)
        {
            memcpy(g_tParam.multipleCardID.keyBoardID, &data[10], 3);
            g_tParam.updateMultipleCardID(g_tParam.multipleCardID.keyBoardID, 3, e_keyBoardID);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        }
        break;
        
    case 0x1B://���ؿ�
        if(data[7] == 0)
        {
            SendDataToServer(data[2], 0, g_tParam.multipleCardID.multipleCardID, 30);
        }
        else if(data[7] == 1)
        {
            memcpy(g_tParam.multipleCardID.multipleCardID, &data[10], 30);
            g_tParam.updateMultipleCardID(g_tParam.multipleCardID.multipleCardID, 30, e_multipleCardID);
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        }
        break;
        
    case 0x1C://ָ��ID����9���ֽڱ�ʾ�ڼ���
        if(data[7] == 0)
        {
            memcpy(&data[10], &g_tParam.multipleCardID.fingerID[data[9]*300], 300);
            SendDataToServer(data[2], 0, &data[10], 300);
        }
        else if(data[7] == 1)
        {
            if(data[9] <= 4)
            {
                memcpy(&g_tParam.multipleCardID.fingerID[data[9]*300], &data[10], 300);
            }
            if(data[9] == 4)//0,1,2,3,4
            {
                //5��������
                g_tParam.updateMultipleCardID(g_tParam.multipleCardID.fingerID, 1500, e_fingerID);
            }
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        }
        break;
        
    case 0x1D://��ͨ������9���ֽڱ�ʾ�ڼ���
        if(data[7] == 0)
        {
            memcpy(&data[10], &g_tParam.multipleCardID.generalCardID[data[9]*300], 300);
            SendDataToServer(data[2], 0, &data[10], 300);
        }
        else if(data[7] == 1)
        {
            if(data[9] <= 4)
            {
                memcpy(&g_tParam.multipleCardID.generalCardID[data[9]*300], &data[10], 300);
            }
            if(data[9] == 4)//0,1,2,3,4
            {
                //5��������
                g_tParam.updateMultipleCardID(g_tParam.multipleCardID.generalCardID, 1500, e_generalCardID);
            }
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
        }
        break;
        
    case 0x1E://�����ļ�
        //length��2λ������ʾ�ڼ���
        //spi����ǰ256k�ֽ�,(0--255)
        //ret = data[9];//page num
        if(data[9]%4 == 0)
        {
            sf_EraseSector(data[9]*1024);//��������4k�ֽ�
        }
        sf_PageWrite(&data[10], data[9]*1024, 1024);//����ÿ��1024�ֽڴ�С˳��д��spi flash
        ret = 0x55;
        SendDataToServer(data[2], 1, &ret, 1);

        break;
    
    case 0x1F://�������в���
        resetParam();
        ret = 0x55;
        SendDataToServer(data[2], 1, &ret, 1);
        break;
    
    case 0x20://Զ�̿���,A/B        
        if((data[10]==0x01) && (g_tDoorStatus.doorA.switcherStatus == NC)&& (g_tDoorStatus.doorB.switcherStatus == NC))
        {           
            g_tRunInfo.remoteOpen = data[10];//��a��b
            //����Զ�̿��ŵ��¼���־ 
            os_evt_set(REMOTE_OPEN_BIT, HandleTaskButton);
            ret = 0x55;
        }
        else if((data[10]==0x10) && (g_tDoorStatus.doorA.switcherStatus == NC)&& (g_tDoorStatus.doorB.switcherStatus == NC))
        {           
            g_tRunInfo.remoteOpen = data[10];//��a��b
            //����Զ�̿��ŵ��¼���־ 
            os_evt_set(REMOTE_OPEN_BIT, HandleTaskButton);
            ret = 0x55;
        }
        else ret = 0xAA;
        SendDataToServer(data[2], 1, &ret, 1);
        break;
        
    case 0x21://��������������
        ret = 0x55;
        SendDataToServer(data[2], 1, &ret, 1);
        //��д������־
        ee_WriteOneBytes(1, 0);//1��ʾ��Ҫ����,0��ʾ��iic���׵�ַ
        //���ŷŹ�
        //bsp_DelayMS(25000);//25>20
        //add 3-7����ϵͳ��λָ��   
        MCU_Reset();
        break;
    
    //0x22��Ϊ������Ϣ
    case 0x23://��ʷ��¼
        //length��λ��Ϊ�ڼ���
        if(((data[8]<<8) + data[9]) > (g_tSF.TotalSize/1024 - 256))
        {
            ret = 0xAA;
            SendDataToServer(data[2], 0, &ret, 1);
        }
        else
        {
            sf_ReadBuffer(&data[10], ((data[8]<<8) + data[9] + 256)*1024, 1024);//��spi������
            SendDataToServer(data[2], 0, &data[10], 1024);
        }
        break;
        
    case 0x24://��ȡ�汾��Ϣ
        ret=VERSION;
        SendDataToServer(data[2], 1, &ret, 1);
        break;
                      
    default:
        break;
    }
}

//�����ݷ��͵�������
void SendDataToServer(uint8_t flag, uint8_t rw, uint8_t *data, uint16_t len)
{
    //uint8_t ret;
    if(g_tNetData.status == e_Link)
    {
        makeCommmand(flag, rw, data, len);
        sendto(0, g_tNetData.buf, 10+len+3, g_tParam.netCfg.server_ip, g_tParam.netCfg.server_port);//�ڶ�������µ��÷��ͻ���ʱ
//        DEBUG(COM1, &ret, 1);
    }
}
