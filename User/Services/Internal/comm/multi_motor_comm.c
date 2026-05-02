#include "multi_motor_comm.h"
#include "multi_motor_service.h"
#include "multi_motor_domain.h"
#include "service.h"

uint16_t MultiMotorHandler(void* service, void* arg) {
    if (service == NULL || arg == NULL) {
        return 1;
    }
    
    // 执行业务层
    MotorResult res = MultiMotorExec(service, arg);
    
    // 构造响应
    MultiMotorDomainReply reply;
    reply.protocol = PROTO_MULTI_MOTOR;
    reply.res = res;
    
    // 发送响应
    ServiceComm((char*)&reply, sizeof(MultiMotorDomainReply));
    
    return 0;
}
