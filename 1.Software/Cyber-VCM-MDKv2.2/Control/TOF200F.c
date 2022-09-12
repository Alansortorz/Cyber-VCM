#include "TOF200F.h"

extern Sensor_Original_Data Sensor_data;

/**
 * @brief TOF_OriData_Filter - TOF激光传感器数据滤波
 * @param unsigned short ori_tof_value - 激光传感器输出的原始数据
 * @retval ret_tof_distance - 经过处理后的数据
 * @note  
 */
uint16_t TOF_OriData_Filter(void)
{
	unsigned short ori_tof_value = Sensor_data.TOF_Counts;
	//printf("AAA:%d\n", ori_tof_value);
	static struct _1_ekf_filter ekf = {0.02, 0, 0, 0, 0.001, 0.543};
	uint16_t ret_tof_distance = 0;
	kalman_1(&ekf, (float)ori_tof_value);
	ret_tof_distance = (uint16_t)ekf.out;
	//printf("BBB:%d\n", ret_tof_distance);
	return ret_tof_distance;
}

/**
 * @brief TOF_HEXConvertToEngineerValye()
 * @param unsigned short distance - 激光传感器的输出值
 * @retval return ret_distance - 转化后的工程值
 * @note <1>传感器输出值为0-65535
 *		 <2>转化后的工程值为0-2m(0-2000mm)
 */
float TOF_HEXConvertToEngineerValye(void)
{
	uint16_t distance = TOF_OriData_Filter();
	//printf("BBB:%d\n", distance);
	if(distance > 65535){
		distance = 65535;
	}
	float k = (float)(2000.0 / 65535.0), ret_distance = 0.0;
	ret_distance = k * (float)distance;
	//printf("BBB:%0.3f\n", ret_distance);
	return ret_distance;
}


