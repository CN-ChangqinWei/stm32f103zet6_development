#include "communication.h"
#include "router.h"
#include <string.h>

Communication NewCommunication(void* instance, CommInterface interface) {
    Communication comm;
    comm.instance = instance;
    comm.interface = interface;
    comm.statu = MODE_LEN;
    comm.recvLen = 0;
    comm.packageBuf = NULL;
    return comm;
}

void* CommRecvPackage(Communication* comm, int* len) {
    uint32_t tempLen;
    
    if (comm->statu == MODE_LEN) {
        // 先接收4字节长度
        uint32_t recvBytes = comm->interface.recv(comm->instance, (uint8_t*)&tempLen, sizeof(uint32_t));
        if (recvBytes < sizeof(uint32_t)) {
            return NULL; // 长度未接收完整
        }
        
        comm->recvLen = tempLen;
        
        // 使用内存池创建相应的内存区域
        comm->packageBuf = (uint8_t*)MemoryPollAlloc(comm->recvLen);
        if (comm->packageBuf == NULL) {
            comm->recvLen = 0;
            return NULL; // 内存分配失败
        }
        
        // 切换到数据接收模式
        comm->statu = MODE_DATA;
        return NULL; // 长度接收完成，等待数据接收
        
    } else if (comm->statu == MODE_DATA) {
        // 接收数据写入内存
        uint32_t recvBytes = comm->interface.recv(comm->instance, comm->packageBuf, comm->recvLen);
        if (recvBytes < comm->recvLen) {
            return NULL; // 数据未接收完整
        }
        
        // 接收完全后返回指针
        uint8_t* result = comm->packageBuf;
        if (len != NULL) {
            *len = comm->recvLen;
        }
        
        // 重置状态，准备接收下一个包
        comm->statu = MODE_LEN;
        comm->recvLen = 0;
        comm->packageBuf = NULL;
        
        return result;
    }
    
    return NULL;
}

void CommSendPackage(Communication* comm, uint8_t* data, int len) {
    if (data == NULL || len <= 0) {
        return;
    }
    
    // 先发长度（4字节）
    uint32_t tempLen = (uint32_t)len;
    comm->interface.send(comm->instance, (uint8_t*)&tempLen, sizeof(uint32_t));
    
    // 再发数据
    comm->interface.send(comm->instance, data, len);
}

void CommHandler(Communication* comm){
    int len;
    uint8_t* data=CommRecvPackage(comm,&len);
    int protocol = *((int*)data);
    Task tk = {protocol,data};
    RouterAddTask(tk);
}

