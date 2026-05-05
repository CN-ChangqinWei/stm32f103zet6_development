#ifndef _MULTI_MOTOR_SERVICE_H
#define _MULTI_MOTOR_SERVICE_H
#include<cmsis_os.h>
#include <stdint.h>
#include "multi_motor_domain.h"
#include "motor_domain.h"
#include "task_que.h"
// ========== 批量操作 Repository 接口表（抽象） ==========
typedef struct {
    char (*isMotorExists)(void* repo, int id);
    char (*setPosition)(void* repo, int id, uint32_t numAngel, uint32_t denAngel, uint32_t maxAngel);
    char (*setPositionByEncode)(void* repo, int id, int encode);
    char (*setPwm)(void* repo, int id, int pwmNum, int pwmDen);
    char (*setSpeedByAngel)(void* repo, int id, int spNumAngel, int spDenAngel);
    char (*powerOn)(void* repo, int id);
    char (*shutOff)(void* repo, int id);
} MultiMotorRepoInterface;

// ========== 服务结构体 ==========
typedef struct {
    void* repo;                           // 指向仓储实例
    MultiMotorRepoInterface interface;    // 仓储接口表
    TaskQue* tk;
} MultiMotorService;

// ========== 构造函数 + 业务入口 ==========
MultiMotorService* NewMultiMotorService(void* repo, MultiMotorRepoInterface interface);
MotorResult MultiMotorExec(void* service, void* arg);

#endif
