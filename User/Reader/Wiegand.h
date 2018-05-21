#ifndef _WIEGAND_H_
#define	_WIEGAND_H_

//单个读头的数据结构体
typedef struct
{
    uint8_t readerID;
    uint8_t status;//读头状态，接上或没接
	volatile uint8_t writeIndex;//缓冲区写位置	
    volatile uint8_t isDelayTimerOpen;//延时定时器是否开启
    uint8_t ID[3];	//3字节ID号   
    uint8_t buf[26];//接收缓冲区
}SingleReader_T;

//所有读头
typedef struct
{
    SingleReader_T readerA;//读头A
    SingleReader_T readerB;//读头B
    uint8_t (*dataCheck)(SingleReader_T *reader);//初始化
}Reader_T;

extern Reader_T g_tReader;//提供给其他函数使用
void WiegandInit(void); 
void DetectReader(void);
#endif
