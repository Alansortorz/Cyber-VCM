#include "PositionPID.h"
#include "AM26C32.h"
#include "stdio.h"
/**
 * @breif Get motor actual positon
 * @param none
 * @retval the motor actual position
 * @note 
 */
float GetVCMPositon(void)
{
	return VCMEncoderGetPosition();
}


