#ifndef SERVO_TEST_H
#define SERVO_TEST_H

#include "servo.h"
#include "stm32f1xx_hal_tim.h"
#include <stdint.h>

void ServoTest(TIM_HandleTypeDef* tim,TIM_OC_InitTypeDef config,uint32_t channel);

#endif
