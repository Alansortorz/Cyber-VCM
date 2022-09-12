#include "filter.h"

/**
 * @brief 限幅滤波
 * @param int get_ad - 传感器数据输入值
 * @retval char value - 处理后的数据
 * @note 优点：能够有效克服偶然因素引起的脉冲干扰
 *		
 */ 
#define A 10
char value;
char filter(int get_ad)
{
	char new_value;
	new_value = get_ad;
	if((new_value - value> A) || (value - new_value > A))
	{
		return value;
	}
	else
	{
		return new_value;
	}
}

/*********************************************************************************************************************
**卡尔曼滤波
**@brief: 线性最优评估滤波
**@param[in]  InputData 滤波前的数据，QR误差
**@param[out] None
**@return 滤波后的数据
**@remark: 通过修改过程噪声和测量噪声R,Q值优化输出值
X(k)=A X(k-1)+B U(k)+W(k)  
Z(k)=H X(k)+V(k)
AB是系统参数
XK时刻的系统状态
H：测量系统的参数
Z：K时刻的测量值
WV：过程和测量噪声

X(k|k-1)=X(k-1|k-1) 预测下一状态的系统
P(k|k-1)=P(k-1|k-1) +Q  
Kg(k)= P(k|k-1) / (P(k|k-1) + R)   计算Kg卡尔曼增益
X(k|k)= X(k|k-1)+Kg(k) (Z(k)-X(k|k-1))   根据预测值结合估算值得出当前状态值
P(k|k)=(1-Kg(k))P(k|k-1)  更新协方差


(k-1|k-1)上一个状态的最优评估
(k|k-1) 由上一个状态的最优评估预测当前状态的最优评估
(k|k)  由预测本状态的评估具体实现最优评估

Q:系统过程的协方差
R：测量的协方差
高斯白 = Q+R
Kg：卡尔曼增益
P：协方差

注:本卡尔曼滤波器争对单模型，单测量H,I均为1，没有控制量U=0，通常对A,B初始值取1
注：X会逐渐收敛，X(0|0)初始测量值状态根据，测量前的数值而定。
注 :   P (0|0)一般不取0，取0意味在0时候的方差为0，系统认为0时刻的值是最优的。然而在实际系统中往往0时刻不是最优的
 **********************************************************************************************************************/

/**
 * @brief 一维卡尔曼滤波算法
 * @param struct _1_ekf_filter *ekf - 卡尔曼滤波参数
 * @retval None 
 * @note 返回值通过ekf这个结构体指针传递
 */ 
void kalman_1(struct _1_ekf_filter *ekf, float input)  //一维卡尔曼
{
	ekf->Now_P = ekf->LastP + ekf->Q;
	ekf->Kg = ekf->Now_P / (ekf->Now_P + ekf->R);
	ekf->out = ekf->out + ekf->Kg * (input - ekf->out);
	ekf->LastP = (1-ekf->Kg) * ekf->Now_P ;
}

 /**
 * @brief 抗干扰型滑动均值滤波
 * @param 
 * @retval 
 * @note 
 */
uint16_t AntiPulse_MovingAverage_Filter(MovAverage *_MovAverage)
{
	uint8_t i;	
	uint32_t sum=0;
	uint16_t max=0;
	uint16_t min=0xffff;

	_MovAverage->average[_MovAverage->cnt] = _MovAverage->input;	
	_MovAverage->cnt++;			
	if(_MovAverage->cnt==_MovAverage->max_cnt){
		_MovAverage->cnt=0;
	}	
	for(i=0;i<_MovAverage->max_cnt;i++){
		if(_MovAverage->average[i]>max)
			max = _MovAverage->average[i];
		else if(_MovAverage->average[i]<min)
			min = _MovAverage->average[i];
		sum += _MovAverage->average[i];
		}
	return ((sum-max-min)/(_MovAverage->max_cnt-2));  
}

uint16_t MovingAverage_Filter(MovAverage *_MovAverage)
{
	uint8_t i;
	uint32_t sum=0;
	_MovAverage->average[_MovAverage->cnt] = _MovAverage->input;	
	_MovAverage->cnt++;			
	if(_MovAverage->cnt==_MovAverage->max_cnt){
		_MovAverage->cnt=0;
	}
	for(i=0;i<_MovAverage->max_cnt;i++){
		sum += _MovAverage->average[i];
	}
	return (sum/_MovAverage->max_cnt);                                    
}


