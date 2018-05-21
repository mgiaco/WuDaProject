/*
*********************************************************************************************************
*
*	模块名称 : BSP模块(For STM32F1XX)
*	文件名称 : bsp.c
*	版    本 : V1.0
*	说    明 : 这是硬件底层驱动程序模块的主文件。主要提供 bsp_Init()函数供主程序调用。主程序的每个c文件可以在开
*			  头	添加 #include "bsp.h" 来包含所有的外设驱动模块。
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2013-03-01 armfly   正式发布
*		V1.1    2015-08-10 Eric2013 增加uCOS-III所需函数
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"
static void GetCPU_ID(uint8_t *data, uint8_t len);

RunInfo_T g_tRunInfo;

static void GetCPU_ID(uint8_t *data, uint8_t len)  
{  
    u32 CpuID[3];
    u32 mac_Code; 
    //u8 ID0,ID1,ID2,ID3;
    CpuID[0]=*(u32*)(0x1FFFF7E8);  
    CpuID[1]=*(u32*)(0x1FFFF7EC);  
    CpuID[2]=*(u32*)(0x1FFFF7F0);  
    mac_Code = (CpuID[0]>>1)+(CpuID[1]>>2)+(CpuID[2]>>3);
    data[0]=mac_Code&0x000000FF;
    data[1]=(mac_Code&0x0000FF00)>>8;
    data[2]=(mac_Code&0x00FF0000)>>16;
    data[3]=(mac_Code&0xFF000000)>>24;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_Init
*	功能说明: 初始化硬件设备。只需要调用一次。该函数配置CPU寄存器和外设的寄存器并初始化一些全局变量。
*			 全局变量。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_Init(void)
{
	/*
		由于ST固件库的启动文件已经执行了CPU系统时钟的初始化，所以不必再次重复配置系统时钟。
		启动文件配置了CPU主时钟频率、内部Flash访问速度和可选的外部SRAM FSMC初始化。

		系统时钟缺省配置为72MHz，如果需要更改，可以修改 system_stm32f10x.c 文件
	*/
	
	/* 优先级分组设置为4，可配置0-15级抢占式优先级，0级子优先级，即不存在子优先级。*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    bsp_InitDWT();//用来做精确微秒延时
    
	//bsp_InitUart(); 	/* 初始化串口 */
	bsp_InitLed(); 		/* 初始LED指示灯端口 */
	bsp_InitKey();		/* 初始化按键 */
	bsp_InitHardTimer();    /* 硬件定时器初始化*/   
    
    bsp_InitSPIBus();	/* 配置SPI总线 */		
	bsp_InitSFlash();	/* 初始化SPI 串行Flash */
    
    bsp_InitI2C();		/* 配置I2C总线 */
    WiegandInit();		//初始化韦根信号线
    
    ds1302_GPIO_Configuration();//1302模拟时序引脚初始化   
    
    WIZ_SPI_Init();//初始化w5500用到的GPIO口和spi功能
    Reset_W5500();//硬重启w5500
    
    paramInit();//读取参数
    GetCPU_ID(g_tParam.netCfg.mcuID, 4);//获取cpuID
    set_default(&g_tParam.netCfg);//设置默认的MAC，IP，GW，SUB，DNS,并初始化
    
    bsp_InitIwdg(0xC35);//看门狗溢出时间为20s
    
    //状态置0
    g_tRunInfo.firstCardStatus = 0;
    g_tRunInfo.multipleCardStatus = 0;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
