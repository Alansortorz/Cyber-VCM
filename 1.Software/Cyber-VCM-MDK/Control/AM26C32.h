#ifndef __AM26C32_H
#define __AM26C32_H

#include "stm32f1xx_hal.h"

void VCMEncoderInit(void);
int32_t VCMEncoderGetCnt(void);
float VCMEncoderGetPosition(void);

#endif /* __AM26C32_H */

