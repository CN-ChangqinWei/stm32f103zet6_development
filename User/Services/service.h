
#ifndef _SERVICE_H
#define _SERVICE_H
#include"serial_comm.h"
#include"router.h"
#include"protocol.h"
#include"communication.h"
typedef struct{
    Communication listener;
}Service;
void SerivceInit();
void ServiceExec();
void ServiceCommHanlder();
void ServiceComm(char* buf,int len);
#endif
