/*
*********************************************************************************************************
*
*	模块名称 : 主程序模块。
*	文件名称 : main.c(任务的优先级数值越小优先级越低，这个跟uCOS相反)

*********************************************************************************************************
*/

#include "bsp.h"			/* 底层硬件驱动 */
/*
**********************************************************************************************************
											函数声明
**********************************************************************************************************
*/
//#define HEART_TIME 15//心跳包周期为15s

static void AppTaskCreate (void);//创建除start任务外的其他任务
static void AppObjCreate (void);//用来初始化邮箱和软件定时器

    
//任务函数
__task void AppTaskReader(void);

__task void AppTaskFirst(void);
__task void AppTaskMulti(void);
__task void AppTaskInterLock(void);

__task void AppTaskKey(void);
__task void AppTaskButton(void);
__task void AppTaskNet(void);
__task void AppTaskStart(void);

/*
**********************************************************************************************************
											 变量
**********************************************************************************************************
*/
static uint64_t AppTaskReaderStk[512/8];   /* 任务栈 */

static uint64_t AppTaskFirstStk[512/8];   /* 任务栈 */
static uint64_t AppTaskMultiStk[512/8];     /* 任务栈 */
static uint64_t AppTaskInterLockStk[512/8];   /* 任务栈 */

static uint64_t AppTaskKeyStk[512/8];   /* 任务栈 */
static uint64_t AppTaskButtonStk[512/8];   /* 任务栈 */
static uint64_t AppTaskNetStk[512/8];     /* 任务栈 */
static uint64_t AppTaskStartStk[512/8];   /* 任务栈 */

/* 任务句柄 */
OS_TID HandleTaskReader = NULL;

OS_TID HandleTaskFirst = NULL;
OS_TID HandleTaskMulti = NULL;
OS_TID HandleTaskInterLock = NULL;

OS_TID HandleTaskKey = NULL;
OS_TID HandleTaskButton = NULL;
OS_TID HandleTaskNet = NULL;
OS_TID HandleTaskStart = NULL;

/* 声明支持8个消息的消息邮箱 */
os_mbx_declare (mailboxCardRX, 8);

os_mbx_declare (mailboxCardFirst, 8);
os_mbx_declare (mailboxCardMulti, 8);
os_mbx_declare (mailboxCardInterLock, 8);

/* 定时器句柄 */
OS_ID  OneTimerA;//操作继电器A
OS_ID  OneTimerB;//操作继电器B

uint8_t FB_data;

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
	                  8,                        /* 任务优先级 */
	                  &AppTaskStartStk,         /* 任务栈 */
	                  sizeof(AppTaskStartStk)); /* 任务栈大小，单位字节数 */
	while(1);
}


/*
*********************************************************************************************************
*	函 数 名: AppTaskReader
*	主要功能: 处理按键开门和远程开门事件
*********************************************************************************************************
*/
__task void AppTaskReader(void)
{
    const uint16_t usMaxBlockTime = 100; /* 延迟周期 */
    while(1)
    {
        os_dly_wait(usMaxBlockTime);
    }//end of while
}

//处理首卡的任务
__task void AppTaskFirst(void)
{

    const uint16_t usMaxBlockTime = 100; /* 延迟周期 */
    while(1)
    {
           
        os_dly_wait(usMaxBlockTime);
    }//end of while
}

//处理多重卡的任务
__task void AppTaskMulti(void)
{

    const uint16_t usMaxBlockTime = 100; /* 延迟周期 */
    while(1)
    {
        os_dly_wait(usMaxBlockTime);
    }//end of while
}


