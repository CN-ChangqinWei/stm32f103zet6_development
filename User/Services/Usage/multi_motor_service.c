#include "multi_motor_service.h"
#include "portable.h"

/**
 * @brief 批量电机控制执行
 * @note 数据包格式: MultiMotorDomain + nums个MotorDomain
 * 
 * @param service MultiMotorService实例
 * @param arg 数据包指针 (MultiMotorDomain + MotorDomain数组)
 * @return MotorResult 执行结果
 */
MotorResult MultiMotorExec(void* service, void* arg) {
    if (service == NULL || arg == NULL) {
        return ArgErr;
    }
    
    MultiMotorService* multiSvc = (MultiMotorService*)service;
    MultiMotorDomain* multiDomain = (MultiMotorDomain*)arg;
    
    // 获取电机数量
    int nums = multiDomain->nums;
    if (nums <= 0) {
        return ArgErr;
    }
    
    // 计算MotorDomain数组的起始地址 (紧跟在MultiMotorDomain后面)
    MotorDomain* motorDomains = (MotorDomain*)((uint8_t*)arg + sizeof(MultiMotorDomain));
    
    // 依次设置每个电机
    MotorResult finalResult = Success;
    for (int i = 0; i < nums; i++) {
        MotorResult result = (MotorResult)MotorExec(
            multiSvc->motorService, 
            &motorDomains[i]
        );
        
        // 记录第一个错误结果
        if (result != Success && finalResult == Success) {
            finalResult = result;
        }
    }
    
    return finalResult;
}

MultiMotorService* NewMultiMotorService(MotorService* motorService) {
    if (motorService == NULL) {
        return NULL;
    }
    
    MultiMotorService* service = (MultiMotorService*)pvPortMalloc(sizeof(MultiMotorService));
    if (service == NULL) {
        return NULL;
    }
    
    service->motorService = motorService;
    return service;
}
