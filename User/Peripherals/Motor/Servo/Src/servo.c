#include"servo.h"
#include "motor.h"
#include "portable.h"
#include "pwm.h"


Servo* NewServo(GPIO_TypeDef*powerCtrlPort,uint16_t powerCtrlPin, PWM* pwm){
    Servo* res = pvPortMalloc(sizeof(Servo));
    if(res != NULL){
        res->num=0;
        res->den=3600;
        res->powerCtrlPin=powerCtrlPin;
        res->powerCtrlPort=powerCtrlPort;
        res->isPowerOn=0;
        res->pwm=pwm;
    }
    return res;
}

void DeleteServo(Servo* servo){
    if(servo != NULL){
        ServoShutDown(servo);
        vPortFree(servo);
    }
}

uint8_t ServoPowerOnByAngle( Servo* servo,uint32_t num,uint32_t den){
    if(servo == NULL || servo->pwm == NULL){
        return 1;
    }
    if(servo->powerCtrlPort != NULL){
        HAL_GPIO_WritePin(servo->powerCtrlPort,servo->powerCtrlPin,GPIO_PIN_SET);
    }
    servo->isPowerOn = 1;
    PWMStart(servo->pwm);
    return ServoSetPosition(servo,num,den,0);
}
uint8_t ServoShutDown( Servo* servo){
    if(servo == NULL){
        return 1;
    }
    if(servo->pwm != NULL){
        PWMClose(servo->pwm);
    }
    if(servo->powerCtrlPort != NULL){
        HAL_GPIO_WritePin(servo->powerCtrlPort,servo->powerCtrlPin,GPIO_PIN_RESET);
    }
    servo->isPowerOn = 0;
    return 0;
}
uint8_t ServoSetPosition( Servo* servo,uint32_t num,uint32_t den,uint32_t maxAngel){
    if(servo == NULL || servo->pwm == NULL || den == 0||den<num){
        return 1;
    }
    if(!servo->isPowerOn){
        if(ServoPowerOnByAngle(servo,num,den) != 0){
            return 1;
        }
        return 0;
    }
    servo->num=num;
    servo->den=den;
    uint32_t xden = servo->den*40;
    uint32_t xnum = 4*servo->num+den;
    
    
    if(PWMSetByRate(servo->pwm,xnum,xden) != 0){
        return 1;
    }
    return 0;
}
uint8_t ServoSetPositionByAngle( Servo* servo,float angle){
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
    return ServoSetPosition(servo,num,den,0);
}

// 适配函数：powerOn
static uint8_t ServoAdapterPowerOn(void* instance){
    if(instance == NULL) return 1;
    // Servo默认上电到0度位置
    return ServoPowerOnByAngle((Servo*)instance, 0, 3600);
}

// 适配函数：shutDown
static uint8_t ServoAdapterShutDown(void* instance){
    if(instance == NULL) return 1;
    return ServoShutDown((Servo*)instance);
}

// 适配函数：setPosition
static uint8_t ServoAdapterSetPosition(void* instance, uint32_t numAngel, uint32_t denAngel, uint32_t maxAngel){
    if(instance == NULL) return 1;
    return ServoSetPosition((Servo*)instance, numAngel, denAngel, maxAngel);
}

// 适配函数：setPositionByEncode (Servo不支持编码器)
static uint8_t ServoAdapterSetPositionByEncode(void* instance, int encode){
    // Servo不支持编码器模式
    return 1;
}

// 适配函数：setSpeedByPwm (Servo不支持速度控制，但可以通过PWM设置位置)
static uint8_t ServoAdapterSetSpeedByPwm(void* instance, int pwmNum, int pwmDen){
    // Servo不支持速度控制模式
    return 1;
}

// 适配函数：setSpeedByAngel (Servo不支持速度控制)
static uint8_t ServoAdapterSetSpeedByAngel(void* instance, int spNumAngel, int spDenAngel){
    // Servo不支持速度控制模式
    return 1;
}

// 适配函数：setSpeedByEncode (Servo不支持编码器)
static uint8_t ServoAdapterSetSpeedByEncode(void* instance, int encode){
    // Servo不支持编码器模式
    return 1;
}

MotorInterface ServoInterface(){
    MotorInterface interface={
        .powerOn = ServoAdapterPowerOn,
        .shutDown = ServoAdapterShutDown,
        .setPosition = ServoAdapterSetPosition,
        .setPositionByEncode = ServoAdapterSetPositionByEncode,
        .setSpeedByPwm = ServoAdapterSetSpeedByPwm,
        .setSpeedByAngel = ServoAdapterSetSpeedByAngel,
        .setSpeedByEncode = ServoAdapterSetSpeedByEncode
    };
    return interface;
}