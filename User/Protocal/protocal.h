#ifndef __PROTOCAL_H
#define __PROTOCAL_H

/* ���ⲿ���õĺ������� */

//���������ʽ
void makeCommmand(uint8_t cmdFlag, uint8_t rw, uint8_t *data, uint8_t len);
//ִ������
void processCommand(uint8_t *data, uint16_t len);
//������������
void SendDataToServer(uint8_t flag, uint8_t rw, uint8_t *data, uint8_t len);
#endif

