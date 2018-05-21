#include "bsp.h" 
	
#define KEY_PORT    GPIOA

#define KEY0_PIN     GPIO_Pin_11
#define READ_KEY0    GPIO_ReadInputDataBit(KEY_PORT,KEY0_PIN)//读取按键0

#define KEY1_PIN     GPIO_Pin_12
#define READ_KEY1    GPIO_ReadInputDataBit(KEY_PORT,KEY1_PIN)//读取按键1

//按键初始化函数
void bsp_InitKey(void) //IO初始化
{   
 	GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能PORTA时钟
	
	GPIO_InitStructure.GPIO_Pin  = KEY0_PIN | KEY1_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(KEY_PORT, &GPIO_InitStructure);

}

void KEY_Scan()
{	   
    static uint8_t num0 = 0;
    static uint8_t num1 = 0;
    
    if(READ_KEY0==0)//KEY0 high default,0表示低电平
    {
        //先用static，被每100ms扫描一次，第二次再确认按下
	    //发送事件标志
        num0++;
        if(num0 == 2)
        {
            num0 = 0; 
            os_evt_set(BUTTON_A_BIT, HandleTaskButton);//post
        }
    } 
    
    if(READ_KEY1==0)
    {
        num1++;
        if(num1 == 2)
        {
            num1 = 0;
            os_evt_set(BUTTON_B_BIT, HandleTaskButton);//post
        }  
    }

}
