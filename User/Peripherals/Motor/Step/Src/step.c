#include "step.h"
#include <stdlib.h>

// ========== 静态函数声明（前置声明） ==========
static int StepMotorPowerOn(void* instance);
static int StepMotorShutDown(void* instance);
static int StepMotorSetPosition(void* instance, int numAngel, int denAngel, int maxAngel);
static int StepMotorSetPositionByEncode(void* instance, int encode);
static int StepMotorSetSpeedByPwm(void* instance, int pwmNum, int pwmDen);
static int StepMotorSetSpeedByAngel(void* instance, int spNumAngel, int spDenAngel);
static int StepMotorSetSpeedByEncode(void* instance, int encode);
static void StepMotorDelete(void* p);

// ========== 具体函数实现（静态化，不暴露） ==========
static int StepMotorPowerOn(void* instance) {
    // TODO: 实现步进电机上电
    return 0;
}

static int StepMotorShutDown(void* instance) {
    // TODO: 实现步进电机关闭
    return 0;
}

static int StepMotorSetPosition(void* instance, int numAngel, int denAngel, int maxAngel) {
    // TODO: 实现步进电机角度位置设置
    return 0;
}

static int StepMotorSetPositionByEncode(void* instance, int encode) {
    // TODO: 实现步进电机编码器位置设置
    return 0;
}

static int StepMotorSetSpeedByPwm(void* instance, int pwmNum, int pwmDen) {
    // TODO: 实现步进电机PWM速度设置
    return 0;
}

static int StepMotorSetSpeedByAngel(void* instance, int spNumAngel, int spDenAngel) {
    // TODO: 实现步进电机角度速度设置
    return 0;
}

static int StepMotorSetSpeedByEncode(void* instance, int encode) {
    // TODO: 实现步进电机编码器速度设置
    return 0;
}

static void StepMotorDelete(void* p) {
    if (p != NULL) {
        free(p);
    }
}

// ========== 接口表初始化 ==========
MotorInterface StepMotorInterfaces(void) {
    MotorInterface interface = {
        .powerOn = StepMotorPowerOn,
        .shutDown = StepMotorShutDown,
        .setPosition = StepMotorSetPosition,
        .setPositionByEncode = StepMotorSetPositionByEncode,
        .setSpeedByPwm = StepMotorSetSpeedByPwm,
        .setSpeedByAngel = StepMotorSetSpeedByAngel,
        .setSpeedByEncode = StepMotorSetSpeedByEncode,
    };
    return interface;
}

// ========== 构造函数 ==========
Motor* NewStepMotor(int channel) {
    if (channel < 0) return NULL;
    
    StepMotorInstance* inst = (StepMotorInstance*)malloc(sizeof(StepMotorInstance));
    if (inst == NULL) return NULL;
    
    inst->stepChannel = channel;
    inst->currentStep = 0;
    inst->targetStep = 0;
    inst->speed = 0;
    
    return NewMotor(inst, StepMotorInterfaces());
}

void InitStepMotor(Motor* motor, int channel) {
    if (motor == NULL || channel < 0) return;
    
    StepMotorInstance* inst = (StepMotorInstance*)malloc(sizeof(StepMotorInstance));
    if (inst == NULL) return;
    
    inst->stepChannel = channel;
    inst->currentStep = 0;
    inst->targetStep = 0;
    inst->speed = 0;
    
    motor->instance = inst;
    motor->interface = StepMotorInterfaces();
}
