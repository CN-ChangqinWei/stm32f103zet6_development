#include"health_comm.h"
#include "health_service.h"
#include"service.h"
uint16_t HealthCommHandler(void* arg){
    int len;
    char* res=HealthExec(&len);
    ServiceComm(res,len);
    return 0;
}
