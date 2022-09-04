#include "SpeedPID.h"
#include "AM26C32.h"
#include "retarget.h"

/**
  * @brief 获取电机速度
  *
  */
float GetVCMPreSpeed(void)
{
    static float position = 0.0f;
    static float lastposition = 0.0f;
    float speed;
    position = VCMEncoderGetPosition();
    speed = position - lastposition;
    if(speed < -180){
        speed = speed - 360;
    }
    if(speed > 180){
        speed = speed - 360;
    }
    lastposition = position;
    //printf("Speed:%0.2f\n", speed); //打印出assertion
    return speed;
}




