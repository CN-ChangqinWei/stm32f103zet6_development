#ifndef SERVO_H
#define SERVO_H
#include "stm32f1xx_hal.h"
#include <stdint.h>
#include "pwm.h"
struct Servo{
    //位置量 (num*360)/den 度
    uint32_t      num;
    uint32_t      den;
    //电源控制引脚 可有可无
    GPIO_TypeDef* powerCtrlPort;
    uint16_t      powerCtrlPin;
    uint8_t       isPowerOn;
    //定时器设置pwm
    struct PWM    pwm;

};

struct Servo NewServo(GPIO_TypeDef*powerCtrlPort,uint16_t powerCtrlPin,struct PWM pwm);
uint8_t ServoPowerOn(struct Servo* servo,uint32_t num,uint32_t den);
uint8_t ServoShutDown(struct Servo* servo);
uint8_t ServoSetPosition(struct Servo* servo,uint32_t num,uint32_t den);
uint8_t ServoSetPositionByAngle(struct Servo* servo,float angle);

#endif
