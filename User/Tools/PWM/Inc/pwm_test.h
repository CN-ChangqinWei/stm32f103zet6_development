#ifndef PWM_TEST_H
#define PWM_TEST_H

#include "stm32f1xx_hal.h"
#include "pwm.h"
void PWMTest(TIM_HandleTypeDef* tim,TIM_OC_InitTypeDef config,uint32_t channel);

#endif
