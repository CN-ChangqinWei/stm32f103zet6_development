#include "step.h"
#include "portable.h"

// ========== 静态辅助函数 ==========

/**
 * @brief 设置方向引脚
 * @param inst 实例指针
 * @param direction +1为正方向，-1为负方向
 */
static void SetDirection(StepMotorInstance* inst, int direction) {
    if (inst == NULL || inst->dirPort == NULL) return;
    
    // 根据极性和方向计算实际电平
    // 如果极性为+1，正方向输出高电平；极性为-1，正方向输出低电平
    GPIO_PinState pinState = (direction * inst->dirPolarity > 0) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(inst->dirPort, inst->dirPin, pinState);
}

/**
 * @brief 设置使能引脚
 * @param inst 实例指针
 * @param enable 0: 禁用(失能), 非0: 使能
 */
static void SetEnable(StepMotorInstance* inst, int enable) {
    if (inst == NULL || inst->enPort == NULL) return;
    
    // 根据使能电平和enable标志计算实际电平
    // 如果enLevel为1，使能时输出高电平；enLevel为0，使能时输出低电平
    GPIO_PinState pinState = (enable ? inst->enLevel : !inst->enLevel) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(inst->enPort, inst->enPin, pinState);
}

/**
 * @brief 计算需要发送的脉冲数
 * @param targetNum 目标角度分子
 * @param targetDen 目标角度分母
 * @param curNum 当前角度分子
 * @param curDen 当前角度分母
 * @param stepNum 步进角度分子
 * @param stepDen 步进角度分母
 * @param maxAngel 最大角度
 * @return 脉冲数（正值为正方向，负值为负方向，0为无需移动）
 */
static int CalculateSteps(int targetNum, int targetDen,
                          int curNum, int curDen,
                          int stepNum, int stepDen,
                          int maxAngel) {
    if (targetDen == 0 || curDen == 0 || stepNum == 0) return 0;
    
    // 计算目标角度值（相对于maxAngel的分数）
    // 目标角度 = targetNum / targetDen * maxAngel
    // 当前角度 = curNum / curDen * maxAngel
    
    // 为避免浮点数，使用交叉相乘比较
    // 角度差 = (targetNum/targetDen - curNum/curDen) * maxAngel
    //       = (targetNum * curDen - curNum * targetDen) / (targetDen * curDen) * maxAngel
    
    int64_t diffNum = (int64_t)targetNum * curDen - (int64_t)curNum * targetDen;
    int64_t diffDen = (int64_t)targetDen * curDen;
    
    if (diffNum == 0) return 0;
    
    // 计算需要的脉冲数
    // 脉冲数 = 角度差 / 步进角度
    //       = (diffNum/diffDen * maxAngel) / (stepNum/stepDen)
    //       = diffNum * maxAngel * stepDen / (diffDen * stepNum)
    
    int64_t steps = diffNum * (int64_t)maxAngel * stepDen / (diffDen * stepNum);
    
    return (int)steps;
}

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
    StepMotorInstance* inst = (StepMotorInstance*)instance;
    if (inst == NULL || inst->plus == NULL) return -1;
    
    // 步进电机上电：使能电机
    SetEnable(inst, 1);
    
    // 设置初始方向
    SetDirection(inst, 1);
    return 0;
}

static int StepMotorShutDown(void* instance) {
    StepMotorInstance* inst = (StepMotorInstance*)instance;
    if (inst == NULL || inst->plus == NULL) return -1;
    
    // 步进电机关闭：先停止脉冲发送
    PlusStop(inst->plus);
    
    // 禁用电机
    SetEnable(inst, 0);
    return 0;
}

/**
 * @brief 分数式转动到指定角度
 * @param numAngel 目标角度分子
 * @param denAngel 目标角度分母
 * @param maxAngel 最大角度值
 */
static int StepMotorSetPosition(void* instance, int numAngel, int denAngel, int maxAngel) {
    StepMotorInstance* inst = (StepMotorInstance*)instance;
    if (inst == NULL || inst->plus == NULL) return -1;
    if (denAngel == 0) return -1;
    
    // 如果正在发送脉冲，返回错误
    if (PlusIsRunning(inst->plus)) {
        return -1;
    }
    
    // 计算需要发送的脉冲数
    int steps = CalculateSteps(numAngel, denAngel,
                               inst->curAngle.num, inst->curAngle.den,
                               inst->stepAngle.num, inst->stepAngle.den,
                               inst->maxAngel);
    
    if (steps == 0) {
        // 无需移动，直接更新当前角度
        inst->curAngle.num = numAngel;
        inst->curAngle.den = denAngel;
        return 0;
    }
    
    // 设置方向
    SetDirection(inst, steps > 0 ? 1 : -1);
    
    // 发送脉冲（取绝对值）
    uint16_t pulseCount = (steps > 0) ? (uint16_t)steps : (uint16_t)(-steps);
    if (PlusSend(inst->plus, pulseCount) != 0) {
        return -1;
    }
    
    // 更新当前角度
    inst->curAngle.num = numAngel;
    inst->curAngle.den = denAngel;
    
    return 0;
}

