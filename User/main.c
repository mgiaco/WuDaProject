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
#define HEART_TIME 15//心跳包周期为15s

static void AppTaskCreate (void);//创建除start任务外的其他任务
static void AppObjCreate (void);//用来初始化邮箱和软件定时器

static void ReaderAlarm(enum ReaderOrButton_Enum type);//读头报警
static void SysLed(void);//系统状态灯闪烁
static void OpenTips(enum ReaderOrButton_Enum type);//开门提示音
    
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
    
    #if Test
//    bsp_LedOn(1);
//    bsp_LedOn(2);
    #endif
    
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
    uint8_t type;
    SingleReader_T *readerMsg;
    const uint16_t usMaxBlockTime = 100; /* 延迟周期 */
    while(1)
    {
        //最重要的部分
//        1.等待韦根函数发来的邮箱消息，先转发一次，然后自己再处理
//        2.判断多种开门功能的开关，把SingleReader_T放到其他邮箱中
//        3。首卡或多重卡或互锁收到邮箱消息后，解析ID，做出开门反应（多加几个邮箱和任务） 
//        if(A)//首卡
//        {转发给A}
//        if(B)//多重卡
//        {转发给B}
//        if(C)//互锁
//        {转发给C}
//        
//        type = searchID(readerMsg->ID);
//        if(type 为超级卡/码或胁迫卡/码)
//        {判断AB读头和AB继电器后，开门或报警}
        
        if(os_mbx_wait(&mailboxCardRX, (void *)&readerMsg, usMaxBlockTime) != OS_R_TMO)
        {
            
            if(g_tParam.systemCfg.multipleOpenCfg[1] == 1)//首卡已启用
            {
                g_tRunInfo.isCardUsed = 1;
                os_mbx_send(&mailboxCardFirst, readerMsg, 100);//向消息邮箱发数据，如果消息邮箱满了，等待100个时钟节拍
            }
            else if((g_tParam.systemCfg.multipleOpenCfg[2]&0x0F) == 1)//多重卡已启用//高四位表示卡数
            {
                g_tRunInfo.isCardUsed = 1;
                os_mbx_send(&mailboxCardMulti, readerMsg, 100);//向消息邮箱发数据，如果消息邮箱满了，等待100个时钟节拍
            }
            else if(g_tParam.systemCfg.multipleOpenCfg[0] == 1)//互锁已启用
            {
                g_tRunInfo.isCardUsed = 1;
                os_mbx_send(&mailboxCardInterLock, readerMsg, 100);//向消息邮箱发数据，如果消息邮箱满了，等待100个时钟节拍
            }
            else g_tRunInfo.isCardUsed = 0;
            
            if(!g_tRunInfo.isCardUsed)
            {
                //处理超级卡、码和胁迫卡、码
                switch(readerMsg->readerID)
                {
                case e_READER_A:                    
                    g_tReader.dataCheck(readerMsg);
                    type = searchID(readerMsg->ID);//searchID函数先读配置，再检测卡号有没有

                    if(type == e_superCardID || type == e_superPasswordID)//超级卡或超级密码随便开
                    {
                        if(g_tDoorStatus.doorA.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            //把卡号上传到服务器
                            SendDataToServer(0x06, e_READER_A, g_tReader.readerA.ID, 3);
                        }
                    }
                    if(type == e_threatCardID || type == e_threatPasswordID)//胁迫卡和胁迫码
                    {
                        if(g_tDoorStatus.doorA.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            
                            SendDataToServer(0x22, e_READER_A, g_tReader.readerA.ID, 3);//上传报警消息
                        }
                    }
                    if(type==e_keyBoardID || type==e_generalCardID || type==e_fingerID)
                    {
                        if(g_tDoorStatus.doorA.switcherStatus == NC)
                        {   
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            OpenTips(e_READER_A);
                            
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            //把卡号上传到服务器
                            //add3-8 读写位0表示A，1表示B
                            SendDataToServer(0x06, e_READER_A, g_tReader.readerA.ID, 3);
                        }
                    }
                    if(type == 0xFF)
                    {
                        //把卡号上传到服务器
                        SendDataToServer(0x06, e_READER_A, g_tReader.readerA.ID, 3);
                    }
                    break;
                
                case e_READER_B:
                    g_tReader.dataCheck(readerMsg);
                    type = searchID(readerMsg->ID);//searchID函数先读配置，再检测卡号有没有

                    if(type == e_superCardID || type == e_superPasswordID)//超级卡或超级密码随便开
                    {
                        if(g_tDoorStatus.doorB.switcherStatus == NC)
                        { 
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            OneTimerB = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            //把卡号上传到服务器
                            SendDataToServer(0x06, e_READER_B, g_tReader.readerB.ID, 3);
                        }
                    }
                    if(type == e_threatCardID || type == e_threatPasswordID)//胁迫卡和胁迫码
                    {
                        if(g_tDoorStatus.doorB.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                            
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            
                            SendDataToServer(0x22, e_READER_B, g_tReader.readerB.ID, 3);//上传报警消息
                        }
                    }
                    if(type==e_keyBoardID || type==e_generalCardID || type==e_fingerID)
                    {
                        if(g_tDoorStatus.doorB.switcherStatus == NC)
                        {              
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            
                            OneTimerB = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 3);
                            OpenTips(e_READER_B);
                            
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            //把卡号上传到服务器
                            SendDataToServer(0x06, e_READER_B, g_tReader.readerB.ID, 3);
                        }
                    }
                    if(type == 0xFF)
                    {
                        //把卡号上传到服务器
                        SendDataToServer(0x06, e_READER_B, g_tReader.readerB.ID, 3);
                    }
                    break;
                    
                
                default:
                    break;
            }//end of switch
        }//end of if
           
        //翻转系统状态灯
        SysLed();
            
        }//end of if
    }//end of while
}

