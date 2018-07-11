#include "bsp.h"

#define UPDATE_DATA_LENGTH 64

//��λ
void MCU_Reset(void)
{
    __disable_fault_irq();      
    NVIC_SystemReset();// STM32 ��λ
}

//���������ʽ,������д�������buf������
void makeCommmand(uint8_t cmdFlag, uint8_t rw, uint8_t *data, uint8_t len)
{
    uint16_t i;
    uint8_t crcRet;
    
    //ǰ3���ֽڴ�ŷ������ĵ�ַ���ŵ�����
    //��ַ�ĸ�λ��ǰ��dtuAddress�����ǵ�λ��ǰ
    g_tLora.buf[0]=g_tParam.dtuAddress[0];//dtu��ַ��8λ
    g_tLora.buf[1]=g_tParam.dtuAddress[1];//dtu��ַ��8λ
    g_tLora.buf[2]=g_tParam.channel;//lora�ŵ�
    
    g_tLora.buf[3]=0xA5;//����ͷ
    g_tLora.buf[4]=0xA5;//����ͷ
    g_tLora.buf[5]=cmdFlag;//�����
    g_tLora.buf[6]=0x00;//��֤4�ֽ�ID��
    g_tLora.buf[7]=g_tParam.loraAddress[0];//lora��ַ��8λ
    g_tLora.buf[8]=g_tParam.loraAddress[1];//lora��ַ��8λ
    g_tLora.buf[9]=g_tParam.channel;//lora�ŵ�
    g_tLora.buf[10]=rw;//��дλ
    g_tLora.buf[11]=len >> 8;//���ݳ��ȸ�8λ
    g_tLora.buf[12]=(len & 0xFF);//���ݳ��ȵ�8λ
    //���ݲ���
    for(i=0; i<len;i++)
    {
        g_tLora.buf[13+i] = data[i];
    }
    
    //crcRet = Get_Crc8(g_tLora.buf, 13+len);
    crcRet = 0xFF;
    
    g_tLora.buf[13+len+0] = crcRet;//У��λ
    g_tLora.buf[13+len+1] = 0x5A;//����β
    g_tLora.buf[13+len+2] = 0x5A;//����β 
}

