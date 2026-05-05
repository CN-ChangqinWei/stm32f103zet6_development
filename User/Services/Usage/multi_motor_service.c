#include "multi_motor_service.h"
#include "portable.h"
#include<string.h>
typedef struct{
    int len;
    MotorDomain * domains;
}MultiMotorPackage;

static void MultiMotorSet(MultiMotorService* multiSvc,MotorDomain* motorDomains,int nums);

static void MultiMotorSet(MultiMotorService* multiSvc,MotorDomain* motorDomains,int nums){
    void* repo = multiSvc->repo;
    MotorResult finalResult;
    // 获取电机数量
    
    if (nums <= 0) {
        return ;
    }
    for (int i = 0; i < nums; i++) {
        MotorDomain* domain = &motorDomains[i];
        int id = domain->id;
        
        // ① 检查电机是否存在
        if (!multiSvc->interface.isMotorExists(repo, id)) {
            if (finalResult == Success) {
                finalResult = NoSuchDev;
            }
            continue;
        }
        
        // ② 根据电源状态操作
        if (domain->powerOn) {
            if (!multiSvc->interface.powerOn(repo, id)) {
                if (finalResult == Success) {
                    finalResult = Fail;
                }
                continue;
            }
        } else {
            if (!multiSvc->interface.shutOff(repo, id)) {
                if (finalResult == Success) {
                    finalResult = Fail;
                }
            }
            continue;
        }
        
        // ③ 根据模式执行相应操作
        char result = 0;
        switch (domain->mode) {
            case PositionAngelMode:
                result = multiSvc->interface.setPosition(repo, id,
                    domain->numAngel, domain->denAngel, domain->maxAngel);
                break;
            case PositionEncodeMode:
                result = multiSvc->interface.setPositionByEncode(repo, id, domain->encode);
                break;
            case SpeedEncodeMode:
                result = multiSvc->interface.setPwm(repo, id, domain->pwmNum, domain->pwmDen);
                break;
            case SpeedAngelMode:
                result = multiSvc->interface.setSpeedByAngel(repo, id, domain->spNumAngel, domain->spDenAngel);
                break;
            default:
                if (finalResult == Success) {
                    finalResult = ArgErr;
                }
                continue;
        }
        
        if (!result && finalResult == Success) {
            finalResult = Fail;
        }
    }
}

uint16_t MotorExecDelay(void *instance,void * data){
    if(NULL==instance||NULL==data) return 0;
    MultiMotorPackage* package =(MultiMotorPackage*)data;
    for(int i=0;i<package->len;i++){
        MultiMotorSet((MultiMotorService*)instance,package->domains,package->len);
    }
    vPortFree(package->domains);
    osDelay(2000);
}



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
   
    
    // 计算MotorDomain数组的起始地址 (紧跟在MultiMotorDomain后面)
    MotorDomain* motorDomains = (MotorDomain*)((uint8_t*)arg + sizeof(MultiMotorDomain));
    MultiMotorPackage* pkg = pvPortMalloc(sizeof(MultiMotorPackage));
    pkg->len = multiDomain->nums;
    pkg->domains = pvPortMalloc(sizeof(MotorDomain)*pkg->len);
    memcpy(pkg->domains,motorDomains,sizeof(MotorDomain)*pkg->len);
    TaskPackage task ={
        .func=MotorExecDelay,
        .data=pkg,
        .instance=service
    };
    TaskQueAdd(multiSvc->tk,task);
    
    return Success;
}

MultiMotorService* NewMultiMotorService(void* repo, MultiMotorRepoInterface interface) {
    if (repo == NULL) {
        return NULL;
    }
    
    MultiMotorService* service = (MultiMotorService*)pvPortMalloc(sizeof(MultiMotorService));
    if (service == NULL) {
        return NULL;
    }
    
    service->repo = repo;
    service->interface = interface;
    service->tk=NewTaskQue(100);
    TaskQueStart(service->tk,0);
    return service;
}