//处理首卡的任务
__task void AppTaskFirst(void)
{
    uint8_t type;
    SingleReader_T *readerMsg;
    const uint16_t usMaxBlockTime = 100; /* 延迟周期 */
    while(1)
    {
        if(os_mbx_wait(&mailboxCardFirst, (void *)&readerMsg, usMaxBlockTime) != OS_R_TMO)
        {
            switch(readerMsg->readerID)
            {
                case e_READER_A:
                    g_tReader.dataCheck(readerMsg);
                    type = searchID(readerMsg->ID);//searchID函数先读配置，再检测卡号有没有
                    if(type == e_firstCardID)//首卡
                    {
                        g_tRunInfo.firstCardStatus = 1;//首卡已刷，其他卡可以继续刷
                        if(g_tDoorStatus.doorA.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            //把卡号上传到服务器
                            SendDataToServer(0x06, 0, g_tReader.readerA.ID, 3);
                        }
                    }
                    //权限高的卡号处理
                    if(type == e_superCardID || type == e_superPasswordID)//超级卡或超级密码随便开
                    {
                        if(g_tDoorStatus.doorA.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            //把卡号上传到服务器
                            SendDataToServer(0x06, e_READER_A, g_tReader.readerA.ID, 3);
                        }
                    }
                    if(type == e_threatCardID || type == e_threatPasswordID)//胁迫卡和胁迫码
                    {
                        if(g_tDoorStatus.doorA.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            
                            SendDataToServer(0x22, e_READER_A, g_tReader.readerA.ID, 3);//上传报警消息
                        }
                    }
                    //符合首卡条件
                    if(g_tRunInfo.firstCardStatus == 1)
                    {
                        if(type==e_keyBoardID || type==e_generalCardID || type==e_fingerID)
                        {
                            if(g_tDoorStatus.doorA.switcherStatus == NC)
                            {
                                g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                                OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                                
                                //存储记录到spi
                                storeRecord(readerMsg->ID, e_READER_A);
                                //把卡号上传到服务器
                                SendDataToServer(0x06, 0, g_tReader.readerA.ID, 3);
                            }
                        }
                    }
                    break;
                
                case e_READER_B:
                    g_tReader.dataCheck(readerMsg);
                    type = searchID(readerMsg->ID);//searchID函数先读配置，再检测卡号有没有
                    if(type == e_firstCardID)//首卡
                    {
                        g_tRunInfo.firstCardStatus = 1;//首卡已刷，其他卡可以继续刷
                        if(g_tDoorStatus.doorB.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            OneTimerB = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                                                              
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            //把卡号上传到服务器
                            SendDataToServer(0x06, 0, g_tReader.readerB.ID, 3);
                        }
                    }
                    //权限高的卡号处理
                    if(type == e_superCardID || type == e_superPasswordID)//超级卡或超级密码随便开
                    {
                        if(g_tDoorStatus.doorB.switcherStatus == NC)
                        { 
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            OneTimerB = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            //把卡号上传到服务器
                            SendDataToServer(0x06, e_READER_B, g_tReader.readerB.ID, 3);
                        }
                    }
                    if(type == e_threatCardID || type == e_threatPasswordID)//胁迫卡和胁迫码
                    {
                        if(g_tDoorStatus.doorB.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                            
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            
                            SendDataToServer(0x22, e_READER_B, g_tReader.readerB.ID, 3);//上传报警消息
                        }
                    }
                    //符合首卡条件
                    if(g_tRunInfo.firstCardStatus == 1)
                    {
                        if(type==e_keyBoardID || type==e_generalCardID || type==e_fingerID)
                        {
                            if(g_tDoorStatus.doorB.switcherStatus == NC)
                            {
                                g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                                OneTimerB = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                                                                  
                                //存储记录到spi
                                storeRecord(readerMsg->ID, e_READER_B);
                                //把卡号上传到服务器
                                SendDataToServer(0x06, 0, g_tReader.readerB.ID, 3);
                            }
                        }
                    }                 
                    break;
                
                default:
                    break;
            }//end of switch
        }//if
    }//end of while
}

