#ifndef __PARAM_H
#define __PARAM_H

#include "stm32f1xx_hal.h"
#include "stdbool.h"

typedef struct{
	uint16_t Encoder_Counts;
	uint16_t ADC_Nums;
	uint16_t TOF_Counts;
}Sensor_Original_Data;	

/**
 * @brief Main Parameters Used in The Pragram
 */
typedef struct{
	/* Actual Current Value */
	float Actual_Current;
	/* Actual Speed Value */
	float Actual_Speed;
	/* Actual Position Value */
	float Actual_Position;
    
	float Ctrl_Position;
}Main_Data; 

typedef struct{
	int Duty;  /* PWM duty */
	bool DIR;      /* direction */
}PWM_Control_Params;


#endif /* __PARAM_H */
