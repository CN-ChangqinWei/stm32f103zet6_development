#include "router.h"
#include "memory_poll.h"
#include <string.h>
static Router router={0};
uint8_t RouterInit(){
    memset(&router, 0, sizeof(Router));
    return 0;
}//初始化全局router单例
uint8_t RouterExec(){
    if(router.taskHeadCur!=router.taskTailCur){
        Task tk=router.taskQue[router.taskHeadCur];
        
        if(tk.handler!=NULL)tk.handler(tk.arg);
        router.taskHeadCur++;
        router.taskHeadCur%=_ROUTER_MAX_TASK_CNT;
        MemoryPollFree(tk.arg);
    }
    
    
}
uint8_t RouterRegister(uint32_t protocol,RouterHandler handler){
    if(protocol>=_ROUTER_MAX_CNT) return 1;
    router.handlers[protocol]=handler;
}//注册handlerS

uint8_t RouterAddTask(Task tk){
    uint32_t check = (router.taskHeadCur+1)%(_ROUTER_MAX_TASK_CNT+1);
    if(check==router.taskTailCur) return 1;
    router.taskQue[router.taskTailCur]=tk;
    router.taskTailCur++;
    router.taskTailCur%=_ROUTER_MAX_TASK_CNT+1;
}
void  RouterAnlyPackage(void*package,int len){
    int protocol = *((int*)package);
    RouterHandler handler = router.handlers[protocol];
    Task tk ={handler,package};
    RouterAddTask(tk);
}

