#ifndef _PWM_H
#define _PWM_H
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"
struct PWM{
    TIM_OC_InitTypeDef config;
    TIM_HandleTypeDef* tim;
    uint32_t channel;
};

struct PWM NewPWM(TIM_HandleTypeDef* tim,TIM_OC_InitTypeDef config,uint32_t channel);
uint8_t PWMStart(struct PWM* pwm);
uint8_t PWMClose(struct PWM* pwm);
uint8_t PWMSet(struct PWM* pwm,uint16_t pluse);
uint8_t PWMSetByRate(struct PWM* pwm,uint32_t num,uint32_t den);

#endif
