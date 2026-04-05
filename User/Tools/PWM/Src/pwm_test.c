#include "pwm_test.h"
#include "stm32f1xx_hal_gpio.h"

void PWMTest(TIM_HandleTypeDef* tim,TIM_OC_InitTypeDef config,uint32_t channel){
    PWM* pwm= NewPWM(tim,config,channel);
    int num=0;
    int den=100;
    PWMStart(pwm);
    PWMSetByRate(pwm,num,den);
    // HAL_TIM_PWM_Start(tim,channel);
    while(1){
        HAL_Delay(20);
        num=(num+1)%(den+1);
        PWMSetByRate(pwm,num,den);
        // __HAL_TIM_SetCompare(tim,channel,num);
        // HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, !(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5)));
    }
}