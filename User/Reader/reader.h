#ifndef _READER_H_
#define	_READER_H_

//̽�����ṹ��
typedef struct
{
    uint8_t level;//�ܼ���Ϣ
    uint32_t tStart;//1pps����ʱ��
    uint32_t tStop;//̽��������ʱ��
    uint8_t preciseTime[9];//ǰ6�ֽڱ�ʾСʱ�����ӣ��룬��3�ֽڱ�ʾ΢��
}Reader_T;

extern Reader_T g_tReader;//�ṩ����������ʹ��
void ReaderInit(void); 
#endif


