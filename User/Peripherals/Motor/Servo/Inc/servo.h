#ifndef SERVO_H
#define SERVO_H
#include "stm32f1xx_hal.h"
#include <stdint.h>
#include "pwm.h"
#include"cmsis_os.h"
#include"motor.h"
typedef struct {
    //位置量 (num*360)/den 度
    int      num;
    int      den;
    int      maxAngle;
    //电源控制引脚 可有可无
    GPIO_TypeDef* powerCtrlPort;
    uint16_t      powerCtrlPin;
    int       isPowerOn;
    //定时器设置pwm
    PWM*    pwm;

}Servo;

Servo* NewServo(GPIO_TypeDef*powerCtrlPort,uint16_t powerCtrlPin, PWM* pwm);
void DeleteServo(Servo* servo);
int ServoPowerOn(Servo* servo);
int ServoPowerOnByAngle( Servo* servo,int num,int den);
int ServoShutDown(Servo* servo);
int ServoSetPosition( Servo* servo,int num,int den,int maxAngel);
int ServoSetPositionByAngle( Servo* servo,float angle);
MotorInterface ServoInterface();
// servo 不存在以下类型接口但其它的motor可能会存在
// int ServoSetPositionByEncode(Servo* servo,int encode);
// int ServoSetSpeed(Servo* servo,int pwmNum,int pwmDen);
// int ServoSetSpeedByEncode(Servo* servo,int spEncode);
// int ServoSetSpeedByAngel(Servo* servo,int num,int den);
#endif