//处理多重卡的任务
__task void AppTaskMulti(void)
{
    uint8_t type;
    SingleReader_T *readerMsg;
    const uint16_t usMaxBlockTime = 100; /* 延迟周期 */
    while(1)
    {
        if(os_mbx_wait(&mailboxCardMulti, (void *)&readerMsg, usMaxBlockTime) != OS_R_TMO)
        {
            switch(readerMsg->readerID)
            {
                case e_READER_A:
                    g_tReader.dataCheck(readerMsg);
                    type = searchID(readerMsg->ID);//searchID函数先读配置，再检测卡号有没有
                    if(type == e_multipleCardID)//首卡
                    {
                        //TODO:不能直接累加计数
                        g_tRunInfo.multipleCardStatus++;//卡已刷，累加计数
                    }
                    //权限高的卡号处理
                    if(type == e_superCardID || type == e_superPasswordID)//超级卡或超级密码随便开
                    {
                        if(g_tDoorStatus.doorA.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            //把卡号上传到服务器
                            SendDataToServer(0x06, e_READER_A, g_tReader.readerA.ID, 3);
                        }
                    }
                    if(type == e_threatCardID || type == e_threatPasswordID)//胁迫卡和胁迫码
                    {
                        if(g_tDoorStatus.doorA.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            
                            SendDataToServer(0x22, e_READER_A, g_tReader.readerA.ID, 3);//上传报警消息
                        }
                    }
                    //符合多重卡条件
                    if(g_tRunInfo.multipleCardStatus == (g_tParam.systemCfg.multipleOpenCfg[2]>>4))
                    {
                        
                        if(g_tDoorStatus.doorA.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            //把卡号上传到服务器
                            SendDataToServer(0x06, 0, g_tReader.readerA.ID, 3);      
                        }
                        g_tRunInfo.multipleCardStatus = 0;//开门后复位状态
                    }                 
                    break;
                
                case e_READER_B:
                    g_tReader.dataCheck(readerMsg);
                    type = searchID(readerMsg->ID);//searchID函数先读配置，再检测卡号有没有
                    if(type == e_multipleCardID)//首卡
                    {
                        g_tRunInfo.multipleCardStatus++;//卡已刷，累加计数
                    }
                    //权限高的卡号处理
                    if(type == e_superCardID || type == e_superPasswordID)//超级卡或超级密码随便开
                    {
                        if(g_tDoorStatus.doorB.switcherStatus == NC)
                        { 
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            OneTimerB = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            //把卡号上传到服务器
                            SendDataToServer(0x06, e_READER_B, g_tReader.readerB.ID, 3);
                        }
                    }
                    if(type == e_threatCardID || type == e_threatPasswordID)//胁迫卡和胁迫码
                    {
                        if(g_tDoorStatus.doorB.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                            
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            
                            SendDataToServer(0x22, e_READER_B, g_tReader.readerB.ID, 3);//上传报警消息
                        }
                    }
                    //符合多重卡条件
                    if(g_tRunInfo.multipleCardStatus == (g_tParam.systemCfg.multipleOpenCfg[2]>>4))
                    {
                        
                        if(g_tDoorStatus.doorB.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            OneTimerB = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                            
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            //把卡号上传到服务器
                            SendDataToServer(0x06, 0, g_tReader.readerB.ID, 3);
                        }
                        g_tRunInfo.multipleCardStatus = 0;//开门后复位状态
                    }
                    break;
                
                default:
                    break;
            }//end of switch
        }//if
    }//end of while
}


