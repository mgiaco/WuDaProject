#include "bsp.h"

#define UPDATE_DATA_LENGTH 64

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
    g_tLora.buf[0]=g_tParam.dtuAddress[0];//dtu地址高8位
    g_tLora.buf[1]=g_tParam.dtuAddress[1];//dtu地址低8位
    g_tLora.buf[2]=g_tParam.channel;//lora信道
    
    g_tLora.buf[3]=0xA5;//报文头
    g_tLora.buf[4]=0xA5;//报文头
    g_tLora.buf[5]=cmdFlag;//命令号
    g_tLora.buf[6]=0x00;//保证4字节ID号
    g_tLora.buf[7]=g_tParam.loraAddress[0];//lora地址高8位
    g_tLora.buf[8]=g_tParam.loraAddress[1];//lora地址低8位
    g_tLora.buf[9]=g_tParam.channel;//lora信道
    g_tLora.buf[10]=rw;//读写位
    g_tLora.buf[11]=len >> 8;//数据长度高8位
    g_tLora.buf[12]=(len & 0xFF);//数据长度低8位
    //数据部分
    for(i=0; i<len;i++)
    {
        g_tLora.buf[13+i] = data[i];
    }
    
    //crcRet = Get_Crc8(g_tLora.buf, 13+len);
    crcRet = 0xFF;
    
    g_tLora.buf[13+len+0] = crcRet;//校验位
    g_tLora.buf[13+len+1] = 0x5A;//报文尾
    g_tLora.buf[13+len+2] = 0x5A;//报文尾 
}

//执行命令,写入参数时，还要把参数更新到IIC存储中
void processCommand(uint8_t *data, uint16_t len)
{
    static uint8_t ret;
    static uint16_t battery;
    static uint8_t pasitionAndBattery[12];//5字节纬度，5字节经度，2字节电量
    static uint16_t updateCounts;//升级包计数
    //对于设置命令，要进行参数有效性判断
    /*0x00:搜索设备，返回基础状态信息（经纬度，电量）
    *0x01:返回探测器触发的精确时间和能级
    *0x02:复位探测器
    *
    以下命令后期再加入。
    *擦除iic flash
    *配置本地地址
    *配置本地信道
    *配置终端地址
    *把升级文件写入spi Flash，每包128字节，则4k/128=32,需要32包数据才擦除一个区
    *升级重启
    *读取版本信息
    */
    
    //先判断ID号
    if(data[3] != 0x00 || \
       data[4] != g_tParam.loraAddress[0] || \
       data[5] != g_tParam.loraAddress[1] || \
       data[6] != g_tParam.channel)
    {
        return;
    }
    //判断命令号
    switch(data[2])
    {
        case 0x00://搜索设备，也用作
            //纬度
            pasitionAndBattery[0] = (g_tGps.latitude[0]-0x30)*10+(g_tGps.latitude[1]-0x30);
            pasitionAndBattery[1] = (g_tGps.latitude[2]-0x30)*10+(g_tGps.latitude[3]-0x30);
            //跳过中间的小数点
            pasitionAndBattery[2] = (g_tGps.latitude[5]-0x30)*10+(g_tGps.latitude[6]-0x30);
            pasitionAndBattery[3] = (g_tGps.latitude[7]-0x30)*10+(g_tGps.latitude[8]-0x30);
            pasitionAndBattery[4] = (g_tGps.latitude[9]-0x30)*10+(g_tGps.latitude[10]-0x30);
            //经度
            pasitionAndBattery[5] = (g_tGps.longiitude[0]-0x30)*100+(g_tGps.longiitude[1]-0x30)*10+(g_tGps.longiitude[2]-0x30);
            pasitionAndBattery[6] = (g_tGps.longiitude[3]-0x30)*10+(g_tGps.longiitude[4]-0x30);
            //跳过中间的小数点
            pasitionAndBattery[7] = (g_tGps.longiitude[6]-0x30)*10+(g_tGps.longiitude[7]-0x30);
            pasitionAndBattery[8] = (g_tGps.longiitude[8]-0x30)*10+(g_tGps.longiitude[9]-0x30);
            pasitionAndBattery[9] = (g_tGps.longiitude[10]-0x30)*10+(g_tGps.longiitude[11]-0x30);
            if(pasitionAndBattery[9] == 0xF0)//没有信号时，全部为0
            {
                memset(pasitionAndBattery, 0, sizeof(pasitionAndBattery)-2);
            }
            //获取电压
            GetADC(&battery);
            battery = battery*3300*2/4095;
            pasitionAndBattery[10] = battery >> 8;
            pasitionAndBattery[11] = (battery & 0xFF);
            
            SendDataToServer(data[2], 0, pasitionAndBattery, sizeof(pasitionAndBattery));              
            break;
        
        case 0x01://返回探测器触发的精确时间和能级
            //20180625取消自动复位，防止误触发
            g_tReader.preciseTime[6] = GetDetectorLevel();
            SendDataToServer(data[2], 0, g_tReader.preciseTime, sizeof(g_tReader.preciseTime));
            break;
            
        case 0x02://能级复位
            ret=0x55;
            os_evt_set(REAET_LEVEL_BIT, HandleTaskNet);//post
            SendDataToServer(data[2], 1, &ret, 1);
            break;
        
        case 0x1E://升级文件
            //length的2位用来表示第几包
            //spi分配前256k字节,(0--255)
            //20180704实际用不到这么多，考虑到传输时间问题，
            //现在的bin大小为12K，所以上限改为16K,共16*1024/64=256包
            //4096/64=64;
            if(data[9]%(4*1024/UPDATE_DATA_LENGTH) == 0)//64包加起来为4K
            {
                sf_EraseSector(data[9]*UPDATE_DATA_LENGTH);//扇区擦除4k字节
            }
            sf_exWrite_64(&data[10], data[9]*UPDATE_DATA_LENGTH);//按照每包UPDATE_DATA_LENGTH字节大小顺序写入spi flash
            updateCounts=data[9];
            ret = 0x55;
            SendDataToServer(data[2], 1, &ret, 1);
            break;
        
        case 0x21://重启（并升级）
            if(updateCounts==0xFF)
            {
                ret = 0x55;
                SendDataToServer(data[2], 0, &ret, 1);
                //改写升级标志
                ee_WriteOneBytes(1, 0);//1表示需要升级,0表示在iic的首地址          
                //延时3秒，等待数据传输到服务器后再重启，待测试
                bsp_DelayMS(3000);
                //调用系统复位指令   
                MCU_Reset();
            }
            else
            {
                ret = 0xAA;
                SendDataToServer(data[2], 0, &ret, 1);
            }
            break;
        
        case 0x24://读取版本信息
            ret=VERSION;
            SendDataToServer(data[2], 0, &ret, 1);
            break;
        
        default:
            break;
    }
    
}

//把数据发送到服务器
void SendDataToServer(uint8_t flag, uint8_t rw, uint8_t *data, uint8_t len)
{
    //判断AUX引脚状态，为1时（高电平）可以进行发送
    //也可以不判断，模块内部有缓冲区，不超出就可以
    //while(!getAuxStatus());

    makeCommmand(flag, rw, data, len);
    LoraSendData(g_tLora.buf, 13+len+3);//在断网情况下调用发送会延时
}

//--------------------------------------------------
