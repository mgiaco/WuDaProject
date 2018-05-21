/*
*********************************************************************************************************
*
*	ģ������ : BSPģ��(For STM32F1XX)
*	�ļ����� : bsp.c
*	��    �� : V1.0
*	˵    �� : ����Ӳ���ײ���������ģ������ļ�����Ҫ�ṩ bsp_Init()��������������á��������ÿ��c�ļ������ڿ�
*			  ͷ	��� #include "bsp.h" ���������е���������ģ�顣
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2013-03-01 armfly   ��ʽ����
*		V1.1    2015-08-10 Eric2013 ����uCOS-III���躯��
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"
static void GetCPU_ID(uint8_t *data, uint8_t len);

RunInfo_T g_tRunInfo;

static void GetCPU_ID(uint8_t *data, uint8_t len)  
{  
    u32 CpuID[3];
    u32 mac_Code; 
    //u8 ID0,ID1,ID2,ID3;
    CpuID[0]=*(u32*)(0x1FFFF7E8);  
    CpuID[1]=*(u32*)(0x1FFFF7EC);  
    CpuID[2]=*(u32*)(0x1FFFF7F0);  
    mac_Code = (CpuID[0]>>1)+(CpuID[1]>>2)+(CpuID[2]>>3);
    data[0]=mac_Code&0x000000FF;
    data[1]=(mac_Code&0x0000FF00)>>8;
    data[2]=(mac_Code&0x00FF0000)>>16;
    data[3]=(mac_Code&0xFF000000)>>24;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_Init
*	����˵��: ��ʼ��Ӳ���豸��ֻ��Ҫ����һ�Ρ��ú�������CPU�Ĵ���������ļĴ�������ʼ��һЩȫ�ֱ�����
*			 ȫ�ֱ�����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_Init(void)
{
	/*
		����ST�̼���������ļ��Ѿ�ִ����CPUϵͳʱ�ӵĳ�ʼ�������Բ����ٴ��ظ�����ϵͳʱ�ӡ�
		�����ļ�������CPU��ʱ��Ƶ�ʡ��ڲ�Flash�����ٶȺͿ�ѡ���ⲿSRAM FSMC��ʼ����

		ϵͳʱ��ȱʡ����Ϊ72MHz�������Ҫ���ģ������޸� system_stm32f10x.c �ļ�
	*/
	
	/* ���ȼ���������Ϊ4��������0-15����ռʽ���ȼ���0�������ȼ����������������ȼ���*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    bsp_InitDWT();//��������ȷ΢����ʱ
    
	//bsp_InitUart(); 	/* ��ʼ������ */
	bsp_InitLed(); 		/* ��ʼLEDָʾ�ƶ˿� */
	bsp_InitKey();		/* ��ʼ������ */
	bsp_InitHardTimer();    /* Ӳ����ʱ����ʼ��*/   
    
    bsp_InitSPIBus();	/* ����SPI���� */		
	bsp_InitSFlash();	/* ��ʼ��SPI ����Flash */
    
    bsp_InitI2C();		/* ����I2C���� */
    WiegandInit();		//��ʼ��Τ���ź���
    
    ds1302_GPIO_Configuration();//1302ģ��ʱ�����ų�ʼ��   
    
    WIZ_SPI_Init();//��ʼ��w5500�õ���GPIO�ں�spi����
    Reset_W5500();//Ӳ����w5500
    
    paramInit();//��ȡ����
    GetCPU_ID(g_tParam.netCfg.mcuID, 4);//��ȡcpuID
    set_default(&g_tParam.netCfg);//����Ĭ�ϵ�MAC��IP��GW��SUB��DNS,����ʼ��
    
    bsp_InitIwdg(0xC35);//���Ź����ʱ��Ϊ20s
    
    //״̬��0
    g_tRunInfo.firstCardStatus = 0;
    g_tRunInfo.multipleCardStatus = 0;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
