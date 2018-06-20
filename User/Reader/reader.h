#ifndef _READER_H_
#define	_READER_H_

//探测器结构体
typedef struct
{
    uint32_t tStart;//1pps触发时刻
    uint32_t tStop;//探测器触发时刻
    uint8_t preciseTime[7];//前3字节表示小时，分钟，秒，后3字节表示微秒,最后一字节为能级信息
}Reader_T;

extern Reader_T g_tReader;//提供给其他函数使用
void ReaderInit(void);
void DetectorInit(void);
void ResetDetector(void);
uint8_t GetDetectorLevel(void);
#endif


