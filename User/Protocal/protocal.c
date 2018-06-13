#include "bsp.h"

//软复位
void MCU_Reset(void)
{
    __disable_fault_irq();      
    NVIC_SystemReset();// STM32 软复位
}

//构造命令格式,把数据写入网络的buf数组中
void makeCommmand(uint8_t cmdFlag, uint8_t rw, uint8_t *data, uint8_t len)
{
    uint16_t i;
    uint8_t crcRet;
    
    //前3个字节存放服务器的地址和信道参数
    //地址的高位在前，dtuAddress数组是低位在前
    g_tLora.buf[0]=g_tParam.dtuAddress[0];
    g_tLora.buf[1]=g_tParam.dtuAddress[1];
    g_tLora.buf[2]=g_tParam.channel;
    
    g_tLora.buf[3]=0xA5;
    g_tLora.buf[4]=0xA5;
    g_tLora.buf[5]=cmdFlag;
    g_tLora.buf[6]=0x00;//保证4字节ID号
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

//执行命令,写入参数时，还要把参数更新到IIC存储中
void processCommand(uint8_t *data, uint16_t len)
{
    //uint8_t ret=0;
    
    //对于设置命令，要进行参数有效性判断
    //TODO:命令等板子基础功能测试之后再慢慢往上加
    /*0x00:搜索设备，返回基础状态信息（经纬度，电量）
    *0x01:返回探测器触发的精确时间和能级
    *
    *0x06:把升级文件写入spi Flash，每包128字节，则4k/128=32,需要32包数据才擦除一个区
    *0x07:升级重启
    *0x08:读取版本信息
    *0x03:配置本地地址
    *0x04:配置本地信道
    *0x05:配置DTU地址
    */
    switch(data[2])
    {
        case 0x00:
            break;
        
        
        default:
            break;
    }
    
}

//把数据发送到服务器
void SendDataToServer(uint8_t flag, uint8_t rw, uint8_t *data, uint8_t len)
{
    //TODO:判断AUX引脚状态，为1时（高电平）可以进行发送
    if(1)
    {
        makeCommmand(flag, rw, data, len);
        LoraSendData(g_tLora.buf, 13+len+3);//在断网情况下调用发送会延时
    }
}

//--------------------------------------------------
