#include "VCMControl.h"
#include "tim.h"
#include "param.h"

Main_Data main_data; 
PWM_Control_Params pwm_control_params;

/** 
 * @brief vcm position init 
 * 
 *
 */
void Down_Move(uint8_t duty)
{
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, duty);
	HAL_Delay(50);
	HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_RESET);
}

void Up_Move(uint8_t duty)
{
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, duty);
	HAL_Delay(50);
	HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_SET);
}


/** 
 * @brief 音圈电机回到起始位置函数
 * @note 在回原点的时候尽可能柔和一点~
 */
void VCMGoToOrigin(void)
{
	Down_Move(3);
	HAL_Delay(1000);
	HAL_Delay(1000);
	HAL_Delay(1000);
	//最大占空比推死，让初始位置为0
	Down_Move(50);
	HAL_Delay(1000);
	HAL_Delay(1000);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
}



