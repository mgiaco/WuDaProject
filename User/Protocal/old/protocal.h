#ifndef __PROTOCAL_H
#define __PROTOCAL_H

///* 供外部调用的函数声明 */
extern NetData_T g_tNetData;
//构造命令格式
void makeCommmand(uint8_t cmdFlag, uint8_t rw, uint8_t *data, uint16_t len);
//执行命令
void processCommand(uint8_t *data, uint16_t len);
//发送网络数据
void SendDataToServer(uint8_t flag, uint8_t rw, uint8_t *data, uint16_t len);
#endif

