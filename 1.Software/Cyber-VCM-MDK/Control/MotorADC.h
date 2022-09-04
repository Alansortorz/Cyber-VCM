#ifndef __MOTORADC_H
#define __MOTORADC_H

#include "stm32f1xx_hal.h"

void MotorADCStartDMA(void);
void MotorADCValueStorage(void);
int32_t GetMotorADCPhaseXValue(uint8_t Phase);
float GetMotorPreCurrent(uint8_t Phase);

#endif /* __MOTORADC_H */
