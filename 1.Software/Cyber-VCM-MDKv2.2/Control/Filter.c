#include "filter.h"

/**
 * @brief �޷��˲�
 * @param int get_ad - ��������������ֵ
 * @retval char value - ����������
 * @note �ŵ㣺�ܹ���Ч�˷�żȻ����������������
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
**�������˲�
**@brief: �������������˲�
**@param[in]  InputData �˲�ǰ�����ݣ�QR���
**@param[out] None
**@return �˲��������
**@remark: ͨ���޸Ĺ��������Ͳ�������R,Qֵ�Ż����ֵ
X(k)=A X(k-1)+B U(k)+W(k)  
Z(k)=H X(k)+V(k)
AB��ϵͳ����
X�KKʱ�̵�ϵͳ״̬
H������ϵͳ�Ĳ���
Z��Kʱ�̵Ĳ���ֵ
WV�����̺Ͳ�������

X(k|k-1)=X(k-1|k-1) Ԥ����һ״̬��ϵͳ
P(k|k-1)=P(k-1|k-1) +Q  
Kg(k)= P(k|k-1) / (P(k|k-1) + R)   ����Kg����������
X(k|k)= X(k|k-1)+Kg(k) (Z(k)-X(k|k-1))   ����Ԥ��ֵ��Ϲ���ֵ�ó���ǰ״ֵ̬
P(k|k)=(1-Kg(k))P(k|k-1)  ����Э����


(k-1|k-1)��һ��״̬����������
(k|k-1) ����һ��״̬����������Ԥ�⵱ǰ״̬����������
(k|k)  ��Ԥ�Ȿ״̬����������ʵ����������

Q:ϵͳ���̵�Э����
R��������Э����
��˹�� = Q+R
Kg������������
P��Э����

ע:���������˲������Ե�ģ�ͣ�������H,I��Ϊ1��û�п�����U=0��ͨ����A,B��ʼֵȡ1
ע��X����������X(0|0)��ʼ����ֵ״̬���ݣ�����ǰ����ֵ������
ע :   P (0|0)һ�㲻ȡ0��ȡ0��ζ��0ʱ��ķ���Ϊ0��ϵͳ��Ϊ0ʱ�̵�ֵ�����ŵġ�Ȼ����ʵ��ϵͳ������0ʱ�̲������ŵ�
 **********************************************************************************************************************/

/**
 * @brief һά�������˲��㷨
 * @param struct _1_ekf_filter *ekf - �������˲�����
 * @retval None 
 * @note ����ֵͨ��ekf����ṹ��ָ�봫��
 */ 
void kalman_1(struct _1_ekf_filter *ekf, float input)  //һά������
{
	ekf->Now_P = ekf->LastP + ekf->Q;
	ekf->Kg = ekf->Now_P / (ekf->Now_P + ekf->R);
	ekf->out = ekf->out + ekf->Kg * (input - ekf->out);
	ekf->LastP = (1-ekf->Kg) * ekf->Now_P ;
}

 /**
 * @brief �������ͻ�����ֵ�˲�
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


