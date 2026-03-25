#include "servo_test.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal_tim.h"

void ServoTest(TIM_HandleTypeDef* tim,TIM_OC_InitTypeDef config,uint32_t channel){
    
    struct PWM pwm = NewPWM(tim,config,channel);
    struct Servo servo = NewServo(NULL,(uint16_t)0,pwm);
    uint32_t num=0;
    ServoSetPosition(&servo,num,360);
    while(1){
        HAL_Delay(10);
        num= (num+1)%360;
        ServoSetPosition(&servo,num,360);
    }

}