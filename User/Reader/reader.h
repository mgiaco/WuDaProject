#ifndef _READER_H_
#define	_READER_H_

//̽�����ṹ��
typedef struct
{
    uint32_t tStart;//1pps����ʱ��
    uint32_t tStop;//̽��������ʱ��
    uint8_t preciseTime[7];//ǰ3�ֽڱ�ʾСʱ�����ӣ��룬��3�ֽڱ�ʾ΢��,���һ�ֽ�Ϊ�ܼ���Ϣ
}Reader_T;

extern Reader_T g_tReader;//�ṩ����������ʹ��
void ReaderInit(void);
void DetectorInit(void);
void ResetDetector(void);
uint8_t GetDetectorLevel(void);
#endif