//处理互锁的任务
__task void AppTaskInterLock(void)
{
    uint8_t type;
    SingleReader_T *readerMsg;
    const uint16_t usMaxBlockTime = 100; /* 延迟周期 */
    while(1)
    {
        if(os_mbx_wait(&mailboxCardInterLock, (void *)&readerMsg, usMaxBlockTime) != OS_R_TMO)
        {
            switch(readerMsg->readerID)
            {
                case e_READER_A:
                    g_tReader.dataCheck(readerMsg);
                    type = searchID(readerMsg->ID);//searchID函数先读配置，再检测卡号有没有
                    //权限高的卡号处理
                    if(type == e_superCardID || type == e_superPasswordID)//超级卡或超级密码随便开
                    {
                        if(g_tDoorStatus.doorA.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            //把卡号上传到服务器
                            SendDataToServer(0x06, e_READER_A, g_tReader.readerA.ID, 3);
                        }
                    }
                    if(type == e_threatCardID || type == e_threatPasswordID)//胁迫卡和胁迫码
                    {
                        if(g_tDoorStatus.doorA.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            
                            SendDataToServer(0x22, e_READER_A, g_tReader.readerA.ID, 3);//上传报警消息
                        }
                    }
                    //判断互锁条件
                    if(type==e_keyBoardID || type==e_generalCardID || type==e_fingerID)
                    {
                        //开A门时，B门不能是打开的
                        if(g_tDoorStatus.doorA.switcherStatus == NC && g_tDoorStatus.doorB.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_READER_A);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 1);
                            
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_A);
                            //把卡号上传到服务器
                            SendDataToServer(0x06, 0, g_tReader.readerA.ID, 3);
                        }
                    }
                    break;
                
                case e_READER_B:
                    g_tReader.dataCheck(readerMsg);
                    type = searchID(readerMsg->ID);//searchID函数先读配置，再检测卡号有没有
                    //权限高的卡号处理
                    if(type == e_superCardID || type == e_superPasswordID)//超级卡或超级密码随便开
                    {
                        if(g_tDoorStatus.doorB.switcherStatus == NC)
                        { 
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            OneTimerB = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            //把卡号上传到服务器
                            SendDataToServer(0x06, e_READER_B, g_tReader.readerB.ID, 3);
                        }
                    }
                    if(type == e_threatCardID || type == e_threatPasswordID)//胁迫卡和胁迫码
                    {
                        if(g_tDoorStatus.doorB.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                            
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            
                            SendDataToServer(0x22, e_READER_B, g_tReader.readerB.ID, 3);//上传报警消息
                        }
                    }
                    //判断互锁条件
                    if(type==e_keyBoardID || type==e_generalCardID || type==e_fingerID)
                    {
                        //开B门时，B门不能是打开的
                        if(g_tDoorStatus.doorA.switcherStatus == NC && g_tDoorStatus.doorB.switcherStatus == NC)
                        {
                            g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_READER_B);
                            OneTimerB = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);
                            
                            //存储记录到spi
                            storeRecord(readerMsg->ID, e_READER_B);
                            //把卡号上传到服务器
                            SendDataToServer(0x06, 0, g_tReader.readerB.ID, 3);
                        } 
                    }                        
                    break;
                
                default:
                    break;
            }//end of switch
        }//if
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
    uint16_t ret_flags;
    SingleRelation_T t_remoteOpen;
    while(1)
    {
        //采用或等待方式;退出前bit位被清除
        if(os_evt_wait_or(BIT_ALL, 0xFFFF) == OS_R_EVT)
        {
            ret_flags = os_evt_get();//返回值为对应的bit
            SysLed();
            
            switch(ret_flags)
            {
                case REMOTE_OPEN_BIT://远程开门,套用按键开门方式                   
                    t_remoteOpen.button_switcher = g_tRunInfo.remoteOpen;
                    g_tDoorStatus.openDoor(&t_remoteOpen, e_BUTTON_A);
                    OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 0);  /* 定时1000*n个系统时钟节拍 ；0是回调函数的参数，可用于区分不同的定时器 */
                    break;
                
                case BUTTON_A_BIT://按键A
                    if(g_tDoorStatus.doorA.switcherStatus == NC)
                    {
                        g_tDoorStatus.openDoor(&g_tParam.relation.relationA, e_BUTTON_A);
                        OneTimerA = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 4);  /* 定时1000*n个系统时钟节拍 ；1是回调函数的参数，可用于区分不同的定时器 */
                    }
                    break;
                
                case BUTTON_B_BIT://按键B
                    if(g_tDoorStatus.doorB.switcherStatus == NC)
                    {
                        g_tDoorStatus.openDoor(&g_tParam.relation.relationB, e_BUTTON_B);
                        OneTimerB = os_tmr_create(1000 * g_tParam.systemCfg.openTime, 2);  /* 定时1000*n个系统时钟节拍 ；2是回调函数的参数，可用于区分不同的定时器 */
                    }
                    break; 
                
                default:
                    break;
            }//end of switch
            
            
        //翻转系统状态灯
        SysLed();
            
        }//end of if
    }
}

