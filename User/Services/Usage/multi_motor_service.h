#ifndef _MULTI_MOTOR_SERVICE_H
#define _MULTI_MOTOR_SERVICE_H

#include <stdint.h>
#include "multi_motor_domain.h"
#include "motor_service.h"

// ========== 服务结构体 ==========
typedef struct {
    MotorService* motorService;  // 复用单个电机服务执行具体设置
} MultiMotorService;

// ========== 构造函数 + 业务入口 ==========
MultiMotorService* NewMultiMotorService(MotorService* motorService);
MotorResult MultiMotorExec(void* service, void* arg);

#endif