//ִ������,д�����ʱ����Ҫ�Ѳ������µ�IIC�洢��
void processCommand(uint8_t *data, uint16_t len)
{
    static uint8_t ret;
    static uint16_t battery;
    static uint8_t pasitionAndBattery[12];//5�ֽ�γ�ȣ�5�ֽھ��ȣ�2�ֽڵ���
    static uint16_t updateCounts;//����������
    //�����������Ҫ���в�����Ч���ж�
    /*0x00:�����豸�����ػ���״̬��Ϣ����γ�ȣ�������
    *0x01:����̽���������ľ�ȷʱ����ܼ�
    *0x02:��λ̽����
    *
    ������������ټ��롣
    *����iic flash
    *���ñ��ص�ַ
    *���ñ����ŵ�
    *�����ն˵�ַ
    *�������ļ�д��spi Flash��ÿ��128�ֽڣ���4k/128=32,��Ҫ32�����ݲŲ���һ����
    *��������
    *��ȡ�汾��Ϣ
    */
    
    //���ж�ID��
    if(data[3] != 0x00 || \
       data[4] != g_tParam.loraAddress[0] || \
       data[5] != g_tParam.loraAddress[1] || \
       data[6] != g_tParam.channel)
    {
        return;
    }
    //�ж������
    switch(data[2])
    {
        case 0x00://�����豸��Ҳ����
            //γ��
            pasitionAndBattery[0] = (g_tGps.latitude[0]-0x30)*10+(g_tGps.latitude[1]-0x30);
            pasitionAndBattery[1] = (g_tGps.latitude[2]-0x30)*10+(g_tGps.latitude[3]-0x30);
            //�����м��С����
            pasitionAndBattery[2] = (g_tGps.latitude[5]-0x30)*10+(g_tGps.latitude[6]-0x30);
            pasitionAndBattery[3] = (g_tGps.latitude[7]-0x30)*10+(g_tGps.latitude[8]-0x30);
            pasitionAndBattery[4] = (g_tGps.latitude[9]-0x30)*10+(g_tGps.latitude[10]-0x30);
            //����
            pasitionAndBattery[5] = (g_tGps.longiitude[0]-0x30)*100+(g_tGps.longiitude[1]-0x30)*10+(g_tGps.longiitude[2]-0x30);
            pasitionAndBattery[6] = (g_tGps.longiitude[3]-0x30)*10+(g_tGps.longiitude[4]-0x30);
            //�����м��С����
            pasitionAndBattery[7] = (g_tGps.longiitude[6]-0x30)*10+(g_tGps.longiitude[7]-0x30);
            pasitionAndBattery[8] = (g_tGps.longiitude[8]-0x30)*10+(g_tGps.longiitude[9]-0x30);
            pasitionAndBattery[9] = (g_tGps.longiitude[10]-0x30)*10+(g_tGps.longiitude[11]-0x30);
            if(pasitionAndBattery[9] == 0xF0)//û���ź�ʱ��ȫ��Ϊ0
            {
                memset(pasitionAndBattery, 0, sizeof(pasitionAndBattery)-2);
            }
            //��ȡ��ѹ
            GetADC(&battery);
            battery = battery*3300*2/4095;
            pasitionAndBattery[10] = battery >> 8;
            pasitionAndBattery[11] = (battery & 0xFF);
            
            SendDataToServer(data[2], 0, pasitionAndBattery, sizeof(pasitionAndBattery));              
            break;
        
        case 0x01://����̽���������ľ�ȷʱ����ܼ�
            //20180625ȡ���Զ���λ����ֹ�󴥷�
            g_tReader.preciseTime[6] = GetDetectorLevel();
            SendDataToServer(data[2], 0, g_tReader.preciseTime, sizeof(g_tReader.preciseTime));
            break;
            
        case 0x02://�ܼ���λ
            ret=0x55;
            os_evt_set(REAET_LEVEL_BIT, HandleTaskNet);//post
            SendDataToServer(data[2], 1, &ret, 1);
            break;
        
        case 0x1E://�����ļ�
            //length��2λ������ʾ�ڼ���
            //spi����ǰ256k�ֽ�,(0--255)
            //20180704ʵ���ò�����ô�࣬���ǵ�����ʱ�����⣬
            //���ڵ�bin��СΪ12K���������޸�Ϊ16K,��16*1024/64=256��
            //4096/64=64;
            if(data[9]%(4*1024/UPDATE_DATA_LENGTH) == 0)//64��������Ϊ4K
            {
                sf_EraseSector(data[9]*UPDATE_DATA_LENGTH);//��������4k�ֽ�
            }
            sf_exWrite_64(&data[10], data[9]*UPDATE_DATA_LENGTH);//����ÿ��UPDATE_DATA_LENGTH�ֽڴ�С˳��д��spi flash
            updateCounts=data[9];
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
            break;
        
        case 0x21://��������������
            if(updateCounts==0xFF)
            {
                ret = 0x55;
                SendDataToServer(data[2], 0, &ret, 1);
                //��д������־
                ee_WriteOneBytes(1, 0);//1��ʾ��Ҫ����,0��ʾ��iic���׵�ַ          
                //��ʱ3�룬�ȴ����ݴ��䵽����������������������
                bsp_DelayMS(3000);
                //����ϵͳ��λָ��   
                MCU_Reset();
            }
            else
            {
                ret = 0xAA;
                SendDataToServer(data[2], 0, &ret, 1);
            }
            break;
        
        case 0x24://��ȡ�汾��Ϣ
            ret=VERSION;
            SendDataToServer(data[2], 0, &ret, 1);
            break;
        
        default:
            break;
    }
    
}

//�����ݷ��͵�������
void SendDataToServer(uint8_t flag, uint8_t rw, uint8_t *data, uint8_t len)
{
    //�ж�AUX����״̬��Ϊ1ʱ���ߵ�ƽ�����Խ��з���
    //Ҳ���Բ��жϣ�ģ���ڲ��л��������������Ϳ���
    //while(!getAuxStatus());

    makeCommmand(flag, rw, data, len);
    LoraSendData(g_tLora.buf, 13+len+3);//�ڶ�������µ��÷��ͻ���ʱ
}

//--------------------------------------------------
