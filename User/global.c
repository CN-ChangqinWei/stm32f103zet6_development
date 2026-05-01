#include"global.h"
#include"motor.h"
#include"motor_repo.h"
#include"motor_service.h"
#include"motor_comm.h"
#include "router.h"
#include"servo.h"
#include"pwm.h"
#include"stm32f1xx_hal.h"
#include"service.h"
#include"step.h"
// 外部声明已初始化的TIM句柄
extern TIM_HandleTypeDef htim1;

Motor motors[3]={0};
MotorRepo*    motorRepo=NULL;
MotorService* motorSrv=NULL;

int MotorInit(){
    // TIM_OC_InitTypeDef 配置 (标准PWM模式，已默认配置)
    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    InitStepMotor(&motors[0],0);
    // ========== Servo 0: PE9 (TIM1_CH1) ==========
    PWM* pwm0 = NewPWM(&htim1, sConfigOC, TIM_CHANNEL_1);
    if(pwm0 == NULL) return 1;
    
    Servo* servo0 = NewServo(NULL, 0, pwm0); // 无电源控制线
    if(servo0 == NULL) return 2;
    
    MotorInterface interface0 = ServoInterface();
    motors[1].instance = servo0;
    motors[1].interface = interface0;

    // ========== Servo 1: PE11 (TIM1_CH2) ==========
    PWM* pwm1 = NewPWM(&htim1, sConfigOC, TIM_CHANNEL_2);
    if(pwm1 == NULL) return 3;
    
    Servo* servo1 = NewServo(NULL, 0, pwm1); // 无电源控制线
    if(servo1 == NULL) return 4;
    
    MotorInterface interface1 = ServoInterface();
    motors[2].instance = servo1;
    motors[2].interface = interface1;

    // ========== 创建 MotorRepo ==========
    MotorRepo* repo = NewMotorReop(motors, 2); // 2个电机
    if(repo == NULL) return 5;

    // ========== 创建 MotorService ==========
    motorSrv = NewMotorService(repo, repo->interface);
    return 0;
}

void RoutesInit(){
   
   RouterHandlerPkg motorHandler={MotorHandler,motorSrv};
   RouterRegister(PROTO_MOTOR, motorHandler);
   #ifdef _DEBUG
   ServiceComm("motor service init fin\n",strlen("motor service init fin\n"));
   #endif
}

void GlobalInit(){
    SerivceInit();
    int err=0;
    if(err=MotorInit()){
        ServiceComm("motor init fail\n",strlen("motor init fail\n"));
        ServiceComm((char*)&err,sizeof(err));
        return;
    }else{
        //ServiceComm("motor init fin\n",strlen("motor init fin\n"));
    }

    RoutesInit();
}


