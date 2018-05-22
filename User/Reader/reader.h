#ifndef _READER_H_
#define	_READER_H_

//探测器结构体
typedef struct
{
    uint8_t level;//能级信息
    uint32_t tStart;//1pps触发时刻
    uint32_t tStop;//探测器触发时刻
    uint8_t preciseTime[9];//前6字节表示小时，分钟，秒，后3字节表示微秒
}Reader_T;

extern Reader_T g_tReader;//提供给其他函数使用
void ReaderInit(void); 
#endif