//处理互锁的任务
__task void AppTaskInterLock(void)
{  
    const uint16_t usMaxBlockTime = 100; /* 延迟周期 */
    while(1)
    {
        os_dly_wait(usMaxBlockTime);
    }//end of while
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskButton
*	主要功能: 处理按键开门和远程开门事件
*********************************************************************************************************
*/
__task void AppTaskButton(void)
{

    while(1)
    {
        
    }//end of while
}

//按键和反馈的扫描任务
__task void AppTaskKey(void)
{
    while(1)
    {     
        
        os_dly_wait(100);
        
    }//end of while
           
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskNet
*	功能说明: 网络处理任务，附加按键扫描,读取门吸反馈	
*********************************************************************************************************
*/
__task void AppTaskNet(void)
{
    while(1)
    {    
        
        /* os_itv_wait是周期性延迟，os_dly_wait是相对延迟。*/
		os_dly_wait(100);
    }//end of while
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	主要功能: 进行心跳包和喂狗，读DS1302时间，网络状态
*********************************************************************************************************
*/

__task void AppTaskStart(void)
{
    
    /* 获取启动任务的句柄 */
	HandleTaskStart = os_tsk_self();
    //通过start任务间接地创建其他任务
	AppTaskCreate();
    /* 创建任务通信机制 */
	AppObjCreate();
	
    while(1)
    {
        IWDG_Feed();//喂狗
        
        os_dly_wait(15000);
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
    HandleTaskReader = os_tsk_create_user(AppTaskKey,              /* 任务函数 */ 
                                       1,                       /* 任务优先级 */ 
                                       &AppTaskKeyStk,          /* 任务栈 */
                                       sizeof(AppTaskKeyStk));  /* 任务栈大小，单位字节数 */
    
    HandleTaskReader = os_tsk_create_user(AppTaskReader,              /* 任务函数 */ 
                                       6,                       /* 任务优先级 */ 
                                       &AppTaskReaderStk,          /* 任务栈 */
                                       sizeof(AppTaskReaderStk));  /* 任务栈大小，单位字节数 */
    
    HandleTaskFirst = os_tsk_create_user(AppTaskFirst,              /* 任务函数 */ 
                                       2,                       /* 任务优先级 */ 
                                       &AppTaskFirstStk,          /* 任务栈 */
                                       sizeof(AppTaskFirstStk));  /* 任务栈大小，单位字节数 */
    
    HandleTaskMulti = os_tsk_create_user(AppTaskMulti,              /* 任务函数 */ 
	                                   5,                       /* 任务优先级 */ 
	                                   &AppTaskMultiStk,          /* 任务栈 */
	                                   sizeof(AppTaskMultiStk));  /* 任务栈大小，单位字节数 */
    
	HandleTaskInterLock = os_tsk_create_user(AppTaskInterLock,              /* 任务函数 */ 
	                                   4,                       /* 任务优先级 */ 
	                                   &AppTaskInterLockStk,          /* 任务栈 */
	                                   sizeof(AppTaskInterLockStk));  /* 任务栈大小，单位字节数 */
    
    HandleTaskButton = os_tsk_create_user(AppTaskButton,              /* 任务函数 */ 
	                                   3,                       /* 任务优先级 */ 
	                                   &AppTaskButtonStk,          /* 任务栈 */
	                                   sizeof(AppTaskButtonStk));  /* 任务栈大小，单位字节数 */
    
	HandleTaskNet = os_tsk_create_user(AppTaskNet,              /* 任务函数 */ 
	                                   7,                       /* 任务优先级 */ 
	                                   &AppTaskNetStk,          /* 任务栈 */
	                                   sizeof(AppTaskNetStk));  /* 任务栈大小，单位字节数 */
}

/*
*********************************************************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 创建任务通信机制
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppObjCreate (void)
{
	/* 创建消息邮箱 */
	os_mbx_init(&mailboxCardRX, sizeof(mailboxCardRX));
    
    os_mbx_init(&mailboxCardFirst, sizeof(mailboxCardFirst));
    os_mbx_init(&mailboxCardMulti, sizeof(mailboxCardMulti));
    os_mbx_init(&mailboxCardInterLock, sizeof(mailboxCardInterLock));
}


/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