static int StepMotorSetPositionByEncode(void* instance, int encode) {
    // 步进电机不使用编码器位置设置
    (void)instance;
    (void)encode;
    return -1;
}

static int StepMotorSetSpeedByPwm(void* instance, int pwmNum, int pwmDen) {
    // 步进电机不使用PWM速度设置
    (void)instance;
    (void)pwmNum;
    (void)pwmDen;
    return -1;
}

static int StepMotorSetSpeedByAngel(void* instance, int spNumAngel, int spDenAngel) {
    // 步进电机不使用角度速度设置
    (void)instance;
    (void)spNumAngel;
    (void)spDenAngel;
    return -1;
}

static int StepMotorSetSpeedByEncode(void* instance, int encode) {
    // 步进电机不使用编码器速度设置
    (void)instance;
    (void)encode;
    return -1;
}

static void StepMotorDelete(void* p) {
    if (p != NULL) {
        StepMotorInstance* inst = (StepMotorInstance*)p;
        // 不删除plus实例，由外部管理
        vPortFree(p);
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
Motor* NewStepMotor(int channel, Plus* plus,
                    int initNum, int initDen,
                    int stepNum, int stepDen,
                    int maxAngel,
                    GPIO_TypeDef* dirPort, uint16_t dirPin, int dirPolarity,
                    GPIO_TypeDef* enPort, uint16_t enPin, int enLevel) {
    if (channel < 0 || plus == NULL) return NULL;
    if (initDen == 0 || stepNum == 0 || stepDen == 0) return NULL;
    if (dirPolarity != 1 && dirPolarity != -1) return NULL;
    if (enLevel != 0 && enLevel != 1) return NULL;
    
    StepMotorInstance* inst = (StepMotorInstance*)pvPortMalloc(sizeof(StepMotorInstance));
    if (inst == NULL) return NULL;
    
    inst->stepChannel = channel;
    inst->plus = plus;
    
    // 初始化当前角度（分数式）
    inst->curAngle.num = initNum;
    inst->curAngle.den = initDen;
    
    // 初始化步进角度（分数式）
    inst->stepAngle.num = stepNum;
    inst->stepAngle.den = stepDen;
    
    inst->maxAngel = maxAngel;
    
    // 初始化方向控制
    inst->dirPort = dirPort;
    inst->dirPin = dirPin;
    inst->dirPolarity = dirPolarity;
    
    // 初始化使能控制
    inst->enPort = enPort;
    inst->enPin = enPin;
    inst->enLevel = enLevel;
    
    // 初始状态禁用电机
    SetEnable(inst, 0);
    
    return NewMotor(inst, StepMotorInterfaces());
}

void InitStepMotor(Motor* motor, int channel, Plus* plus,
                   int initNum, int initDen,
                   int stepNum, int stepDen,
                   int maxAngel,
                   GPIO_TypeDef* dirPort, uint16_t dirPin, int dirPolarity,
                   GPIO_TypeDef* enPort, uint16_t enPin, int enLevel) {
    if (motor == NULL || channel < 0 || plus == NULL) return;
    if (initDen == 0 || stepNum == 0 || stepDen == 0) return;
    if (dirPolarity != 1 && dirPolarity != -1) return;
    if (enLevel != 0 && enLevel != 1) return;
    
    StepMotorInstance* inst = (StepMotorInstance*)pvPortMalloc(sizeof(StepMotorInstance));
    if (inst == NULL) return;
    
    inst->stepChannel = channel;
    inst->plus = plus;
    
    // 初始化当前角度（分数式）
    inst->curAngle.num = initNum;
    inst->curAngle.den = initDen;
    
    // 初始化步进角度（分数式）
    inst->stepAngle.num = stepNum;
    inst->stepAngle.den = stepDen;
    
    inst->maxAngel = maxAngel;
    
    // 初始化方向控制
    inst->dirPort = dirPort;
    inst->dirPin = dirPin;
    inst->dirPolarity = dirPolarity;
    
    // 初始化使能控制
    inst->enPort = enPort;
    inst->enPin = enPin;
    inst->enLevel = enLevel;
    
    // 初始状态禁用电机
    SetEnable(inst, 0);
    
    motor->instance = inst;
    motor->interface = StepMotorInterfaces();
}
