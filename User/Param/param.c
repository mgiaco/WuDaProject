#include "bsp.h"

Param_T g_tParam;

//初始化参数结构体
void paramInit(void)
{
    //先读取参数，判断是否有效？如果无效则使用宏定义的参数
    LoadParam();
    //如果没有存储参数，则使用默认值
    if((0xFF == g_tParam.loraAddress[0]) && (0xFF == g_tParam.loraAddress[1]))
    {
        g_tParam.loraAddress[0] = LORA_ADDRESS & 0xFF;
        g_tParam.loraAddress[1] = LORA_ADDRESS >> 8;
    }
    
    if((0xFF == g_tParam.dtuAddress[0]) && (0xFF == g_tParam.dtuAddress[1]))
    {
        g_tParam.dtuAddress[0] = DTU_ADDRESS & 0xFF;
        g_tParam.dtuAddress[1] = DTU_ADDRESS >> 8;
    }
    
    if(0xFF == g_tParam.channel)
    {
        g_tParam.channel = CHANNEL;
    }
    
    //TODO:串口配置完成后，根据使用参数对lora模块进行配置，
    //需要切换模式，串口发送配置命令
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

