#include"memory_poll.h"
static  char memory[_MEMORY_POLL_MAX_BYTE]={0};
static MemoryUnitManager manager={0};

void MemoryPollInit(){
    manager.startAddr=memory;
    manager.remain=_MEMORY_POLL_MAX_BYTE;
    manager.isInit=1;
}
void* MemoryPollAlloc(int size){
    if(manager.remain<size) return 0;
    if(manager.cnt>=_MEMORY_POLL_MAX_UNIT) return 0;
    if(manager.unitCur>=_MEMORY_POLL_MAX_UNIT) UnitsMigration();
    if(manager.end+size>_MEMORY_POLL_MAX_BYTE) MemoryMigration();
    char* res = manager.startAddr+manager.end;
    manager.units[manager.unitCur].len=size;
    manager.units[manager.unitCur++].start=res;
    manager.cnt++;
    manager.remain-=size;
    manager.end+=size;
    return res;
}
char MemoryPollFree(void* buf){
    for(int i=0;i<_MEMORY_POLL_MAX_UNIT;i++){
        if(manager.units[i].start==buf){
            int*len=&manager.units[i].len;
            manager.remain+=*len;
            manager.cnt--;
            *len=0;
            return 0;
        }
    }
    return 1;
}
static void UnitsMigration(){
    int cur=0;
    MemoryUnit *unitArry = manager.units;
    for(int i=0;i<_MEMORY_POLL_MAX_UNIT;i++){
        if(unitArry[i].len>0){
            if(i!=cur){
                unitArry[cur]=unitArry[i];
                unitArry[i].len=0;
            }
            cur++;
        }
    }
    manager.unitCur=cur;
}

static void MemoryMigration(){
    char * memCur=manager.startAddr;
    MemoryUnit *unitArry = manager.units;
    for(int i=0;i<_MEMORY_POLL_MAX_UNIT;i++){
        if(unitArry[i].len>0){
           if(unitArry[i].start!=memCur){
                memcpy(memCur,unitArry[i].start,unitArry[i].len);
           }
           unitArry[i].start=memCur;
           memCur+=unitArry[i].len;
        }
    }
    manager.end=memCur-manager.startAddr;
}

