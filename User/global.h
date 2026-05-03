#ifndef _GLOBAL_H
#define _GLOBAL_H
#include"defines.h"
#include"cmsis_os.h"
#include"motor.h"
extern Motor motors[3];
void GlobalInit();

#endif