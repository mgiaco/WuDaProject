#ifndef _MAIN_H_
#define _MAIN_H_

//事件标志位宏定义
#define BIT_ALL 0xFF
#define REMOTE_OPEN_BIT	(1 << 3)
#define BUTTON_A_BIT	(1 << 1)
#define BUTTON_B_BIT	(1 << 2)
//#define READER_A_BIT	(1 << 3)
//#define READER_B_BIT	(1 << 4)

extern OS_TID HandleTaskButton;
extern os_mbx_declare (mailboxCardRX, 8);
#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
