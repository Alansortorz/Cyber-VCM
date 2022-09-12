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
 * @brief ��Ȧ����ص���ʼλ�ú���
 * @note �ڻ�ԭ���ʱ�򾡿������һ��~
 */
void VCMGoToOrigin(void)
{
	Down_Move(3);
	HAL_Delay(1000);
	HAL_Delay(1000);
	HAL_Delay(1000);
	//���ռ�ձ��������ó�ʼλ��Ϊ0
	Down_Move(50);
	HAL_Delay(1000);
	HAL_Delay(1000);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
}



