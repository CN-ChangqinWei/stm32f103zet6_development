#ifndef _MEMORY_POLL_H
#define _MEMORY_POLL_H
#define _MEMORY_POLL_MAX_BYTE 1024
#define _MEMORY_POLL_MAX_UNIT 100
#include <string.h>
typedef struct {
    char *start;
    int len;

}MemoryUnit;
typedef struct{
    MemoryUnit units[_MEMORY_POLL_MAX_UNIT];
    char *     startAddr;
    int end;
    int cnt;
    int remain;
    int unitCur;
    char isInit;
}MemoryUnitManager;

void MemoryPollInit();
void* MemoryPollAlloc(int size);
char MemoryPollFree(void* buf);

#endif
