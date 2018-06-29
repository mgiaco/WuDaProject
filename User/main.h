#ifndef _MAIN_H_
#define _MAIN_H_

//����ʱ��״̬
typedef struct
{
    uint8_t isTriggered;//̽�����Ƿ񴥷�
    uint8_t readTimes;//��ȡ�ܼ�����
}RunInfo_T;

//�¼���־λ�궨��
#define BIT_ALL 0xFF
#define LORA_RECV_BIT	(1<<0)
#define TASK_NET_BIT	(1<<1)
#define REAET_LEVEL_BIT	(1<<2)

extern OS_TID HandleTaskNet;
extern RunInfo_T g_tRunInfo;
#endif

/******************************************/
