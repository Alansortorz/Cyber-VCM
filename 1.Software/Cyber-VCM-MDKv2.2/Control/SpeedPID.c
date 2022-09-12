#include "SpeedPID.h"
#include "AM26C32.h"
#include "stdio.h"

/**
  * @brief 获取电机速度
  * @note 
  */
float GetVCMPreSpeed(void)
{
    static float position = 0.0f;
    static float lastposition = 0.0f;
    float speed;
    position = VCMEncoderGetPosition();
    speed = position - lastposition;
    lastposition = position;
    return speed;
}





