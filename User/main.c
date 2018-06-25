/*
*********************************************************************************************************
*	文件名称 : main.c(任务的优先级数值越小优先级越低，这个跟uCOS相反)
*********************************************************************************************************
*/

#include "bsp.h"			/* 底层硬件驱动 */

#define READ_LEVEL_DELAY 3//读取能级的延时秒数
static void AppTaskCreate (void);//创建除start任务外的其他任务

/*任务1：看门狗
*任务2：处理串口3的lora数据（通过事件标志组），和服务器进行命令交互
*/
//任务函数
__task void AppTaskStart(void);
__task void AppTaskNet(void);
__task void AppTaskMonitor(void);
 
static uint64_t AppTaskStartStk[512/8];   /* 任务栈 */
static uint64_t AppTaskNetStk[512/8];     /* 任务栈 */
//static uint64_t AppTaskMonitorStk[512/8];     /* 任务栈 */
/* 任务句柄 */
OS_TID HandleTaskStart = NULL;
OS_TID HandleTaskNet = NULL;
OS_TID HandleTaskMonitor = NULL;

/* 定时器句柄 */
OS_ID  OneTimerA;//定时器A

RunInfo_T g_tRunInfo;


/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int main(void) 
{	
	/* 初始化外设 */
	bsp_Init();
    
	/* 创建启动任务 */
 	os_sys_init_user(AppTaskStart,             /* 任务函数 */
	                  4,                        /* 任务优先级 */
	                  &AppTaskStartStk,         /* 任务栈 */
	                  sizeof(AppTaskStartStk)); /* 任务栈大小，单位字节数 */
	while(1);
}



/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	主要功能: 进行喂狗
*********************************************************************************************************
*/

__task void AppTaskStart(void)
{  
    uint16_t ret_flags;
    /* 获取启动任务的句柄 */
	HandleTaskStart = os_tsk_self();
    //通过start任务间接地创建其他任务
	AppTaskCreate();
	
    while(1)
    {        
        //采用或等待方式;退出前bit位被清除
        if(os_evt_wait_or(BIT_ALL, 0xFFFF) == OS_R_EVT)
        {
            ret_flags = os_evt_get();//返回值为对应的bit
            
            switch(ret_flags)
            {
                case TASK_NET_BIT://接收到任务事件标志位                 
                    //test
                    //IWDG_Feed();//喂狗
                    break;                   
                
                default:
                    break;
            }//end of switch
            
        }//end of if
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskNet
*	功能说明: 网络处理任务
*********************************************************************************************************
*/
__task void AppTaskNet(void)
{
    uint16_t ret_flags;
    const uint16_t usMaxBlockTime = 0x0FFF; /* 延迟周期,4095ms */
    while(1)
    {
        //采用或等待方式;退出前bit位被清除
        if(os_evt_wait_or(BIT_ALL, usMaxBlockTime) == OS_R_EVT)
        {
            ret_flags = os_evt_get();//返回值为对应的bit
            
            switch(ret_flags)
            {
                case LORA_RECV_BIT://接收到服务端命令                 
                    processCommand(g_tLora.buf, g_tLora.len);
                    break; 
                
                case GET_LEVEL_BIT://获取能级大小
                    //先延时，等待稳定，获取到能级后复位探测器
                    ResetDetector();
                    break;
                
                default:
                    break;
            }//end of switch
            
        }//end of if
        
        //判断标志位超时后执行的操作
        //向看门狗任务发送事件标志位
        os_evt_set(TASK_NET_BIT, HandleTaskStart);//post
    }  
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskMonitor
*	功能说明: 监测任务
*********************************************************************************************************
*/
__task void AppTaskMonitor(void)
{
    const uint16_t usMaxBlockTime = 3000; /* 延迟周期,3000s */
    while(1)
    {
        //若没有触发，但有能级，此时需要处理
        if(g_tRunInfo.isTriggered == 0)
        {
            if(GetDetectorLevel() != 0)
            {
                g_tRunInfo.readTimes++;
            }
            
            if(g_tRunInfo.readTimes == 3)
            {
                ResetDetector();
            }
        }
        
        os_dly_wait(usMaxBlockTime);
    }  
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{ 
	HandleTaskNet = os_tsk_create_user(AppTaskNet,              /* 任务函数 */ 
	                                   3,                       /* 任务优先级 */ 
	                                   &AppTaskNetStk,          /* 任务栈 */
	                                   sizeof(AppTaskNetStk));  /* 任务栈大小，单位字节数 */
    
//    HandleTaskMonitor = os_tsk_create_user(AppTaskMonitor,              /* 任务函数 */ 
//	                                   2,                       /* 任务优先级 */ 
//	                                   &AppTaskMonitorStk,          /* 任务栈 */
//	                                   sizeof(AppTaskMonitorStk));  /* 任务栈大小，单位字节数 */
}

/******************************************************/
