#include "bsp.h"

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
    g_tLora.buf[0]=g_tParam.dtuAddress[0];
    g_tLora.buf[1]=g_tParam.dtuAddress[1];
    g_tLora.buf[2]=g_tParam.channel;
    
    g_tLora.buf[3]=0xA5;
    g_tLora.buf[4]=0xA5;
    g_tLora.buf[5]=cmdFlag;
    g_tLora.buf[6]=0x00;//��֤4�ֽ�ID��
    g_tLora.buf[7]=g_tParam.loraAddress[0];
    g_tLora.buf[8]=g_tParam.loraAddress[1];
    g_tLora.buf[9]=g_tParam.channel;
    g_tLora.buf[10]=rw;
    g_tLora.buf[11]=len >> 8;
    g_tLora.buf[12]=(len & 0xFF);
    
    for(i=0; i<len;i++)
    {
        g_tLora.buf[13+i] = data[i];
    }
    
    crcRet = Get_Crc8(g_tLora.buf, 13+len);
    
    g_tLora.buf[13+len+0] = crcRet;
    g_tLora.buf[13+len+1] = 0x5A;
    g_tLora.buf[13+len+2] = 0x5A;   
}

//ִ������,д�����ʱ����Ҫ�Ѳ������µ�IIC�洢��
void processCommand(uint8_t *data, uint16_t len)
{
    //uint8_t ret=0;
    
    //�����������Ҫ���в�����Ч���ж�
    //TODO:����Ȱ��ӻ������ܲ���֮�����������ϼ�
    /*0x00:�����豸�����ػ���״̬��Ϣ����γ�ȣ�������
    *0x01:����̽���������ľ�ȷʱ����ܼ�
    *
    *0x06:�������ļ�д��spi Flash��ÿ��128�ֽڣ���4k/128=32,��Ҫ32�����ݲŲ���һ����
    *0x07:��������
    *0x08:��ȡ�汾��Ϣ
    *0x03:���ñ��ص�ַ
    *0x04:���ñ����ŵ�
    *0x05:����DTU��ַ
    */
    switch(data[2])
    {
        case 0x00:
            break;
        
        
        default:
            break;
    }
    
}

//�����ݷ��͵�������
void SendDataToServer(uint8_t flag, uint8_t rw, uint8_t *data, uint8_t len)
{
    //TODO:�ж�AUX����״̬��Ϊ1ʱ���ߵ�ƽ�����Խ��з���
    if(1)
    {
        makeCommmand(flag, rw, data, len);
        LoraSendData(g_tLora.buf, 13+len+3);//�ڶ�������µ��÷��ͻ���ʱ
    }
}

//--------------------------------------------------
