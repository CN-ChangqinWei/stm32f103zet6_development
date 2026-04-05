#include "motor.h"

Motor* NewMotor(void*instance,MotorInterface interface){
    if(NULL==instance) return NULL;
    Motor* res = (Motor*)pvPortMalloc(sizeof(Motor));

    if(NULL!=res){
        res->instance=instance;
        res->interface=interface;
    }
    return res;
}
