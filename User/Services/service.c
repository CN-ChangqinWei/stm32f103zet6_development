#include "service.h"
#include "router.h"
#include "health_comm.h"
static Service service;
static SerialComm serialComm;
void SerivceInit(){

    RouterInit();
    RouterRegister(Health, HealthCommHandler);

    serialComm=NewSerialComm(&serial1);
    service.listener=NewCommunicationFromSerial(&serialComm);
    CommSendPackage(&service.listener,"hello",strlen("hello"));
}

void ServiceExec(){
    RouterExec();
}
void ServiceCommHanlder(){
    int len;
    char* buf=CommRecvPackage(&service.listener,&len);
    RouterAnlyPackage(buf,len);
    CommSendPackage(&service.listener,buf,len);
}
void ServiceComm(char* buf,int len){
    CommSendPackage(&service.listener,buf,len);
}
