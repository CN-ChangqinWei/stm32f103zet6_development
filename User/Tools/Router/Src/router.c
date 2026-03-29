#include"router.h"
static Router router={0};
uint8_t RouterInit(){

}//初始化全局router单例
uint8_t RouterExec(){
    router.execFlag=1;
    while (router.execFlag)
    {
        if(router.taskHeadCur!=router.taskTailCur){
            Task tk=router.taskQue[router.taskHeadCur];
            tk.handler(tk.arg);
            router.taskHeadCur++;
            router.taskHeadCur%=_ROUTER_MAX_TASK_CNT;
        }
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


