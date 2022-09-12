#include "MotorADC.h"
#include "main.h"
#include "stdio.h"

extern ADC_HandleTypeDef hadc1;

#define FILEER_KP 0.9f
#define AD_TO_CURRENT 0.0032

struct SMotorADC_Struct{
	uint32_t adDmaValue[2];	//DMA原始数据
	int adValue[2];			//电流实际AD值
	int lastValue[2];		//上次电流实际AD值
};
struct SMotorADC_Struct gMotorADC = {0};

/**
  * @brief 开始一次DMA转换
  * 
  */
void MotorADCStartDMA(void)
{
	HAL_ADC_Start_DMA(&hadc1, gMotorADC.adDmaValue, 2);
	printf("Start DMA Finish\n");
}

void MotorADCValueStorage(void)
{
	gMotorADC.adValue[0] = gMotorADC.adDmaValue[0] - 2048;
	gMotorADC.adValue[1] = gMotorADC.adDmaValue[1] - 2048;
}

int32_t GetMotorADCPhaseXValue(uint8_t Phase)
{
	if(Phase > 3){
		return 0;
	}
	int32_t value;
	//MotorADCValueStorage();	//存储ADC转换值
	value = (int32_t)(FILEER_KP * (float)gMotorADC.adValue[Phase] + (1 - FILEER_KP) * (float)gMotorADC.lastValue[Phase]);
	gMotorADC.lastValue[Phase] = value;
	printf("----\n");
	return value;
}

float GetMotorPreCurrent(uint8_t Phase)
{
	float ADC_Value = GetMotorADCPhaseXValue(Phase) * AD_TO_CURRENT * 1.5;
	return ADC_Value;
}


