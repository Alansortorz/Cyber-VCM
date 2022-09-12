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
#include "filter.h"
#include "TOF200F.h"
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

float Kp_Position = 26.0;
float Ki_Position = 5.0;
float Kd_Position = 0.0;

Sensor_Original_Data Sensor_data;


uint16_t distance;
float Filter_distance;

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
void StartGetDataTask(void *argument)
{
  /* USER CODE BEGIN StartGetDataTask */
  //char posi_buff[5], spee_buff[5], voil_buff[5], curr_buff[5], tof_buff[5];
  //float ADC_Value[2], fake_voil = 3.31;
  /* Infinite loop */
  for(;;)
  {
	  
	  /*
	  distance = TOF_HEXConvertToEngineerValye();
	  sprintf(spee_buff, "%0.3f", main_data.Actual_Speed);
	  sprintf(posi_buff, "%0.3f", main_data.Actual_Position);
	  sprintf(curr_buff, "%0.3f", main_data.Actual_Current);
	  sprintf(voil_buff, "%0.2f", fake_voil);
	  sprintf(tof_buff, "%0.3f", distance);
	  OLED_ShowString(66, 0, (uint8_t*)voil_buff, 24, 1);
	  OLED_ShowString(20, 28, (uint8_t*)posi_buff, 16, 1);
	  OLED_ShowString(20, 46, (uint8_t*)"          ", 16, 1);
	  OLED_ShowString(20, 46, (uint8_t*)spee_buff, 16, 1);
	  OLED_ShowString(20, 64, (uint8_t*)curr_buff, 16, 1);
	  OLED_ShowString(88, 28, (uint8_t*)tof_buff, 16, 1);
	  if(main_data.Actual_Speed > 0){
		  OLED_ShowString(86, 46, (uint8_t*)"DIR:+", 16, 1);
	  }else if(main_data.Actual_Speed < 0){
		  OLED_ShowString(86, 46, (uint8_t*)"DIR:-", 16, 1);
	  }else{
		  OLED_ShowString(86, 46, (uint8_t*)"DIR: ", 16, 1);
	  }
	  */
	  //Get_ADC_Value(ADC_Value);
	  //sprintf(curr_buff, "%0.3f", ADC_Value[0]);
	  //OLED_ShowString(20, 62, (uint8_t*)curr_buff, 16, 1);
	  //printf("%0.3f, %0.3f\n", main_data.Actual_Position, distance);
	  //OLED_Refresh();
	  //HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
	  printf("%0.3f\n", Filter_distance);
	  osDelay(1);
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
void StartShowDataTask(void *argument)
{
  /* USER CODE BEGIN StartShowDataTask */
  /* Infinite loop */
  for(;;)
  {
	 Sensor_data.TOF_Counts = tof200f_trans();
	 Filter_distance = TOF_HEXConvertToEngineerValye();
	 main_data.Actual_Speed = GetVCMPreSpeed();
	 main_data.Actual_Position = GetVCMPositon();
	 pre_position = VCMEncoderGetPosition();
	 osDelay(40);
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
  /* Infinite loop */

  for(;;)
  {
	  Actual_Position = VCMEncoderGetPosition();
	  error_position = set_position - Actual_Position;

	  //位置环增量式PID
	  pwm_control_params.Duty += Kp_Position * (error_position - error_position_old) + Ki_Position * error_position;

	  //位置更新
	  error_position_old = error_position;  

	  // 本次速度给上次速度
	  error_position_old2 = error_position_old;

	  if(error_position > 0){
		  HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_SET);
	  }

	  if(error_position < 0){
		  HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_RESET);
	  }

	  //设置占空比
	  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, abs((int)pwm_control_params.Duty));
	  osDelay(1);
  }
  /* USER CODE END StartControlMethodTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
