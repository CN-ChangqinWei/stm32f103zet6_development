#ifndef _STEP_MOTOR_H
#define _STEP_MOTOR_H

#include "motor.h"

// ========== 具体类实例结构体（对用户隐藏） ==========
typedef struct {
    int stepChannel;        // 步进电机通道
    int currentStep;        // 当前步数
    int targetStep;         // 目标步数
    int speed;              // 转速
    // ... 其他具体字段
} StepMotorInstance;

// ========== 构造函数 ==========
Motor* NewStepMotor(int channel);
void InitStepMotor(Motor* motor, int channel);

// ========== 接口表初始化 ==========
MotorInterface StepMotorInterfaces(void);

#endif
