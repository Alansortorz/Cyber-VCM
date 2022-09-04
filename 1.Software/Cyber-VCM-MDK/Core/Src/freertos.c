/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "SpeedPID.h"
#include "PositionPID.h"
#include "adc.h"
#include "MotorADC.h"
#include "tim.h"
#include "param.h"
#include "VCMControl.h"
#include "stdlib.h"
#include "AM26C32.h"
#include "tof200f_uart.h"
#include "oled.h"
#include "gpio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
float set_speed = 5.0;
float Motor_speed = 0.0;
float error_speed = 0.0;
float error_speed_I = 0.0;
float error_speed_old = 0.0;  //上一次的误差
float error_speed_old2 = 0.0;  //上上次的误差
float pre_position = 0.0;
float set_position = 0.3;

float Actual_Position = 0.0;
float error_position = 0.0;
float error_position_old = 0.0;
float error_position_old2 = 0.0;

/*float Kp_Speed = 20.0;
float Ki_Speed = 0.16;
float Kd_Speed = 0.0;*/

float Kp_Position = 40.0;
float Ki_Position = 8.0;
float Kd_Position = 0.0;

unsigned int distance;
float dis;

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern Main_Data main_data; 
extern PWM_Control_Params pwm_control_params;
/* USER CODE END Variables */
/* Definitions for GetDataTask */
osThreadId_t GetDataTaskHandle;
const osThreadAttr_t GetDataTask_attributes = {
  .name = "GetDataTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ShowDataTask */
osThreadId_t ShowDataTaskHandle;
const osThreadAttr_t ShowDataTask_attributes = {
  .name = "ShowDataTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for ControlMethodTa */
osThreadId_t ControlMethodTaHandle;
const osThreadAttr_t ControlMethodTa_attributes = {
  .name = "ControlMethodTa",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartGetDataTask(void *argument);
void StartShowDataTask(void *argument);
void StartControlMethodTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of GetDataTask */
  GetDataTaskHandle = osThreadNew(StartGetDataTask, NULL, &GetDataTask_attributes);

  /* creation of ShowDataTask */
  ShowDataTaskHandle = osThreadNew(StartShowDataTask, NULL, &ShowDataTask_attributes);

  /* creation of ControlMethodTa */
  ControlMethodTaHandle = osThreadNew(StartControlMethodTask, NULL, &ControlMethodTa_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartGetDataTask */
/**
  * @brief  Function implementing the GetDataTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartGetDataTask */
/**
* @brief 2022.5.4 控制目标设定任务
* @note set_position
*/
void StartGetDataTask(void *argument)
{
  /* USER CODE BEGIN StartGetDataTask */

  /* Infinite loop */
  for(;;)
  {
	  OLED_ShowNum(0, 32, distance, 5, 24, 1);
	  OLED_Refresh();
	  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
	  osDelay(50);
  }
  /* USER CODE END StartGetDataTask */
}

/* USER CODE BEGIN Header_StartShowDataTask */
/**
* @brief Function implementing the ShowDataTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartShowDataTask */
/**
* @brief 2022.5.2 数据交互任务
* @note 此任务中主要完成传感器数据获取，显示与输出操作
* <1> ADC采用中断模式，因为DMA不知道啥原因，数据出不来
*/
void StartShowDataTask(void *argument)
{
  /* USER CODE BEGIN StartShowDataTask */
  //char posi_buff[5], spee_buff[5], voil_buff[5], curr_buff[5];
  //float ADC_Value[2], fake_voil = 3.31;
  /* Infinite loop */
  for(;;)
  {
	  /*
	  main_data.Actual_Speed = GetVCMPreSpeed();
	  main_data.Actual_Position = GetVCMPositon();
	  pre_position = VCMEncoderGetPosition();
	  sprintf(spee_buff, "%0.3f", main_data.Actual_Speed);
	  sprintf(posi_buff, "%0.3f", main_data.Actual_Position);
	  OLED_ShowString(20, 28, (uint8_t*)posi_buff, 16, 1);
	  OLED_ShowString(20, 46, (uint8_t*)"        ", 16, 1);
	  OLED_ShowString(20, 46, (uint8_t*)spee_buff, 16, 1);
	  Get_ADC_Value(ADC_Value);
	  sprintf(curr_buff, "%0.3f", ADC_Value[0]);
	  OLED_ShowString(20, 62, (uint8_t*)curr_buff, 16, 1);
	  sprintf(voil_buff, "%0.2f", fake_voil);
	  OLED_ShowString(66, 4, (uint8_t*)voil_buff, 24, 1);
	  distance = tof200f_trans();
	  OLED_ShowNum(96, 28, distance, 4, 16, 1);
	  if(main_data.Actual_Speed > 0){
		  OLED_ShowString(86, 46, (uint8_t*)"DIR:+", 16, 1);
	  }else if(main_data.Actual_Speed < 0){
		  OLED_ShowString(86, 46, (uint8_t*)"DIR:-", 16, 1);
	  }else{
		  OLED_ShowString(86, 46, (uint8_t*)"DIR: ", 16, 1);
	  }
	  */
	  //OLED_Refresh();
	  main_data.Actual_Speed = GetVCMPreSpeed();
	  main_data.Actual_Position = GetVCMPositon();
	  pre_position = VCMEncoderGetPosition();

	  distance = tof200f_trans();
	  
	  osDelay(10);
  }
  /* USER CODE END StartShowDataTask */
}

/* USER CODE BEGIN Header_StartControlMethodTask */
/**
* @brief Function implementing the ControlMethodTa thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartControlMethodTask */
void StartControlMethodTask(void *argument)
{
  /* USER CODE BEGIN StartControlMethodTask */
  pwm_control_params.Duty = 0.0;  //Duty represent the force of VCM
  pwm_control_params.DIR = 1;     //DIR represent the direction of VCM
  //set_position = 0.1;
  //set_speed = 0.05;
  /* Infinite loop */

  for(;;)
  {
	  //main_data.Actual_Speed = GetVCMPreSpeed();
	  Actual_Position = VCMEncoderGetPosition();
	  //获取偏差
	  //error_speed = set_speed - main_data.Actual_Speed;
	  error_position = set_position - Actual_Position;
	  printf("%0.4f, %0.4f\n", set_position, Actual_Position);
	  //积分当前偏差
	  //error_speed_I += error_speed;

	  /*if(error_position > 0){
		  HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_RESET);
	  }
	  if(error_position < 0){
		  HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_SET);
	  }*/

	  //积分限幅
	  /*if(error_speed_I > 1000)
		  error_speed_I = 1000;
	  if(error_speed_I < -1000)
		  error_speed_I = -1000;*/

	  //增量式PID控制
	  //pwm_control_params.Duty += Kp*(error_speed-error_speed_old)+Ki*error_speed+Kd*(error_speed-2*error_speed_old+error_speed_old2);

	  //位置式PID控制
	  //pwm_control_params.Duty = Kp * error_speed + Ki * (error_speed_I) + Kd * (error_speed-error_speed_old);

	  //位置环增量式PID
	  pwm_control_params.Duty += Kp_Position * (error_position - error_position_old) + Ki_Position * error_position;

	  //速度更新
	  /*error_speed_old = error_speed;  // 本次速度给上次速度
	  error_speed_old2 = error_speed_old;*/

	  //位置更新
	  error_position_old = error_position;  // 本次速度给上次速度
	  error_position_old2 = error_position_old;

	   //error_speed_old2 = error_speed_old;
	   //error_speed_old = error_speed;
	  
	  
	  /*if(pwm_control_params.Duty > 50){
		  pwm_control_params.Duty = 50;
	  }
	  if(pwm_control_params.Duty < -50){
		  pwm_control_params.Duty = -50;
	  }*/
	  
	  
	  /*if(set_speed > 0){
		  HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_RESET);
	  }
	  if(set_speed < 0){
		  HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_SET);
	  }*/
		
	  if(error_position > 0){
		  HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_SET);
	  }
	  if(error_position < 0){
		  HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_RESET);
	  }
	  
	  //设置占空比
	  //__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, abs((int)pwm_control_params.Duty));
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, abs((int)pwm_control_params.Duty));
	  osDelay(20);
 
	}
  /* USER CODE END StartControlMethodTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
