#ifndef _UART_H_
#define _UART_H_

//GPS--����1
#define UART1_BAUD			9600
#define GPS_DATA_LEN        128
//lora--����3
#define UART3_BAUD			9600
#define LORA_DATA_LEN       150

typedef enum
{
	  at_statIdle,
	  at_statRecving
}at_stateType;

typedef struct
{
    uint8_t len;
    uint8_t buf[GPS_DATA_LEN];
    uint8_t temp;
    at_stateType at_state;
    uint8_t atHead[2]; 
    uint8_t time[6];//GPSʱ��
    uint8_t status;
    uint8_t latitude[11];//γ�ȣ�3027.634058
    uint8_t longiitude[12];//���ȣ�11427.176258
}Gps_T;


typedef struct
{
    uint8_t len;
    uint8_t buf[LORA_DATA_LEN];
    uint8_t temp;
    at_stateType at_state;
    uint8_t atHead[2];  
}Lora_T;

extern Gps_T g_tGps;//�ṩ����������ʹ��
extern Lora_T g_tLora;//�ṩ����������ʹ��
void uartInit(void);
void LoraSendData(uint8_t *data, uint8_t len);
#endif

//------------------------
