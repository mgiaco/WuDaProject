#include "bsp.h"

//初始化ADC功能
void AdcInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	__IO uint16_t ADCConvertedValue;

    /* 使能 ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);

	/* 配置PC2为模拟输入(ADC Channel2) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* 配置ADC1, 不用DMA, 用软件触发 */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* 配置ADC1, 规则通道2, 规则组采样顺序1，指定 ADC 通道的采样时间值55.5周期 */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_55Cycles5);

	/* 使能ADC1 DMA功能 */
	ADC_DMACmd(ADC1, ENABLE);

	/* 使能 ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* 使能ADC1 复位校准寄存器 */
	ADC_ResetCalibration(ADC1);
	/* 检查ADC1的复位寄存器 */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* 启动ADC1校准 */
	ADC_StartCalibration(ADC1);
	/* 检查校准是否结束 */
	while(ADC_GetCalibrationStatus(ADC1));

	/* 软件启动ADC转换 */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/*
*********************************************************************************************************
*	函 数 名: GetADC
*	功能说明: 读取ADC采样的平均值
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void GetADC(uint16_t *AdcValue)
{
	*AdcValue = ADC_GetConversionValue(ADC1);//把采样值存在AdcValue这个指针的内存上
    
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);	/* 软件启动下次ADC转换 */
}



