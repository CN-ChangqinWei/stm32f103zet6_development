#ifndef _STEP_MOTOR_H
#define _STEP_MOTOR_H

#include "motor.h"
#include "plus.h"
#include "stm32f1xx_hal.h"

// ========== 分数式角度结构 ==========
typedef struct {
    int num;    // 分子
    int den;    // 分母
} StepAngle;

// ========== 具体类实例结构体（对用户隐藏） ==========
typedef struct {
    int stepChannel;            // 步进电机通道
    Plus* plus;                 // Plus实例，用于发送脉冲
    
    // 分数式角度存储
    StepAngle curAngle;         // 当前角度（curAngle.num/curAngle.den * maxAngel）
    StepAngle stepAngle;        // 步进角度（每个脉冲转动的角度，分数式）
    int maxAngel;               // 最大角度（整数）
    
    // 方向控制
    GPIO_TypeDef* dirPort;      // 方向引脚端口
    uint16_t dirPin;            // 方向引脚

    int dirPolarity;            // 方向极性（+1或-1），用于控制转动方向

    GPIO_TypeDef* enPort;      // 使能端口
    uint16_t enPin;            // 使能引脚

    int enLevel;             //使能电平,什么电平才能使能
    
    
} StepMotorInstance;

// ========== 构造函数 ==========
Motor* NewStepMotor(int channel, Plus* plus, 
                    int initNum, int initDen,      // 初始角度（分数式 num/den）
                    int stepNum, int stepDen,      // 步进角度（分数式 num/den）
                    int maxAngel,                  // 最大角度值
                    GPIO_TypeDef* dirPort, uint16_t dirPin, int dirPolarity,
                    GPIO_TypeDef* enPort, uint16_t enPin, int enLevel);

void InitStepMotor(Motor* motor, int channel, Plus* plus,
                   int initNum, int initDen,
                   int stepNum, int stepDen,
                   int maxAngel,
                   GPIO_TypeDef* dirPort, uint16_t dirPin, int dirPolarity,
                   GPIO_TypeDef* enPort, uint16_t enPin, int enLevel);

// ========== 接口表初始化 ==========
MotorInterface StepMotorInterfaces(void);

#endif
