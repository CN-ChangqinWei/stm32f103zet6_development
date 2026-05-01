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

int ServoPowerOnByAngle( Servo* servo,int num,int den){
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
int ServoShutDown( Servo* servo){
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
int ServoSetPosition( Servo* servo,int num,int den,int maxAngel){
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
    int xden = servo->den*40;
    int xnum = 4*servo->num+den;
    
    
    if(PWMSetByRate(servo->pwm,xnum,xden) != 0){
        return 1;
    }
    return 0;
}
int ServoSetPositionByAngle( Servo* servo,float angle){
    if(servo == NULL){
        return 1;
    }
    if(angle < 0){
        angle = 0;
    }
    if(angle > 360){
        angle = 360;
    }
    int num = (int)(angle * 10);
    int den = 3600;
    return ServoSetPosition(servo,num,den,0);
}

// 适配函数：powerOn
static int ServoAdapterPowerOn(void* instance){
    if(instance == NULL) return 1;
    // Servo默认上电到0度位置
    return ServoPowerOnByAngle((Servo*)instance, 0, 3600);
}

// 适配函数：shutDown
static int ServoAdapterShutDown(void* instance){
    if(instance == NULL) return 1;
    return ServoShutDown((Servo*)instance);
}

// 适配函数：setPosition
static int ServoAdapterSetPosition(void* instance, int numAngel, int denAngel, int maxAngel){
    if(instance == NULL) return 1;
    return ServoSetPosition((Servo*)instance, numAngel, denAngel, maxAngel);
}

// 适配函数：setPositionByEncode (Servo不支持编码器)
static int ServoAdapterSetPositionByEncode(void* instance, int encode){
    // Servo不支持编码器模式
    return 1;
}

// 适配函数：setSpeedByPwm (Servo不支持速度控制，但可以通过PWM设置位置)
static int ServoAdapterSetSpeedByPwm(void* instance, int pwmNum, int pwmDen){
    // Servo不支持速度控制模式
    return 1;
}

// 适配函数：setSpeedByAngel (Servo不支持速度控制)
static int ServoAdapterSetSpeedByAngel(void* instance, int spNumAngel, int spDenAngel){
    // Servo不支持速度控制模式
    return 1;
}

// 适配函数：setSpeedByEncode (Servo不支持编码器)
static int ServoAdapterSetSpeedByEncode(void* instance, int encode){
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