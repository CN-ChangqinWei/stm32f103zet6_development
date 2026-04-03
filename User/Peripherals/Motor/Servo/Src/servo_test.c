#include "servo_test.h"

#include "servo.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal_tim.h"

void ServoTest(TIM_HandleTypeDef* tim,TIM_OC_InitTypeDef config,uint32_t channel){
    
     PWM pwm = NewPWM(tim,config,channel);
     Servo servo = NewServo(NULL,(uint16_t)0,pwm);
    uint32_t num=0;
    ServoPowerOnByAngle(&servo,num,360);
    ServoSetPosition(&servo,num,360,0);
    uint16_t arr[]={0,180,360};
    while(1){
        for(int i=0;i<3;i++){
            HAL_Delay(2000);
            ServoSetPosition(&servo,arr[i],360,0);
        }
        
    }

}