//按键和反馈的扫描任务
__task void AppTaskKey(void)
{
    static uint16_t timesA, timesB;
    while(1)
    {
        
        DetectReader();//检测读头连接状态
        g_tDoorStatus.readFeedBack();//读取门吸反馈
        //如果门反馈是开的且继电器是关的，那么N秒后调用报警，直到状态对
        //根据检测到的读头状态来，可能有未连接的，只对已连上的报警
        if(g_tReader.readerA.status == 1)
        {
            if(g_tDoorStatus.doorA.feedBackStatus == NO && g_tDoorStatus.doorA.switcherStatus == NC)
            {
                timesA++;
                //TODO:如果waitTime配成0，则不报警
                if((g_tParam.systemCfg.waitTime!=0) && (timesA >= g_tParam.systemCfg.waitTime*10))
                {
                    if(timesA == g_tParam.systemCfg.waitTime*10*25)
                    {
                        timesA = g_tParam.systemCfg.waitTime*10;//避免times溢出
                    }
                    //alram报警，os_dly和翻转
                    //利用读头上的蜂鸣器和led
                    ReaderAlarm(e_READER_A);
                }
            }
            else 
            {
                timesA = 0;
            }
        }
        
        if(g_tReader.readerB.status == 1)
        {
            if(g_tDoorStatus.doorB.feedBackStatus == NO && g_tDoorStatus.doorB.switcherStatus == NC)
            {
                timesB++;
                if((g_tParam.systemCfg.waitTime!=0) && (timesB >= g_tParam.systemCfg.waitTime*10))
                {
                    if(timesB == g_tParam.systemCfg.waitTime*10*25)
                    {
                        timesB = g_tParam.systemCfg.waitTime*10;//避免times溢出
                    }
                    //alram报警，os_dly和翻转
                    //利用读头上的蜂鸣器和led
                    ReaderAlarm(e_READER_B);
                }
            }
            else 
            {
                timesB = 0;
            }
        }
        
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
    static uint16_t n;
    static uint8_t data;
    static const uint8_t netDelayTime = 50;//网络任务每50ms一次
    
    static uint16_t heartNum = (HEART_TIME*1000)/netDelayTime;//心跳包延时次数
    
    while(1)
    {
        
        KEY_Scan();//按键检测
        //读取网络状态
        wiz_read_buf(PHYCFGR, &data, 1);//读取phy，判断网线是否插好
        if(data & 0x01 == 1)
        {
            g_tNetData.status = e_Link;//网线插好了

        }
        else g_tNetData.status = e_NoLink;//网线没插好
        
        if(g_tNetData.status == e_Link)
        {
            /*Socket状态机，MCU通过读Sn_SR(0)的值进行判断Socket应该处于何种状态*/
            switch(getSn_SR(0))	// 获取socket0的状态
            {
            case SOCK_UDP:		// Socket处于初始化完成(打开)状态
                if(getSn_IR(0) & Sn_IR_RECV)
                {
                    setSn_IR(0, Sn_IR_RECV);// Sn_IR的RECV位置1
                }
                //数据接收
                g_tNetData.len = getSn_RX_RSR(0);
                if((g_tNetData.len>0) && (g_tNetData.len <= (DATA_LEN+13+8)))
                { 
                    g_tNetData.len -= 8;//去除8字节长度的ip头信息
                    // W5500接收来自远程上位机的数据，并通过SPI发送给MCU
                    recvfrom(0, g_tNetData.buf, g_tNetData.len, g_tParam.netCfg.server_ip ,&g_tParam.netCfg.server_port);
                    DEBUG(COM1, g_tNetData.buf, g_tNetData.len);
                    processCommand(g_tNetData.buf, g_tNetData.len);
                }
                break;
                
            case SOCK_CLOSED:// Socket处于关闭状态
                socket(0, Sn_MR_UDP, g_tParam.netCfg.local_port, 0);	// 打开Socket0，并配置为UDP模式，打开一个本地端口
                break;
            
            default:
                break;         
            }//end of switch
            
            if(n >= heartNum)//等到网络任务达到一定次数后发送心跳包
            {
                n = 0;
                FB_data = g_tDoorStatus.doorA.feedBackStatus;
                FB_data += g_tDoorStatus.doorB.feedBackStatus << 4;
                SendDataToServer(0, 0, &FB_data, 1);//发送心跳包
            }
            n++;
        }
        
        /* os_itv_wait是周期性延迟，os_dly_wait是相对延迟。*/
		os_dly_wait(netDelayTime);
    }
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
        
        ds1302_readtime(g_tRunInfo.time, 5);//读取时间

        //判断是否到0点，需要复位首卡或者多重卡的状态
        if(g_tRunInfo.time[3] == 0 && (g_tRunInfo.firstCardStatus != 0 || g_tRunInfo.multipleCardStatus != 0))
        {
            //状态置0
            g_tRunInfo.firstCardStatus = 0;
            g_tRunInfo.multipleCardStatus = 0;
        }
 
        //翻转系统状态灯
        SysLed();
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

//报警
static void ReaderAlarm(enum ReaderOrButton_Enum type)
{
    alarmOn(type);
    os_dly_wait(500);
    alarmOff(type);
}

//系统状态灯闪烁
static void SysLed(void)
{
    bsp_LedToggle(3);
    os_dly_wait(500);
    bsp_LedToggle(3);
}

//开门提示音
static void OpenTips(enum ReaderOrButton_Enum type)
{
//    uint8_t delayTime = 50;
//    os_dly_wait(200);
//    
//    alarmOn(type);
//    os_dly_wait(delayTime);
//    alarmOff(type);
//    
//    os_dly_wait(delayTime);
//    
//    alarmOn(type);
//    os_dly_wait(delayTime);
//    alarmOff(type);
//    
//    os_dly_wait(delayTime);
//    
//    alarmOn(type);
//    os_dly_wait(delayTime);
//    alarmOff(type);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
