#include"servo.h"
#include "pwm.h"


struct Servo NewServo(GPIO_TypeDef*powerCtrlPort,uint16_t powerCtrlPin,struct PWM pwm){
    struct Servo res={
       
        .num=0,
        .den=3600,
        .powerCtrlPort=powerCtrlPort,
        .powerCtrlPin=powerCtrlPin,
        .isPowerOn=0,
        .pwm = pwm
    };
    return res;
}

uint8_t ServoPowerOn(struct Servo* servo,uint32_t num,uint32_t den){
    if(servo == NULL){
        return 1;
    }
    if(servo->powerCtrlPort != NULL){
        HAL_GPIO_WritePin(servo->powerCtrlPort,servo->powerCtrlPin,GPIO_PIN_SET);
    }
    servo->isPowerOn = 1;
    PWMStart(&servo->pwm);
    return ServoSetPosition(servo,num,den);
}
uint8_t ServoShutDown(struct Servo* servo){
    if(servo == NULL){
        return 1;
    }
    if(PWMClose(&servo->pwm) != 0){
        return 1;
    }
    if(servo->powerCtrlPort != NULL){
        HAL_GPIO_WritePin(servo->powerCtrlPort,servo->powerCtrlPin,GPIO_PIN_RESET);
    }
    servo->isPowerOn = 0;
    PWMClose(&servo->pwm);
    return 0;
}
uint8_t 
ServoSetPosition(struct Servo* servo,uint32_t num,uint32_t den){
    if(servo == NULL || den == 0||den<num){
        return 1;
    }
    if(!servo->isPowerOn){
        if(ServoPowerOn(servo,num,den) != 0){
            return 1;
        }
        return 0;
    }
    servo->num=num;
    servo->den=den;
    uint32_t xden = servo->den*40;
    uint32_t xnum = 4*servo->num+den;
    
    
    if(PWMSetByRate(&servo->pwm,xnum,xden) != 0){
        return 1;
    }
    return 0;
}
uint8_t ServoSetPositionByAngle(struct Servo* servo,float angle){
    if(servo == NULL){
        return 1;
    }
    if(angle < 0){
        angle = 0;
    }
    if(angle > 360){
        angle = 360;
    }
    uint32_t num = (uint32_t)(angle * 10);
    uint32_t den = 3600;
    return ServoSetPosition(servo,num,den);
}
