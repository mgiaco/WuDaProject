#include "bsp.h"

/****用作lora的地址，也作为设备ID号（1-500，不能取0和FFFF）*/
#define LORA_ADDRESS         1
/*********************************************************/

Param_T g_tParam;

//初始化lora模块
void loraInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_LORA_PORT, ENABLE);//使能PORT_B时钟
	
	GPIO_InitStructure.GPIO_Pin  = AUX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(LORA_PORT, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	/* 推挽输出模式 */	
	GPIO_InitStructure.GPIO_Pin = LORA_M0_PIN | LORA_M1_PIN;
	GPIO_Init(LORA_PORT, &GPIO_InitStructure);
}

//初始化参数结构体
void paramInit(void)
{
    //初始化lora模块
    loraInit();
    
    //test擦除IIC flash为0xFF
//    resetParam();
//    bsp_DelayMS(100);
    
    
    //先读取参数，判断是否有效？如果无效则使用宏定义的参数
    LoadParam();
    
    //如果没有存储参数，则使用默认值
    if((0xFF == g_tParam.loraAddress[0]) && (0xFF == g_tParam.loraAddress[1]))
    {
        g_tParam.loraAddress[1] = LORA_ADDRESS & 0xFF;
        g_tParam.loraAddress[0] = LORA_ADDRESS >> 8;
    }
    
    if((0xFF == g_tParam.dtuAddress[0]) && (0xFF == g_tParam.dtuAddress[1]))
    {
        g_tParam.dtuAddress[1] = DTU_ADDRESS & 0xFF;
        g_tParam.dtuAddress[0] = DTU_ADDRESS >> 8;
    }
    
    if(0xFF == g_tParam.channel)
    {
        g_tParam.channel = CHANNEL;
    }
    
    //串口配置完成后，根据使用参数对lora模块进行配置，
    //需要切换模式，串口发送配置命令
    setLoraParam();
}

/*
*********************************************************************************************************
*	函 数 名: LoadParam
*	功能说明: 从Flash读参数到g_tParam
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void LoadParam(void)
{
	ee_ReadBytes((uint8_t *)&g_tParam, PARAM_ADDR, sizeof(Param_T));
}

/*
*********************************************************************************************************
*	函 数 名: SaveParam
*	功能说明: 将全局变量g_tParam 写入到IIC Flash
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void SaveParam(void)
{
	/* 将全局的参数变量保存到EEPROM */
	ee_WriteBytes((uint8_t *)&g_tParam, PARAM_ADDR, sizeof(Param_T));
}

//重置IIC数据
void resetParam(void)
{
    uint16_t i;
    uint8_t buf[EE_PAGE_SIZE];  
    
    for(i = 0; i < EE_PAGE_SIZE; i++)
	{
		buf[i] = 0xFF;
	}
    
    ee_WriteBytes(buf, 0, EE_PAGE_SIZE);
}

//设置lora模块模式，0--工作，1-参数设置
void setLoraMode(uint8_t mode)
{
    if(mode == 0)//置低电平
    {
        GPIO_ResetBits(LORA_PORT, LORA_M0_PIN);
        GPIO_ResetBits(LORA_PORT, LORA_M1_PIN);
    }
    else //置高电平
    {
        GPIO_SetBits(LORA_PORT, LORA_M0_PIN);
        GPIO_SetBits(LORA_PORT, LORA_M1_PIN);
    }
}


//设置lora模块参数
void setLoraParam(void)
{
    uint8_t loraParam[6];
    
    loraParam[0]=0xC0;//参数掉电保存
    loraParam[1]=g_tParam.loraAddress[0];
    loraParam[2]=g_tParam.loraAddress[1];
    loraParam[3]=0x1A;//串口速率默认9600，空速默认2.4K
    loraParam[4]=g_tParam.channel;//信道为30,410+30=440MHz
    loraParam[5]=0xC0;//传输方式选择定点传输，发射功率和无线唤醒时间、FEC开关使用默认值
    //先拉高M0，M1
    setLoraMode(1);
    bsp_DelayMS(100);
    //串口输出参数
    LoraSendData(loraParam, sizeof(loraParam));
    bsp_DelayMS(100);
    //再拉低M0，M1
    setLoraMode(0);
    bsp_DelayMS(100);
}

//获取AUX引脚电平状态
uint8_t getAuxStatus(void)
{
    return READ_AUX;
}

//---------------------------
