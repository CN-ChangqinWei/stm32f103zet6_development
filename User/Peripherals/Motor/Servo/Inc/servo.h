#ifndef SERVO_H
#define SERVO_H
#include "stm32f1xx_hal.h"
#include <stdint.h>
#include "pwm.h"
typedef struct {
    //位置量 (num*360)/den 度
    uint32_t      num;
    uint32_t      den;
    uint32_t      maxAngle;
    //电源控制引脚 可有可无
    GPIO_TypeDef* powerCtrlPort;
    uint16_t      powerCtrlPin;
    uint8_t       isPowerOn;
    //定时器设置pwm
    PWM    pwm;

}Servo;

Servo NewServo(GPIO_TypeDef*powerCtrlPort,uint16_t powerCtrlPin, PWM pwm);
uint8_t ServoPowerOn(Servo* servo);
uint8_t ServoPowerOnByAngle( Servo* servo,uint32_t num,uint32_t den);
uint8_t ServoShutDown(Servo* servo);
uint8_t ServoSetPosition( Servo* servo,uint32_t num,uint32_t den,uint32_t maxAngel);
uint8_t ServoSetPositionByAngle( Servo* servo,float angle);
// servo 不存在以下类型接口但其它的motor可能会存在
// uint8_t ServoSetPositionByEncode(Servo* servo,int encode);
// uint8_t ServoSetSpeed(Servo* servo,int pwmNum,int pwmDen);
// uint8_t ServoSetSpeedByEncode(Servo* servo,int spEncode);
// uint8_t ServoSetSpeedByAngel(Servo* servo,int num,int den);
#endif
