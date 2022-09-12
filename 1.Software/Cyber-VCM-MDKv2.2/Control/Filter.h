#ifndef __FILTER_H
#define __FILTER_H

#include "main.h"

struct _1_ekf_filter
{
	float LastP;
	float Now_P;
	float out;
	float Kg;
	float Q;
	float R;	
};

typedef struct {
	uint16_t cnt;
	uint16_t input;
	uint16_t *average;
	uint8_t  max_cnt;
}MovAverage;

void kalman_1(struct _1_ekf_filter *, float); 
uint16_t AntiPulse_MovingAverage_Filter(MovAverage *_MovAverage);
uint16_t MovingAverage_Filter(MovAverage *_MovAverage);

#endif /* __FILTER_H */
