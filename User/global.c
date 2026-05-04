#include"global.h"
#include"motor.h"
#include"motor_repo.h"
#include"motor_service.h"
#include"motor_comm.h"
#include"multi_motor_service.h"
#include"multi_motor_comm.h"
#include "router.h"
#include"servo.h"
#include"pwm.h"
#include"plus.h"
#include"stm32f1xx_hal.h"
#include"service.h"
#include"step.h"
// 外部声明已初始化的TIM句柄
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim5;



Motor motors[3]={0};
MotorRepo*    motorRepo=NULL;
MotorService* motorSrv=NULL;
MultiMotorService* multiMotorSrv=NULL;
Plus* plus0=NULL;
int MotorInit(){
    // TIM_OC_InitTypeDef 配置 (标准PWM模式，已默认配置)
    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    
    // ========== Step Motor 0: htim2/htim5, PE12(dir), PE13(en) ==========
    // 初始角度 90/180 * 180 = 90度, 步进角 1/100 * 180 = 1.8度
    plus0 = NewPlus(&htim2, &htim5, TIM_CHANNEL_2);
    if(plus0 == NULL) return 1;
    
    Motor* stepMotor0 = NewStepMotor(0, plus0, 
                                      90, 180,      // 初始角度 90/180
                                      1, 400,       // 步进角 1/100 * 180 = 1.8度
                                      180,          // 最大角度180
                                      GPIOE, GPIO_PIN_12, 1,  // 方向 PE12, 极性+1
                                      GPIOE, GPIO_PIN_13, 0); // 使能 PE13, 高电平有效
    if(stepMotor0 == NULL) return 2;
    
    motors[0].instance = stepMotor0->instance;
    motors[0].interface = stepMotor0->interface;
    
    // ========== Servo 0: PE9 (TIM1_CH1) ==========
    PWM* pwm0 = NewPWM(&htim1, sConfigOC, TIM_CHANNEL_1);
    if(pwm0 == NULL) return 3;
    
    Servo* servo0 = NewServo(NULL, 0, pwm0); // 无电源控制线
    if(servo0 == NULL) return 4;
    
    MotorInterface interface0 = ServoInterface();
    motors[1].instance = servo0;
    motors[1].interface = interface0;

    // ========== Servo 1: PE11 (TIM1_CH2) ==========
    PWM* pwm1 = NewPWM(&htim1, sConfigOC, TIM_CHANNEL_2);
    if(pwm1 == NULL) return 5;
    
    Servo* servo1 = NewServo(NULL, 0, pwm1); // 无电源控制线
    if(servo1 == NULL) return 6;
    
    MotorInterface interface1 = ServoInterface();
    motors[2].instance = servo1;
    motors[2].interface = interface1;

    // ========== 创建 MotorRepo ==========
    MotorRepo* repo = NewMotorReop(motors, 3); // 3个电机
    if(repo == NULL) return 7;

    // ========== 创建 MotorService ==========
    motorSrv = NewMotorService(repo, repo->interface);
    if(motorSrv == NULL) return 8;
    
    // ========== 创建 MultiMotorService (复用 motorRepo) ==========
    MultiMotorRepoInterface multiInterface = {
        .isMotorExists = (char (*)(void*, int))isMotorExsits,
        .setPosition = (char (*)(void*, int, uint32_t, uint32_t, uint32_t))setPosition,
        .setPositionByEncode = (char (*)(void*, int, int))setPositionByEncode,
        .setPwm = (char (*)(void*, int, int, int))setPwm,
        .setSpeedByAngel = (char (*)(void*, int, int, int))setSpeedByAngel,
        .powerOn = (char (*)(void*, int))powerOn,
        .shutOff = (char (*)(void*, int))shutOff
    };
    multiMotorSrv = NewMultiMotorService(repo, multiInterface);
    if(multiMotorSrv == NULL) return 9;
    
    return 0;
}

void RoutesInit(){
   
   RouterHandlerPkg motorHandler={MotorHandler,motorSrv};
   RouterRegister(PROTO_MOTOR, motorHandler);
   
   RouterHandlerPkg multiMotorHandler={MultiMotorHandler,multiMotorSrv};
   RouterRegister(PROTO_MULTI_MOTOR, multiMotorHandler);
   
   #ifdef _DEBUG
   ServiceComm("motor service init fin\n",strlen("motor service init fin\n"));
   #endif
}

void GlobalInit(){
    SerivceInit();
    int err=0;
    if(err=MotorInit()){
        // ServiceComm("motor init fail\n",strlen("motor init fail\n"));
        // ServiceComm((char*)&err,sizeof(err));
        return;
    }else{
        //ServiceComm("motor init fin\n",strlen("motor init fin\n"));
    }

    RoutesInit();
}


