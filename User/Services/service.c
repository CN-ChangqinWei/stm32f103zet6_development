#include "service.h"
#include "router.h"
#include "health_comm.h"
#include "serial.h"

static Service service={0};
static SerialComm* serialComm=NULL;
void SerivceInit(){
    RouterInit();
    RouterHandlerPkg healthHandler = {HealthCommHandler,NULL};
    RouterRegister(Health, healthHandler);
    SerialsInit();
    serialComm=NewSerialComm(serial1);
    service.listener=NewCommunicationFromSerial(serialComm);
    if(service.listener != NULL){
        CommSendPackage(service.listener,"hello",strlen("hello"));
    }
}

void ServiceExec(){
    RouterExec();
}
void ServiceCommHanlder(){
    if(service.listener == NULL) return;
    int len=0;
    char* buf=CommRecvPackage(service.listener,&len);
    if(NULL==buf) return;
    RouterAnlyPackage(buf,len);
    CommSendPackage(service.listener,buf,len);
}
void ServiceComm(char* buf,int len){
    if(service.listener == NULL) return;
    CommSendPackage(service.listener,buf,len);
}
